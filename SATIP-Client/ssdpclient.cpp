#include "ssdpclient.h"
#include "gatewaydevice.h"

#include <QUdpSocket>
#include <QByteArray>
#include <QtDebug>
#include <QRegExp>
#include <QUrl>
#include <QUuid>

//#ifdef Q_WS_X11
#include <arpa/inet.h>
#include <netinet/in.h>
//#else
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#endif

class SSDPClientPrivate
{
public:
    SSDPClientPrivate() :
        ssdpAnswerRexgExp("HTTP/1\\.1 (\\d+) (.*)")
    {}
    QString address;
    QUdpSocket localSocket;
    QUdpSocket ssdpSocket;
    QRegExp ssdpAnswerRexgExp;
    QHash<QPair <QUuid, QUrl>, GatewayDevice *> devices;
};

SSDPClient::SSDPClient(const QString &address, QObject *parent) :
    QObject(parent),
    d(new SSDPClientPrivate)
{
    d->address = address;

    if (bind())
        searchDevices();
}

SSDPClient::~SSDPClient()
{
    delete d;
}

QString SSDPClient::localAddress() const
{
    return d->address;
}

void SSDPClient::searchDevices()
{
    d->localSocket.writeDatagram(QByteArray("M-SEARCH * HTTP/1.1\r\n"\
                                            "HOST: 239.255.255.250:1900\r\n"\
                                            "ST: urn:ses-com:device:SatIPServer:1\r\n"\
                                            "MAN: \"ssdp:discover\"\r\n"\
                                            "USER-AGENT: Platform/1.1 UPnP/1.1 BREngine/0.1\r\n"
                                            "MX: 2\r\n\r\n"), QHostAddress(0xEFFFFFFA), 1900);
}


bool SSDPClient::bind()
{
    bool ret = true;

    if (!d->localSocket.bind(QHostAddress(d->address), 0, QAbstractSocket::ReuseAddressHint | QAbstractSocket::ShareAddress)) {
        qWarning() << qPrintable(QString("Failed to bind address %1:%2!\n").arg(d->address).arg(0));
        ret &= false;
    } else {
        connect(&d->localSocket, SIGNAL(readyRead()),
                this, SLOT(readyRead()));
        connect(&d->localSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(error(QAbstractSocket::SocketError)));
    }

    if (d->ssdpSocket.bind(QHostAddress::AnyIPv4, 1900, QUdpSocket::ShareAddress)) {
        if (d->ssdpSocket.joinMulticastGroup(QHostAddress("239.255.255.250"))) {
            connect(&d->ssdpSocket, SIGNAL(readyRead()),
                    this, SLOT(readyRead()));
            connect(&d->ssdpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(error(QAbstractSocket::SocketError)));
        } else {
            qWarning() << qPrintable(QString("Failed to bind and join multicast group %1:%2!\n").arg(QHostAddress(0xEFFFFFFA).toString()).arg(1900));
            ret &= false;
        }
    } else {
        qWarning() << qPrintable(QString("Failed to bind to %1:%2!\n").arg(QHostAddress(0xEFFFFFFA).toString()).arg(1900));
        ret &= false;
    }

    return true;
}

void SSDPClient::error(QAbstractSocket::SocketError)
{
    QUdpSocket *localSocket = qobject_cast<QUdpSocket *>(QObject::sender());
    qCritical() << qPrintable(localSocket->errorString());
}

void SSDPClient::readyRead()
{
    QUdpSocket *socket = qobject_cast<QUdpSocket *>(QObject::sender());
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        parseDatagram(QString(datagram));
    }
}

void SSDPClient::parseDatagram(QString datagram)
{
    QStringList tokens = datagram.split("\r\n", QString::SkipEmptyParts);
    QString header = tokens.takeFirst();

    // Check if NOTIFY message
    if (header == "NOTIFY * HTTP/1.1") {
        parseSsdpNotify(tokens);
    }

    // check if anser to M-SEARCH
    else if (d->ssdpAnswerRexgExp.exactMatch(header)) {
        parseSsdpAnswer(tokens);
    }
}

