#include "rtpsocket.h"

#include <QtEndian>
#include <QDateTime>

struct RtpHeader {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    quint8 type:7;
    quint8 m:1;
    quint8 cc:4;
    quint8 x:1;
    quint8 p:1;
    quint8 version:2;
#else
    quint8 version:2;
    quint8 p:1;
    quint8 x:1;
    quint8 cc:4;
    quint8 m:1;
    quint8 type:7;
#endif
    quint16 sn;
    quint32 ts;
    quint32 ssrc;
    quint32 csrc;

    quint8 getVersion () {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        return version;
#else
        return qFromBigEndian(version);
#endif
    }

    quint16 getSequenceNumber() {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        return sn;
#else
        return qFromBigEndian(sn);
#endif
    }
};

class RtpSocketPrivate
{
public:
    RtpSocketPrivate(const QHostAddress &a, bool u) :
        address(a),
        unicast(u),
        mbps(0),
        maxMbps(0),
        averageMbps(0),
        nbBytesRx(0),
        lnbe(0),
        packetCount(0),
        lastSequenceNumber(0),
        ssrc(0),
        nbLostPackets(0)
    {
    }

    QHostAddress address;
    bool unicast;
    double mbps, maxMbps, averageMbps;
    quint32 nbBytesRx;
    quint64 lnbe;
    quint16 packetCount;
    quint16 lastSequenceNumber;
    quint32 ssrc;
    quint64 nbLostPackets;
};

RtpSocket::RtpSocket(const QHostAddress &address, int port, bool unicast, QObject *parent) :
    QUdpSocket(parent),
    d(new RtpSocketPrivate(address, unicast))
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

RtpSocket::~RtpSocket()
{
    delete d;
}

quint16 RtpSocket::packetCount() const
{
    return d->packetCount;
}

quint64 RtpSocket::nbLostPackets() const
{
    return d->nbLostPackets;
}

QString RtpSocket::address() const
{
    return d->address.toString();
}

int RtpSocket::port() const
{
    return localPort();
}

double RtpSocket::mbps() const
{
    return d->mbps;
}

double RtpSocket::maxMbps() const
{
    return d->maxMbps;
}

double RtpSocket::averageMbps() const
{
    return d->averageMbps;
}


void RtpSocket::readPendingDatagrams()
{
    while (hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;

        // Extract datagram form socket
        datagram.resize(pendingDatagramSize());
        readDatagram(datagram.data(), datagram.size(),
                     &sender, &senderPort);

        // Record number of bytes received
        d->nbBytesRx += datagram.size();

        // Parse the datagaram
        parseDatagram(datagram);
    }
}

void RtpSocket::parseDatagram(QByteArray &datagram)
{
    RtpHeader *header = (RtpHeader *)datagram.left(sizeof(RtpHeader)).data();

    // Save SSRC
    if (d->ssrc == 0) {
        d->ssrc = header->ssrc;
    } else if (d->ssrc != header->ssrc) {
//        qWarning() << qPrintable(QDateTime::currentDateTime().toString("hh:mm:ss:zzz")) << " - RTP packet with wrong SSRC expected " << d->ssrc << "got " << header->ssrc;
    }

    // Increment packet count
    ++d->packetCount;

    // Check packets continuity
    quint16 sn = header->getSequenceNumber();
    if (d->lastSequenceNumber != sn) {
//        qWarning() << qPrintable(QDateTime::currentDateTime().toString("hh:mm:ss:zzz")) << " - RTP packet discontinuity expected " << d->lastSequenceNumber << "got " << sn;
        quint64 nbLost = (sn < d->lastSequenceNumber ? (65536 + ((quint32)sn)) : (sn - d->lastSequenceNumber));
        d->nbLostPackets += nbLost;
        d->lastSequenceNumber = sn;
    }
    ++d->lastSequenceNumber;

    // Remove RTP header
    datagram = datagram.right(datagram.size() - 12);

    // Collect TS packets
    QList<QByteArray> packets;
    qint32 index = 0;
    while (index < datagram.size()) {
        packets += QByteArray(datagram.mid(index, 188).data(), 188); // Make sure a deep copy of the data is done
        index += 188;
    }

    emit packetsAvailable(header->ssrc, packets);
}

void RtpSocket::error(QAbstractSocket::SocketError)
{
    write(QString("ERROR: ").append(errorString()).append("\n"));
}

void RtpSocket::timerEvent(QTimerEvent *)
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


void RtpSocket::write(const QString &str)
{
    qDebug() << qPrintable(str);
}
