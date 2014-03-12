#include "rtspsession.h"
#include "rtpsocket.h"
#include "rtcpsocket.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QUrl>
#include <QTimerEvent>
#include <QStringList>
#include <QUrlQuery>
#include <QBuffer>

//#define DEBUG

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
};

class RTSPSessionPrivate
{
public:
    bool ready, unicast;
    quint32 rtpClientPort, rtcpClientPort, rtpServerPort, rtcpServerPort, ttl;
    QString session, lastErrorString;
    quint32 timeout, streamid, cseq, lastCode;
    QUrl url;
    QUrlQuery urlQuery, nextUrlQuery;
    QTcpSocket *rtspSocket;
    RtpSocket *rtpSocket;
    RtcpSocket *rtcpSocket;
    int sessionTimer, statTimer, zapTimer;
    quint64 nbBytesReceived, lastNbBytesReceived;
    QHostAddress source, destination, localAddress;
};

RTSPSession::RTSPSession(const QString &localAddress, const QUrl &url, bool unicast, bool startPlaying, QObject *parent) :
    QObject(parent),
    d(new RTSPSessionPrivate)
{
    // Initialize data
    d->unicast = unicast;
    d->cseq = 0;
    d->rtpClientPort = 0;
    d->rtcpClientPort = 0;
    d->nbBytesReceived = 0;
    d->lastNbBytesReceived = 0;
    d->ready = false;
    d->localAddress = QHostAddress(localAddress);
    d->nextUrlQuery.clear();

    // default SATIP server is 60 second, a server may not indicate its timeout value.
    d->timeout = 30;

    // Set RTSP session url
    setUrl(url);

    // Create the RTP socket
    d->rtpSocket = new RtpSocket(d->localAddress, d->rtpClientPort, d->unicast, this);
    connect(d->rtpSocket, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
            this, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)));

    // Since port is set to 0 the UDP socket will be created with a random port so we need to get it back
    d->rtpClientPort = d->rtpSocket->port();

    // Create the RTCP socket
    d->rtcpSocket = new RtcpSocket(d->localAddress, d->rtcpClientPort, d->unicast, this);
    connect(d->rtcpSocket, SIGNAL(reportAvailable(RTCPReport)),
            this, SIGNAL(rtcpReportAvailable(RTCPReport)));

    // Since port is set to 0 the UDP socket will be created with a random port so we need to get it back
    d->rtcpClientPort = d->rtcpSocket->port();

    // Setup and start playing
    if (setup() && startPlaying)
        play();

//    start();
}

RTSPSession::~RTSPSession()
{
    delete d;
}


QString RTSPSession::session() const
{
    return d->session;
}

quint32 RTSPSession::streamid() const
{
    return d->streamid;
}

bool RTSPSession::unicast() const
{
    return d->unicast;
}

QString RTSPSession::source() const
{
    return d->source.toString();
}

QString RTSPSession::destination() const
{
    return d->destination.toString();
}

int RTSPSession::rtpPort() const
{
    return d->rtpClientPort;
}

int RTSPSession::rtcpPort() const
{
    return d->rtcpClientPort;
}

RtpSocket *RTSPSession::rtpSocket() const
{
    return d->rtpSocket;
}

RtcpSocket *RTSPSession::rtcpSocket() const
{
    return d->rtcpSocket;
}


bool RTSPSession::ready() const
{
    return d->ready;
}

void RTSPSession::setUrl(const QUrl &url)
{
    d->url = url;
    d->urlQuery.setQuery(url.query());
    if (d->ready) play();
}

void RTSPSession::zapUrl(const QUrl &url, int period)
{
    if (!d->nextUrlQuery.isEmpty())
        killTimer(d->zapTimer);

    d->nextUrlQuery = d->urlQuery;
    d->url = url;
    d->urlQuery.setQuery(url.query());

    if (d->ready) play();

    d->zapTimer = startTimer(period * 1000);
}

quint64 RTSPSession::dataRate()
{
    return 0;
}

quint64 RTSPSession::nbPacketReceived()
{
    return 0;
}

void RTSPSession::stop()
{
    teardown();
//    quit();
}

