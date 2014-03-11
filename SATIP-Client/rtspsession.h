#ifndef RTSPSESSION_H
#define RTSPSESSION_H

#include "rtcpreport.h"

#include <QThread>
#include <QHostAddress>
#include <QThread>

class RtcpSocket;
class RtpSocket;
class RTSPSessionPrivate;
class RTSPSession : public QThread
{
    Q_OBJECT
public:
    explicit RTSPSession(const QString &localAddress, const QUrl &url, bool unicast = true, bool startPlaying = true, QObject *parent = 0);
    ~RTSPSession();

    QString session() const;
    quint32 streamid() const;
    bool unicast() const;
    QString source() const;
    QString destination() const;
    int rtpPort() const;
    int rtcpPort() const;

    RtpSocket *rtpSocket() const;
    RtcpSocket *rtcpSocket() const;
    
signals:
    void statisticsAvailable(quint64 dataRate, quint64 nbPacketReceived);
    void packetsAvailable(quint32 ssrc, QList<QByteArray> &packets);
    void rtcpReportAvailable(const RTCPReport &report);
    void sessionUpdated();
    
public slots:
    bool ready() const;
    void setUrl(const QUrl &url);
    void zapUrl(const QUrl &url, int period);
    quint64 dataRate();
    quint64 nbPacketReceived();
    void stop();
    bool updateSetup(const QUrl &url);

protected slots:
    bool setup();
    bool play();
    bool options();
    bool teardown();
    bool describe();

    void publishStats();

    void timerEvent(QTimerEvent *e);
    void error(QAbstractSocket::SocketError);
    
private:
    RTSPSessionPrivate *d;

    bool parseRTSPAnswer(QByteArray answer);
};

#endif // RTSPSESSION_H
