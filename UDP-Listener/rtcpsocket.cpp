#include "rtcpsocket.h"

class RtcpSocketPrivate
{
public:
    RtcpSocketPrivate(const QHostAddress &a, bool u) :
        address(a),
        unicast(u),
        mbps(0),
        maxMbps(0),
        averageMbps(0),
        nbBytesRx(0),
        lnbe(0)
    {
    }

    QHostAddress address;
    bool unicast;
    double mbps, maxMbps, averageMbps;
    quint32 nbBytesRx;
    quint64 lnbe;
    RTCPReport lastReport;
};

RtcpSocket::RtcpSocket(const QHostAddress &address, int port, bool unicast, QObject *parent) :
    QUdpSocket(parent),
    d(new RtcpSocketPrivate(address, unicast))
{
    startTimer(1000);

    connect(this, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));

    if (unicast) {
        if (!bind(address, port,
                  QAbstractSocket::ReuseAddressHint | QAbstractSocket::ShareAddress)) {
            write(QString("Failed to bind address %1:%2!\n").arg(address.toString()).arg(port));
        } else {
//            write(QString("Succeed to bind address %1:%2!\n").arg(address.toString()).arg(port));
        }
    } else {
        if (bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)) {
            if (joinMulticastGroup(address)) {
//                write(QString("Succeed to bind and join multicast group %1:%2!\n").arg(address.toString()).arg(port));
            } else
                write(QString("Failed to bind and join multicast group %1:%2!\n").arg(address.toString()).arg(port));
        } else {
            write(QString("Failed to bind to %1:%2!\n").arg(address.toString()).arg(port));
        }
    }
}

RtcpSocket::~RtcpSocket()
{
    delete d;
}


QString RtcpSocket::address() const
{
    return d->address.toString();
}

int RtcpSocket::port() const
{
    return localPort();
}

double RtcpSocket::mbps() const
{
    return d->mbps;
}

double RtcpSocket::maxMbps() const
{
    return d->maxMbps;
}

double RtcpSocket::averageMbps() const
{
    return d->averageMbps;
}


RTCPReport *RtcpSocket::lastReport() const
{
    return &d->lastReport;
}


void RtcpSocket::readPendingDatagrams()
{
    while (hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;

        datagram.resize(pendingDatagramSize());
        readDatagram(datagram.data(), datagram.size(),
                     &sender, &senderPort);

        d->lastReport = RTCPReport(datagram);

        emit reportAvailable(d->lastReport);
    }
}

void RtcpSocket::error(QAbstractSocket::SocketError)
{
    write(QString("ERROR: ").append(errorString()).append("\n"));
}

void RtcpSocket::timerEvent(QTimerEvent *)
{
    d->mbps = ((float)d->nbBytesRx * 8) / 1000000;
    d->maxMbps = d->mbps > d->maxMbps ? d->mbps : d->maxMbps;
    if (d->averageMbps == 0)
        d->averageMbps = d->mbps;
    else {
        d->averageMbps = ((d->averageMbps * d->lnbe) + d->mbps)/(++d->lnbe);
    }
    emit mbpsUpdated(d->mbps);
    d->nbBytesRx = 0;
}


void RtcpSocket::write(const QString &str)
{
    qDebug() << qPrintable(str);
}
