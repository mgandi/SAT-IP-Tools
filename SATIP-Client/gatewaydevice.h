#ifndef GATEWAYDEVICE_H
#define GATEWAYDEVICE_H

#include "program.h"

#include <QObject>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QAbstractSocket>

enum Fec
{
    F_12 = 0,
    F_23 = 1,
    F_34 = 2,
    F_56 = 3,
    F_78 = 4,
    F_910 = 5,
    F_AUTO = 6
};

struct RequestParams
{
    Msys msys;
    bool rtp;
    bool unicast;
    quint8 src; /* satPos */
    quint8 fe;
    quint32 freq;
    bool vpol;
    Mtype mtype;
    quint32 sr;
    Fec fec;
    quint8 bw;
    QString pids;
};

class SSDPClient;
class GatewayDevicePrivate;
class QPlainTextEdit;
class SessionModel;
class RTSPSession;
class GatewayDevice : public QObject
{
    Q_OBJECT
public:
    explicit GatewayDevice(const QUrl &location, const QString &searchTarget, SSDPClient *parent = 0);
    ~GatewayDevice();

    QString host() const;
    QString description() const;
    QString localAddress() const;

    QString friendlyName() const;
    QString manufacturer() const;
    QString modelDescription() const;
    QString deviceType() const;
    QString modelNumber() const;
    QString modelName() const;
    QString serialNumber() const;
    Systems capabilities() const;
    quint32 nbFEForSystem(Msys msys) const;
    QString uid() const;

    QList<Program *> programs() const;

    SessionModel *sessionModel();

public slots:
    RTSPSession *addSession(Program *program, bool unicast = true, const Program::Purposes &purposes = Program::Watch);
    RTSPSession *addSession(const RequestParams &requestParams);
    RTSPSession *addSession(RTSPSession *session);
    bool updateSession(RTSPSession *session, Program *program, int zapPeriod);
    bool removeSession(RTSPSession *session, bool force = false);
    void savePrograms();
    void clearPrograms();
    void addProgram(Program *program);

protected:
    QUrl requestToUrl(const RequestParams &requestParams);
    void loadPrograms();

protected slots:
    void descriptionReplyFinished();

signals:
    void ready(GatewayDevice *);
    void statisticsAvailable(quint64 dataRate, quint64 nbPacketReceived);
    void sessionUpdated();

private:
    GatewayDevicePrivate *d;
    
};

#endif // GATEWAYDEVICE_H
