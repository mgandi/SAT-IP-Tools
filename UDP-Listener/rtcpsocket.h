#ifndef RTCPSOCKET_H
#define RTCPSOCKET_H

#include "rtcpreport.h"

#include <QUdpSocket>

class RtcpSocketPrivate;
class RtcpSocket : public QUdpSocket
{
    Q_OBJECT
public:
    explicit RtcpSocket(const QHostAddress &address, int port, bool unicast = true, QObject *parent = 0);
    ~RtcpSocket();

    QString address() const;
    int port() const;
    double mbps() const;
    double maxMbps() const;
    double averageMbps() const;

    RTCPReport *lastReport() const;

protected slots:
    void readPendingDatagrams();
    void error(QAbstractSocket::SocketError);
    void timerEvent(QTimerEvent *);

signals:
    void mbpsUpdated(const double mbps);
    void reportAvailable(const RTCPReport &report);

private:
    RtcpSocketPrivate *d;

    void write(const QString &str);

};

#endif // RTCPSOCKET_H
