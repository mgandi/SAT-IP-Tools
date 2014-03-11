#ifndef RTPSOCKET_H
#define RTPSOCKET_H

#include <QUdpSocket>

class RtpSocketPrivate;
class RtpSocket : public QUdpSocket
{
    Q_OBJECT
public:
    explicit RtpSocket(const QHostAddress &address, int port = 0, bool unicast = true, QObject *parent = 0);
    ~RtpSocket();

    quint16 packetCount() const;
    quint64 nbLostPackets() const;

    QString address() const;
    int port() const;
    double mbps() const;
    double maxMbps() const;
    double averageMbps() const;

protected slots:
    void readPendingDatagrams();
    void parseDatagram(QByteArray &datagram);
    void error(QAbstractSocket::SocketError);
    void timerEvent(QTimerEvent *);

signals:
    void mbpsUpdated(const double mbps);
    void packetsAvailable(quint32 ssrc, QList<QByteArray> &packets);
    
private:
    RtpSocketPrivate *d;

    void write(const QString &str);
};

#endif // RTPSOCKET_H