bool RTSPSession::updateSetup(const QUrl &url)
{
    /* Check that session is active */
    if (!d->ready || !d->rtspSocket)
        return false;

    if (!d->nextUrlQuery.isEmpty()) {
        killTimer(d->zapTimer);
        d->nextUrlQuery.clear();
    }

    /* Create RTSP request */
    QString tcpRequest;
    tcpRequest += "SETUP ";
    tcpRequest += url.toString();
    tcpRequest += " RTSP/1.0\r\n";
    tcpRequest += QString("CSeq: %1\r\n").arg(++d->cseq);
    tcpRequest += QString("Session: %1\r\n").arg(d->session);
    tcpRequest += QString("Transport: RTP/AVP;%1=%2-%3\r\n").arg(d->unicast ? "unicast;client_port" : "multicast;port")
            .arg(d->rtpClientPort).arg(d->rtcpClientPort);
    //tcpRequest += "Connection: close\r\n";
    tcpRequest += "\r\n";

#ifdef DEBUG
    qDebug() << "=========================================";
    qDebug() << "TCP request: \n" << qPrintable(tcpRequest);
#endif

    /* Send RTSP request and wait for data in return */
    d->rtspSocket->write(qPrintable(tcpRequest));
    if (!d->rtspSocket->waitForReadyRead())
        return false;

    /* Read data from the socket */
    QByteArray data = d->rtspSocket->readAll();

    /* Check answer from server */
    if (!parseRTSPAnswer(data))
        return false;

    return true;
}


bool RTSPSession::setup()
{
    /* kill Zap timer if running */
    if (!d->nextUrlQuery.isEmpty()) {
        killTimer(d->zapTimer);
        d->nextUrlQuery.clear();
    }

    /* Create TCP socket for communication with host and connect it */
    d->rtspSocket = new QTcpSocket(this);
    d->rtspSocket->connectToHost(QHostAddress(d->url.host()), 554);

    /* Make sure URL is properly formatted for the current request */
    d->url.setQuery(d->urlQuery);

    /* Create RTSP request */
    QString tcpRequest;
    tcpRequest += "SETUP ";
    tcpRequest += d->url.toString();
    tcpRequest += " RTSP/1.0\r\n";
    tcpRequest += QString("CSeq: %1\r\n").arg(++d->cseq);
    tcpRequest += QString("Transport: RTP/AVP;%1=%2-%3\r\n").arg(d->unicast ? "unicast;client_port" : "multicast;port")
            .arg(d->rtpClientPort).arg(d->rtcpClientPort);
    //tcpRequest += "Connection: close\r\n";
    tcpRequest += "\r\n";

#ifdef DEBUG
    qDebug() << "=========================================";
    qDebug() << "TCP request: \n" << qPrintable(tcpRequest);
#endif

    /* Send RTSP request and wait for data in return */
    d->rtspSocket->write(qPrintable(tcpRequest));
    if (!d->rtspSocket->waitForReadyRead())
        return false;

    /* Read data from the socket */
    QByteArray data = d->rtspSocket->readAll();

    /* Check answer from server */
    if (!parseRTSPAnswer(data))
        return false;

//    // Create the RTP socket
//    d->rtpSocket = new RtpSocket(d->destination, d->rtpClientPort, d->unicast, this);
//    connect(d->rtpSocket, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
//            this, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)));

//    // Create the RTCP socket
//    d->rtcpSocket = new RtcpSocket(d->destination, d->rtcpClientPort, d->unicast, this);
//    connect(d->rtcpSocket, SIGNAL(reportAvailable(RTCPReport)),
//            this, SIGNAL(rtcpReportAvailable(RTCPReport)));

    /* Start timer to maintain session alive */
    d->sessionTimer = startTimer(d->timeout * 1000);

    /* Start timer to publich statistics */
    d->statTimer = startTimer(1000);

    /* Now the session is ready */
    d->ready = true;

    return true;
}