void SSDPClient::parseSsdpNotify(QStringList &tokens)
{
    quint32 maxAge;
    QUrl location;
    QString nt;
    QString type;
    QString suuid;
    QUuid uuid;

    /* Capture only the third NOTIFY message */

    foreach (QString token, tokens) {
        if (token.startsWith("CACHE-CONTROL", Qt::CaseInsensitive)) {
            QRegExp regExp(".*:\\s*max-age\\s*=\\s*(\\d+)");
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                maxAge = regExp.cap(1).toUInt();
        }

        else if (token.startsWith("HOST", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("DATE", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("EXT", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("LOCATION", Qt::CaseInsensitive)) {
            QRegExp regExp("LOCATION\\s*:\\s*(\\w.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                location = QUrl(regExp.cap(1));
        }

        else if (token.startsWith("SERVER", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("NTS", Qt::CaseInsensitive)) {
            QRegExp regExp("NTS\\s*:\\s*ssdp:(.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                type = regExp.cap(1);
        }

        else if (token.startsWith("NT", Qt::CaseInsensitive)) {
            QRegExp regExp("NT\\s*:\\s*urn:(.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                nt = regExp.cap(1);
        }

        else if (token.startsWith("USN", Qt::CaseInsensitive)) {
            QRegExp regExp("USN\\s*:\\s*uuid:(.*)::urn:(.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 2) {
                suuid = regExp.cap(1);
                uuid = QUuid(regExp.cap(1));
            }
        }

        else if (token.startsWith("BOOTID.UPNP.ORG", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("CONFIGID.UPNP.ORG", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("SEARCHPORT.UPNP.ORG", Qt::CaseInsensitive)) {
        }
    }

    QPair<QUuid, QUrl> uuidAndLocation = QPair<QUuid, QUrl>(uuid, location);

    if ((nt == "ses-com:device:SatIPServer:1") && (type == "alive")) {
        if (!d->devices.contains(uuidAndLocation)) {
            GatewayDevice *device = new GatewayDevice(location, nt, this);
            d->devices.insert(uuidAndLocation, device);
            connect(device, SIGNAL(ready(GatewayDevice*)),
                    this, SIGNAL(newGatewayDeviceFound(GatewayDevice*)));
        }
    }

    else if ((nt == "ses-com:device:SatIPServer:1") && (type == "byebye")) {
        if (d->devices.contains(uuidAndLocation)) {
            GatewayDevice *device = d->devices.take(uuidAndLocation);
            disconnect(device, SIGNAL(ready(GatewayDevice*)),
                       this, SIGNAL(newGatewayDeviceFound(GatewayDevice*)));
            emit gatewayDeviceLeft(device);
            delete device;
        }
    }
}

void SSDPClient::parseSsdpAnswer(QStringList &tokens)
{
    quint32 maxAge;
    QUrl location;
    QString st;
    QUuid uuid;

    foreach (QString token, tokens) {
        if (token.startsWith("CACHE-CONTROL", Qt::CaseInsensitive)) {
            QRegExp regExp(".*:\\s*max-age\\s*=\\s*(\\d+)");
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                maxAge = regExp.cap(1).toUInt();
        }

        else if (token.startsWith("DATE", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("EXT", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("LOCATION", Qt::CaseInsensitive)) {
            QRegExp regExp("LOCATION\\s*:\\s*(\\w.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                location = QUrl(regExp.cap(1));
        }

        else if (token.startsWith("SERVER", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("ST", Qt::CaseInsensitive)) {
            QRegExp regExp("ST\\s*:\\s*urn:(.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 1)
                st = regExp.cap(1);
        }

        else if (token.startsWith("USN", Qt::CaseInsensitive)) {
            QRegExp regExp("USN\\s*:\\s*uuid:(.*)::urn:(.*)", Qt::CaseInsensitive);
            regExp.indexIn(token);
            if (regExp.captureCount() == 2)
                uuid = QUuid(regExp.cap(1));
        }

        else if (token.startsWith("BOOTID.UPNP.ORG", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("CONFIGID.UPNP.ORG", Qt::CaseInsensitive)) {
        }

        else if (token.startsWith("SEARCHPORT.UPNP.ORG", Qt::CaseInsensitive)) {
        }
    }

    QPair<QUuid, QUrl> uuidAndLocation = QPair<QUuid, QUrl>(uuid, location);

    if (st == "ses-com:device:SatIPServer:1") {
        if (!d->devices.contains(uuidAndLocation)) {
            GatewayDevice *device = new GatewayDevice(location, st, this);
            d->devices.insert(uuidAndLocation, device);
            connect(device, SIGNAL(ready(GatewayDevice*)),
                    this, SIGNAL(newGatewayDeviceFound(GatewayDevice*)));
        }
    }
}
