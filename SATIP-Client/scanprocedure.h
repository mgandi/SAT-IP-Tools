#ifndef SCANPROCEDURE_H
#define SCANPROCEDURE_H

#include "rtcpreport.h"
#include "program.h"

#include <QObject>

class ProgramModel;
class AbstractDvbPsiHandler;
class GatewayDevice;
class ScanProcedurePrivate;
class ScanProcedure : public QObject
{
    Q_OBJECT
public:
    enum ScanStatus {
        Idle      = 0x0,
        Started   = 0x1,
        Paused    = 0x2,
        Completed = 0x3
    };

    explicit ScanProcedure(GatewayDevice *device, QObject *parent = 0, int stepTimeout = 3000, int patTimeout = 5000);
    ~ScanProcedure();

public slots:

    void setLockTimeout(int lockTimeout);
    int lockTimeout() const;
    void setStepTimeout(int stepTimeout);
    int patTimeout() const;
    void setPatTimeout(int patTimeout);
    int stepTimeout() const;
    void setAuto(bool isAuto);
    bool isAuto() const;

    void start();
    void pause();
    void stop();
    void stepForward();
    void stepBackward();
    void resume();

    ScanStatus scanStatus() const;


protected slots:
    void patFound();
    void pmtFound();
    void sdtFound();
#ifndef USE_LIBDVBPSI
#else // USE_LIBDVBPSI
    void nitFound();
    void newHandler(AbstractDvbPsiHandler *handler);
#endif // USE_LIBDVBPSI
    void checkDone();

    void packetsAvailable(quint32 ssrc, QList<QByteArray> &packets);
    void rtcpReportAvailable(const RTCPReport &report);

    void step(bool stepForward = true);
    virtual int getStep(bool stepForward = true) = 0;
    Program *getParam() const;

    void timerEvent(QTimerEvent *e);
    bool event(QEvent *);
    void beginStep();
    void endStep();
    void clearAll();
    void attachHandlers();
    void dettachHandlers();
    void commitData();
    void killRunningTimer();
    void locked();
    void unlocked();
    void deleteSession();

signals:
    void scanStarted();
    void scanProgress(int percent, int frequencyKHz, int endFrequencyKHz);
    void scanDone();
    void scanStopped();
    void scanPaused();

private:
    ScanProcedurePrivate *d;

};

#endif // SCANPROCEDURE_H