bool RTSPSession::play()
{
    /* Make sure URL is properly formatted for the current request */
    d->url.setQuery(d->urlQuery);

    /* Create RTSP request */
    QString tcpRequest;
    tcpRequest += "PLAY ";
    tcpRequest += d->url.toString();
    tcpRequest += " RTSP/1.0\r\n";
    tcpRequest += QString("CSeq: %1\r\n").arg(++d->cseq);
    tcpRequest += QString("Session: %1\r\n").arg(d->session);
    tcpRequest+= "\r\n";

#ifdef DEBUG
    qDebug() << "=========================================";
    qDebug() << "TCP request: \n" << qPrintable(tcpRequest);
#endif

    /* Send RTSP request and wait for data in return */
    d->rtspSocket->write(qPrintable(tcpRequest));
    if (!d->rtspSocket->waitForReadyRead())
        return false;

    /* Read data from the socket */
    QByteArray data = d->rtspSocket->readAll();

    /* Check answer from server */
    if (!parseRTSPAnswer(data))
        return false;

    return true;
}


bool RTSPSession::options()
{
    /* Make sure URL is properly formatted for the current request */
    d->url.setQuery(QUrlQuery());

    /* Create RTSP request */
    QString tcpRequest;
    tcpRequest += "OPTIONS ";
    tcpRequest += d->url.toString();
    tcpRequest += " RTSP/1.0\r\n";
    tcpRequest += QString("CSeq: %1\r\n").arg(++d->cseq);
    tcpRequest += QString("Session: %1\r\n").arg(d->session);
    tcpRequest+= "\r\n";

#ifdef DEBUG
    qDebug() << "=========================================";
    qDebug() << "TCP request: \n" << qPrintable(tcpRequest);
#endif

    /* Send RTSP request and wait for data in return */
    d->rtspSocket->write(qPrintable(tcpRequest));
    if (!d->rtspSocket->waitForReadyRead())
        return false;

    /* Read data from the socket */
    QByteArray data = d->rtspSocket->readAll();

    /* Check answer from server */
    if (!parseRTSPAnswer(data))
        return false;

    return true;
}

bool RTSPSession::teardown()
{
    /* Make sure URL is properly formatted for the current request */
    d->url.setQuery(QUrlQuery());

    /* Kill session maintainer timer */
    killTimer(d->sessionTimer);

    if (!d->nextUrlQuery.isEmpty()) {
        d->nextUrlQuery.clear();
        killTimer(d->zapTimer);
    }

    /* Create TCP request */
    QString tcpRequest;
    tcpRequest += "TEARDOWN ";
    tcpRequest += d->url.toString();
    tcpRequest += " RTSP/1.0\r\n";
    tcpRequest += QString("CSeq: %1\r\n").arg(++d->cseq);
    tcpRequest += QString("Session: %1\r\n").arg(d->session);
    tcpRequest += "Connection: close\r\n";
    tcpRequest+= "\r\n";

#ifdef DEBUG
    qDebug() << "=========================================";
    qDebug() << "TCP request: \n" << qPrintable(tcpRequest);
#endif

    /* Send RTSP request and wait for data in return */
    d->rtspSocket->write(qPrintable(tcpRequest));
    if (!d->rtspSocket->waitForReadyRead())
        return false;

    /* Read data from the socket */
    QByteArray data = d->rtspSocket->readAll();

    /* Check answer from server */
    if (!parseRTSPAnswer(data))
        return false;

    /* Delete the RTSP socket since we won't use it anymore */
    delete d->rtspSocket;
    d->rtspSocket = NULL;

    return true;
}

bool RTSPSession::describe()
{
    return true;
}


void RTSPSession::publishStats()
{
    quint64 diff = d->nbBytesReceived - d->lastNbBytesReceived;
    d->lastNbBytesReceived = d->nbBytesReceived;
    emit statisticsAvailable(diff, d->nbBytesReceived / 188);
}


void RTSPSession::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == d->sessionTimer) {
        options();
        e->accept();
    } else if (e->timerId() == d->statTimer) {
        publishStats();
        e->accept();
    } else if (e->timerId() == d->zapTimer) {
        /* swap Url */
        QUrlQuery urlQuery = d->nextUrlQuery;
        d->nextUrlQuery = d->urlQuery;
        d->urlQuery = urlQuery;

        qDebug() << "switch to " << urlQuery.toString();
        play();
        emit sessionUpdated();
    }
}

void RTSPSession::error(QAbstractSocket::SocketError)
{
    QUdpSocket *socket = qobject_cast<QUdpSocket *>(sender());
    qDebug() << qPrintable(socket->errorString());
}


bool RTSPSession::parseRTSPAnswer(QByteArray answer)
{
    bool cseqValid = false, answerValid = false;

#ifdef DEBUG
    qDebug() << "=========================================";
    qDebug() << answer.data();
#endif

    /* Split answer in tokens */
    QString str = QString(answer);
    QStringList tokens = str.split("\r\n");

    /* Make sure it is an RTSP answer */
    if (!tokens.at(0).startsWith("RTSP/1.0"))
        return false;

    /* Parse each token */
    foreach (QString line, tokens) {

        if (line.startsWith("RTSP/1.0")) { // Check if there was an error in the message sent
            QStringList elements = tokens.at(0).split(" ");
            d->lastCode = elements.at(1).toUInt();
            d->lastErrorString = elements.at(2);
            if (d->lastCode != 200) {
                // Debug
                qWarning() << "SETUP failed: " << qPrintable(d->lastErrorString);
                return false;
            }
            answerValid = true;
        }

        else if (line.startsWith("CSeq:")) { // Check the validity of CSeq
            QString cseqString = line;
            cseqString.remove("CSeq:");
            cseqString.remove(" ");
            quint32 cseq = cseqString.toUInt();
            if (cseq != d->cseq)
                return false;
            cseqValid = true;
        }

        else if (line.startsWith("Transport:")) { // Extract transport data
            QStringList elements = line.split(";");

            foreach (QString element, elements) {
                if (element == "unicast") {
                    if (!d->unicast) {
                        qWarning() << "Server set unicast transport when transport is required multicast";
                        return false;
                    }
                }

                else if (element == "multicast") {
                    if (d->unicast) {
                        qWarning() << "Server set multicast transport when transport is required unicast";
                        return false;
                    }
                }

                else if (element.startsWith("source=")) {
                    QString sourceString = element;
                    sourceString.remove("source=");
                    sourceString.remove(" ");
                    d->source = QHostAddress(sourceString);
                }

                else if (element.startsWith("destination=")) {
                    QString destinationString = element;
                    destinationString.remove("destination=");
                    destinationString.remove(" ");
                    d->destination = QHostAddress(destinationString);
                }

                else if (element.startsWith("client_port=")) {
                    QString portString = element;
                    portString.remove("client_port=");
                    portString.remove(" ");
                    QStringList ports = portString.split("-");
                    d->rtpClientPort = ports.at(0).toUInt();
                    d->rtcpClientPort = ports.at(1).toUInt();
                }

                else if (element.startsWith("server_port=")) {
                    QString portString = element;
                    portString.remove("server_port=");
                    portString.remove(" ");
                    QStringList ports = portString.split("-");
                    d->rtpServerPort = ports.at(0).toUInt();
                    d->rtcpServerPort = ports.at(1).toUInt();
                }

                else if (element.startsWith("port=")) {
                    QString portString = element;
                    portString.remove("port=");
                    portString.remove(" ");
                    QStringList ports = portString.split("-");
                    d->rtpClientPort = ports.at(0).toUInt();
                    d->rtcpClientPort = ports.at(1).toUInt();
                }

                else if (element.startsWith("ttl=")) {
                    QString ttlString = element;
                    ttlString.remove("ttl=");
                    ttlString.remove(" ");
                    d->ttl = ttlString.toUInt();
                }
            }
        }

        else if (line.startsWith("com.ses.streamID:")) { // Extract Streamd ID
            d->streamid = line.remove("com.ses.streamID:", Qt::CaseInsensitive).remove(" ").toUInt();
            d->url.setPath(QString("/stream=%1").arg(d->streamid));
        }

        else if (line.startsWith("Session:")) {
            QStringList sessionElements = line.split(";");
            QString session = sessionElements.at(0);
            session.remove("Session:", Qt::CaseInsensitive);
            session.remove(" ");
            if (d->session.isEmpty())
                d->session = session;
            else if (d->session != session) {
                return false;
            }
            if (sessionElements.count() == 2) {
                QString timeoutString = sessionElements.at(1);
                timeoutString.remove("timeout=", Qt::CaseInsensitive);
                timeoutString.remove(" ");
                d->timeout = timeoutString.toUInt();
                d->timeout = d->timeout/2;
                qDebug() << "timeout" << d->timeout;
            }
        }
    }

    return (cseqValid && answerValid);
}
