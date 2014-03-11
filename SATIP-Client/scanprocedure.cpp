#include "scanprocedure.h"
#include "rtpsocket.h"
#include "rtspsession.h"
#include "gatewaydevice.h"
#include "elementarystream.h"
#include "program.h"
#include "programmodel.h"

//#define USE_LIBDVBPSI

#ifndef USE_LIBDVBPSI
#include "demuxer.h"
#include "patpidhandler.h"
#include "pmtpidhandler.h"
#include "sdtpidhandler.h"
#else // USE_LIBDVBPSI
#include "pathandler.h"
#include "pmthandler.h"
#include "demuxhandler.h"
#include "sdthandler.h"
#include "nithandler.h"
#endif // USE_LIBDVBPSI

#include <QUrl>
#include <QUrlQuery>
#include <QHash>
#include <QFlags>
#include <QCoreApplication>
#include <QTimerEvent>
#include <QElapsedTimer>

class StepEvent : public QEvent
{
public:
    StepEvent() :
        QEvent(QEvent::User) {}
};

class ScanProcedurePrivate
{
public:
    enum TableType {
        PAT = 0x01,
        PMT = 0x02,
        SDT = 0x04
    };
    Q_DECLARE_FLAGS(TableTypes, TableType)

    ScanProcedurePrivate(GatewayDevice *d, ScanProcedure *parent) :
        p(parent),
        device(d),

        lockTimeout(3000),
        patTimeout(5000),
        stepTimeout(10000),
        isAuto(true),

        rtpSession(0),
        status(ScanProcedure::Idle),

        lockTimeoutTimerId(0),
        patTimeoutTimerId(0),
        locked(false),
        lastSsrc(0),
        currentSsrc(0),
        tables(0),

        demuxer(p)
    {}



    // Contant elements
    ScanProcedure *p;
    GatewayDevice *device;
    Program param;

    // Scan setup elements
    int lockTimeout;
    int patTimeout;
    int stepTimeout;
    bool isAuto;

    // Scan progress elements
    RTSPSession *rtpSession;
    ScanProcedure::ScanStatus status;

    // Step management
    int lockTimeoutTimerId;
    int patTimeoutTimerId;
    QList<AbstractDvbPsiHandler *> handlers;
    bool locked;
    quint32 lastSsrc;
    quint32 currentSsrc;
    QMap<quint16, Program *> tmpProgramsMap;
    QList<Program *> tmpPrograms;
    TableTypes tables;

    // Demuxing elements
    Demuxer demuxer;
    QList<AbstractPidHandler *> pidHandlers;

    QElapsedTimer patTimer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ScanProcedurePrivate::TableTypes)

ScanProcedure::ScanProcedure(GatewayDevice *device, QObject *parent, int stepTimeout, int patTimeout) :
    QObject(parent),
    d(new ScanProcedurePrivate(device, this))
{
    d->stepTimeout = stepTimeout;
    d->patTimeout = patTimeout;
}

ScanProcedure::~ScanProcedure()
{
    delete d;
}

void ScanProcedure::setLockTimeout(int lockTimeout)
{
    d->lockTimeout = lockTimeout;
}

int ScanProcedure::lockTimeout() const
{
    return d->lockTimeout;
}

void ScanProcedure::setStepTimeout(int stepTimeout)
{
    d->stepTimeout = stepTimeout;
}

int ScanProcedure::patTimeout() const
{
    return d->patTimeout;
}

void ScanProcedure::setPatTimeout(int patTimeout)
{
    d->patTimeout = patTimeout;
}

int ScanProcedure::stepTimeout() const
{
    return d->stepTimeout;
}

void ScanProcedure::setAuto(bool isAuto)
{
    d->isAuto = isAuto;
}

bool ScanProcedure::isAuto() const
{
    return d->isAuto;
}

void ScanProcedure::start()
{
    // Clear all frequency specific variables
    clearAll();

    // Initialize all progress management variables
    d->param.setFrequency(0);
    d->status = Started;

    // Notify that scan was started
    emit scanStarted();

    // Go to first step
    step();
}

void ScanProcedure::pause()
{
    // Kill the timeout timer
    killRunningTimer();

    // Stop the RTSP session if running
    deleteSession();

    // Current scan is Paused
    d->status = Paused;

    // Delet all potential temporary programs that where found
    qDeleteAll(d->tmpPrograms);
    d->tmpPrograms.clear();
    d->tmpProgramsMap.clear();

    // Notify that scan was paused
    emit scanPaused();
}

void ScanProcedure::stop()
{
    // Kill the timeout timer
    killRunningTimer();

    // Stop the RTSP session is running
    deleteSession();

    // Current scan is Idle
    d->status = Idle;

    // Delet all potential temporary programs that where found
    qDeleteAll(d->tmpPrograms);
    d->tmpPrograms.clear();
    d->tmpProgramsMap.clear();

    // Notify that scan was stopped
    emit scanStopped();
}

void ScanProcedure::stepForward()
{
    step();
}

void ScanProcedure::stepBackward()
{
    step(false);
}

void ScanProcedure::resume()
{
    step();
}


ScanProcedure::ScanStatus ScanProcedure::scanStatus() const
{
    return d->status;
}

Program *ScanProcedure::getParam() const
{
    return &d->param;
}

void ScanProcedure::step(bool stepForward)
{
    int progress;

    beginStep();

    // Update current frequency if not currently paused
    if (scanStatus() != Paused) {
        progress = getStep(stepForward);
    } else {
        d->status = Started;

        // Notify that the scan was re-started
        emit scanStarted();
    }

    if (progress == 100) {
        // Stop the RTSP session if running
        deleteSession();

        // Current scan is Completed
        d->status = Completed;

        //  Notify that sacn is completed
        emit scanDone();
        return;
    }

    qDebug() << "=========================================";
    qDebug() << "Frequency: " << d->param.frequency();

    // Communicate the progress of the scan
    emit scanProgress(progress, d->param.frequency(), 0 /* d->endFrequency */);

    /* request more than just PID 0 to speed up the acquisition */
    d->param.clearScanPids();
    //d->param.appendScanPids(QList<quint16>() << 1 << 16 << 17 << 18 << 19 << 20);
    QUrl qUrl = d->param.toUrl(Program::RTSP, d->device->host(), Program::Scan, NULL);

    // Create new RTSP session if does not exist & connect its signals
    if (!d->rtpSession) {
        d->rtpSession = new RTSPSession(d->device->localAddress(), qUrl, true, this);
        connect(d->rtpSession, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
                this, SLOT(packetsAvailable(quint32,QList<QByteArray>&)));
        connect(d->rtpSession, SIGNAL(rtcpReportAvailable(RTCPReport)),
                this, SLOT(rtcpReportAvailable(RTCPReport)));
        d->device->addSession(d->rtpSession);
    } else { // Or simply update the session
        // Setup RTSP session to receive the right frequency
        d->rtpSession->setUrl(qUrl);
    }

    // Launch lock timeout timer
    d->lockTimeoutTimerId = startTimer(d->lockTimeout);

    // We are done with step
    endStep();
}

#ifndef USE_LIBDVBPSI
void ScanProcedure::patFound()
{
    PatPidHandler *patHandler = qobject_cast<PatPidHandler *>(sender());

    // Kill running timer
    killRunningTimer();

    // Take notes that we found a PAT
    d->tables |= ScanProcedurePrivate::PAT;

    // A PAT was found so we can create an sdt handler
    SdtPidHandler *sdtPidHandler = new SdtPidHandler(SdtPidHandler::CurrentTs, true, this);
    connect(sdtPidHandler, SIGNAL(dataChanged()),
            this, SLOT(sdtFound()));
    d->demuxer.addPidHandler(0x11, sdtPidHandler);
    d->pidHandlers += sdtPidHandler;
    d->param.appendScanPid(0x11);

    // Go through all PMT
    QDateTime now = QDateTime::currentDateTime();
    qDebug() << "=========================================";
    qDebug() << "Found a PAT for TS ID" << patHandler->tsId() << "@" << d->param.frequency() <<
                "in" << d->patTimer.elapsed() << "ms";

    foreach (PatProgram program, patHandler->programs()) {
        qDebug() << "| " << qPrintable(QString("%1").arg(program.number, 5))
                 << qPrintable(QString(" PID 0x%1 (%2)").arg(program.pid, 4, 16, QLatin1Char('0')).arg(program.pid));
        if (program.number) { // Ignore program number 0 which when present is the NIT
            // Create a program for each PMT mentioned in the PAT
            Program *prgm = new Program(d->param);
            prgm->setNumber(program.number);
            prgm->setPid(program.pid);
            prgm->setTransportStreamID(patHandler->tsId());

            d->tmpProgramsMap.insert(program.number, prgm);
            d->tmpPrograms += prgm;

            // Create a handler for each PMT
            PmtPidHandler *handler = new PmtPidHandler(program.number, true, this);
            connect(handler, SIGNAL(dataChanged()),
                    this, SLOT(pmtFound()));
            d->demuxer.addPidHandler(program.pid, handler);
            d->pidHandlers += handler;

            d->param.appendScanPid(program.pid);
        }
    }
    /* Update RTSP request with these new PIDs */
    d->rtpSession->setUrl(d->param.toUrl(Program::RTSP, d->device->host(),
                                         Program::Scan | Program::UpdateOnlyPid,
                                         d->rtpSession));

    checkDone();
}
#else // USE_LIBDVBPSI
void ScanProcedure::patFound()
{
    PatHandler *patHandler = qobject_cast<PatHandler *>(sender());

    // Kill running timer
    killRunningTimer();

    // Take notes that we found a PAT
    d->tables |= ScanProcedurePrivate::PAT;

    // A PAT was found so we can create a demux handler and attach it
    DemuxHandler *demuxHandler = new DemuxHandler(this);
    connect(demuxHandler, SIGNAL(newHandler(AbstractDvbPsiHandler*)),
            this, SLOT(newHandler(AbstractDvbPsiHandler*)));
    demuxHandler->attach();
    d->handlers += demuxHandler;

    // Go through all PMT
    qDebug() << "=========================================";
    qDebug() << "Found a PAT for TS ID in " << patHandler->tsId() << "@" << d->param.frequency());
    foreach (PatProgram program, patHandler->programs()) {
        qDebug() << "| " << qPrintable(QString("%1").arg(program.number, 5)) << qPrintable(QString(" PID 0x%1 (%2)").arg(program.pid, 4, 16, QLatin1Char('0')).arg(program.pid));
        if (program.number) { // Ignore program number 0 which when present is the NIT
            // Create a program for each PMT mentioned in the PAT
            Program *prgm = new Program(d->param.frequency(), (quint8)(d->bandwidth/1000), patHandler->tsId(), program.number, program.pid);
            d->tmpProgramsMap.insert(program.number, prgm);
            d->tmpPrograms += prgm;

            // Create a handler for each PMT
            PmtHandler *handler = new PmtHandler(program.number, this);
            connect(handler, SIGNAL(dataChanged()),
                    this, SLOT(pmtFound()));
            handler->attach();
            d->handlers += handler;
        }
    }

    checkDone();
}
#endif // USE_LIBDVBPSI

#ifndef USE_LIBDVBPSI
void ScanProcedure::pmtFound()
{
    PmtPidHandler *pmtHandler = qobject_cast<PmtPidHandler *>(sender());

    Program *program = d->tmpProgramsMap.value(pmtHandler->programNumber());
    if (program) {
        program->setPcrPid(pmtHandler->pcrPid());
        foreach (PmtElementaryStream *es, pmtHandler->elementaryStreams()) {
            program->appendElementaryStream(es->pid, es->type);
        }
    }

    checkDone();
}
#else // USE_LIBDVBPSI
void ScanProcedure::pmtFound()
{
    PmtHandler *pmtHandler = qobject_cast<PmtHandler *>(sender());

    Program *program = d->tmpProgramsMap.value(pmtHandler->programNumber());
    if (program) {
        program->setPcrPid(pmtHandler->pcrPid());
        foreach (PmtElementaryStream *es, pmtHandler->elementaryStreams()) {
            program->appendElementaryStream(es->pid, es->type);
        }
    }

    checkDone();
}
#endif // USE_LIBDVBPSI

#ifndef USE_LIBDVBPSI
void ScanProcedure::sdtFound()
{
    SdtPidHandler *sdtHandler = qobject_cast<SdtPidHandler *>(sender());

    d->tables |= ScanProcedurePrivate::SDT;

    qDebug() << "=========================================";
    foreach (SdtService *service, sdtHandler->services()->values()) {
        Program *program = d->tmpProgramsMap.value(service->serviceId);
        if (program) {
            program->setServiceProviderName(service->serviceProviderName);
            program->setServiceName(service->serviceName);
            program->setScrambled(service->scrambled);
            qDebug() << service->serviceName;
        }
    }

    checkDone();
}
#else // USE_LIBDVBPSI
void ScanProcedure::sdtFound()
{
    SdtHandler *sdtHandler = qobject_cast<SdtHandler *>(sender());

    d->tables |= ScanProcedurePrivate::SDT;

    qDebug() << "=========================================";
    foreach (SdtService *service, sdtHandler->services()->values()) {
        Program *program = d->tmpProgramsMap.value(service->serviceId);
        if (program) {
            program->setServiceProviderName(service->serviceProviderName);
            program->setServiceName(service->serviceName);
            qDebug() << service->serviceName;
        }
    }

    checkDone();
}
#endif // USE_LIBDVBPSI

#ifndef USE_LIBDVBPSI
#else // USE_LIBDVBPSI
void ScanProcedure::nitFound()
{
    NitHandler *nitHandler = qobject_cast<NitHandler *>(sender());
    Q_UNUSED(nitHandler);
}

void ScanProcedure::newHandler(AbstractDvbPsiHandler *handler)
{
    SdtHandler *sdtHandler;
    NitHandler *nitHandler;

    if ((sdtHandler = qobject_cast<SdtHandler *>(handler))) {
        connect(sdtHandler, SIGNAL(dataChanged()),
                this, SLOT(sdtFound()));
        d->handlers += sdtHandler;
    }

    else if ((nitHandler = qobject_cast<NitHandler *>(handler))) {
        qDebug() << "=========================================";
        qDebug() << "Have a new NIT handler!!!";
        connect(nitHandler, SIGNAL(dataChanged()),
                this, SLOT(nitFound()));
        d->handlers += nitHandler;
    }
}
#endif // USE_LIBDVBPSI

void ScanProcedure::checkDone()
{
    bool allPmtTriggered = true, hasPmtHandler = false;

#ifndef USE_LIBDVBPSI
    foreach (AbstractPidHandler *handler, d->pidHandlers) {
        PmtPidHandler *pmtHandler = qobject_cast<PmtPidHandler *>(handler);
        if (pmtHandler) {
            allPmtTriggered &= pmtHandler->triggered();
            hasPmtHandler = true;
        }
    }
#else // USE_LIBDVBPSI
    foreach (AbstractDvbPsiHandler *handler, d->handlers) {
        PmtHandler *pmtHandler = qobject_cast<PmtHandler *>(handler);
        if (pmtHandler) {
            allPmtTriggered &= pmtHandler->triggered();
            hasPmtHandler = true;
        }
    }
#endif // USE_LIBDVBPSI

    qDebug() << "=========================================";
    if (allPmtTriggered && hasPmtHandler) {
        d->tables |= ScanProcedurePrivate::PMT;
    } else {
        if (!allPmtTriggered) {
            qDebug() << "Not all PMT have been seen yet!";
        }
        if (!hasPmtHandler) {
            qDebug() << "No PMT handlers created!";
        }
    }

    if (d->tables == (ScanProcedurePrivate::PAT |
                      ScanProcedurePrivate::SDT |
                      ScanProcedurePrivate::PMT)) {
        qDebug() << "Got all information";
        if (d->isAuto)
            QCoreApplication::postEvent(this, new StepEvent());
    } else {
        if (!(d->tables & ScanProcedurePrivate::PAT)) {
            qDebug() << "No PAT found!";
        }
        if (!(d->tables & ScanProcedurePrivate::SDT)) {
            qDebug() << "No SDT found!";
        }
    }
}


void ScanProcedure::packetsAvailable(quint32 ssrc, QList<QByteArray> &packets)
{
    if (d->currentSsrc == 0) {
        if (d->lastSsrc == ssrc) {
//            qDebug() << "Packet from last SSRC (" << ssrc << ") -> ignored!";
            return;
        } else
            d->currentSsrc = ssrc;
    }

#ifndef USE_LIBDVBPSI
    d->demuxer.push(packets);
#else // USE_LIBDVBPSI
    foreach (AbstractDvbPsiHandler *handler, d->handlers) {
        if (handler->attached()) handler->push(packets);
    }
#endif // USE_LIBDVBPSI
}

void ScanProcedure::rtcpReportAvailable(const RTCPReport &report)
{
    static RTCPReport lastReport;

    // Make sure that we don't treat packet from previous frequency
    if (d->currentSsrc == 0) {
        if (d->lastSsrc == report.ssrc()) {
//            qDebug() << "RTCP Report from last SSRC (" << report.ssrc() << ") -> ignored!";
            return;
        } else
            d->currentSsrc = report.ssrc();
    }

    if (report == lastReport) {
    } else {
        qDebug() << "=========================================";
        qDebug() << "RTCP Report";
        qDebug() << "SSRC: " << report.ssrc();
        qDebug() << "Front end ID: " << report.feID();
        qDebug() << "Lock status: " << (report.locked() ? "locked" : "unlocked");
        qDebug() << "Signal level: " << report.signalLevel();
        qDebug() << "Signal quality: " << report.signalQuality();
        qDebug() << "Frequency: " << report.frequency();
        lastReport = report;
    }

    if (report.locked() && !d->locked)
        locked();
    else if (d->locked && !report.locked())
        unlocked();
}


void ScanProcedure::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == d->lockTimeoutTimerId) {
        qDebug() << "=========================================";
        qDebug() << "Lock timeout!";
    } else if (e->timerId() == d->patTimeoutTimerId) {
        qDebug() << "=========================================";
        qDebug() << "PAT timeout!";
    }

    // Kill timer so it does not repeat if not re-armed
    killRunningTimer();

    // If in auto scan mode, step to next frequency
    if (d->isAuto)
        step();
}

bool ScanProcedure::event(QEvent *e)
{
    if (dynamic_cast<StepEvent *>(e)) {
        qDebug() << "=========================================";
        qDebug() << "Step event!";

        // If in auto scan mode, step to next frequency
        if (d->isAuto)
            step();

        return true;
    }

    return QObject::event(e);
}

#ifndef USE_LIBDVBPSI
void ScanProcedure::beginStep()
{
    // Kill all running timer
    killRunningTimer();

    // Dettach all handlers
    dettachHandlers();

    // Clear all handlers
    qDeleteAll(d->pidHandlers);
    d->demuxer.clear();
    d->pidHandlers.clear();

    // Commit temporary data if any
    commitData();

    // Update SSRC
    d->lastSsrc = d->currentSsrc;
    d->currentSsrc = 0;

    // Clear tables found
    d->tables = 0;
}
#else // USE_LIBDVBPSI
void ScanProcedure::beginStep()
{
    // Kill all running timer
    killRunningTimer();

    // Dettach all handlers
    dettachHandlers();

    // Clear all handlers
    qDeleteAll(d->handlers);
    d->handlers.clear();

    // Commit temporary data if any
    commitData();

    // Update SSRC
    d->lastSsrc = d->currentSsrc;
    d->currentSsrc = 0;

    // Clear tables found
    d->tables = 0;
}
#endif // USE_LIBDVBPSI

void ScanProcedure::endStep()
{
    // Force unlock status
    d->locked = false;
}

void ScanProcedure::clearAll()
{
    beginStep();
    endStep();
}

#ifndef USE_LIBDVBPSI
void ScanProcedure::attachHandlers()
{
    foreach (AbstractPidHandler *handler, d->pidHandlers) {
        if (!handler->isAttached()) handler->attach();
    }
}
#else // USE_LIBDVBPSI
void ScanProcedure::attachHandlers()
{
    foreach (AbstractDvbPsiHandler *handler, d->handlers) {
        if (!handler->attached()) handler->attach();
    }
}
#endif // USE_LIBDVBPSI

#ifndef USE_LIBDVBPSI
void ScanProcedure::dettachHandlers()
{
    foreach (AbstractPidHandler *handler, d->pidHandlers) {
        if (handler->isAttached()) handler->dettach();
    }
}
#else // USE_LIBDVBPSI
void ScanProcedure::dettachHandlers()
{
    foreach (AbstractDvbPsiHandler *handler, d->handlers) {
        if (handler->attached()) handler->dettach();
    }
}
#endif // USE_LIBDVBPSI

void ScanProcedure::commitData()
{
    if (d->tables == (ScanProcedurePrivate::PAT |
                      ScanProcedurePrivate::SDT |
                      ScanProcedurePrivate::PMT)) { // We have ll the information required for the listed programs
        // Add each program found to gateway device
        foreach (Program *program, d->tmpPrograms) {
            d->device->addProgram(program);
        }

        // Clear the tmp list and map of programs
        d->tmpPrograms.clear();
        d->tmpProgramsMap.clear();
    } else { // Information about program is not complete
        // Delet all potential temporary programs that where found
        qDeleteAll(d->tmpPrograms);
        d->tmpPrograms.clear();
        d->tmpProgramsMap.clear();
    }
}

void ScanProcedure::killRunningTimer()
{
    if (d->lockTimeoutTimerId) {
        killTimer(d->lockTimeoutTimerId);
        d->lockTimeoutTimerId = 0;
    }
    if (d->patTimeoutTimerId) {
        killTimer(d->patTimeoutTimerId);
        d->patTimeoutTimerId = 0;
    }
}

#ifndef USE_LIBDVBPSI
void ScanProcedure::locked()
{
    // Check if already locked
    if (d->locked)
        return;

    // We kill the potential running timer since the tuner is locked
    killRunningTimer();

    // Change scan procedure lock status
    d->locked = true;
    qDebug() << "=========================================";
    qDebug() << "Locked!";

    // Check if a pat handler already exists
    PatPidHandler *patHandler = 0;
    foreach (AbstractPidHandler *handler, d->pidHandlers) {
        if ((patHandler = qobject_cast<PatPidHandler *>(handler)))
            break;
    }

    // If no PAT handler exist create it and attach it
    if (!patHandler) {
        PatPidHandler *handler = new PatPidHandler(this);
        connect(handler, SIGNAL(dataChanged()),
                this, SLOT(patFound()));
        d->demuxer.addPidHandler(0x0, handler);
        d->pidHandlers += handler;

        d->patTimer.start();
    }

    // Attach all handlers
    attachHandlers();

    // Launch PAT timeout timer
    d->patTimeoutTimerId = startTimer(d->patTimeout);
}
#else // USE_LIBDVBPSI
void ScanProcedure::locked()
{
    // We kill the potential running timer since the tuner is locked
    killRunningTimer();

    // Change scan procedure lock status
    d->locked = true;
    qDebug() << "=========================================";
    qDebug() << "Locked!";

    // Check if a pat handler already exists
    PatHandler *patHandler = 0;
    foreach (AbstractDvbPsiHandler *handler, d->handlers) {
        if ((patHandler = qobject_cast<PatHandler *>(handler)))
            break;
    }

    // If no PAT handler exist create it and attach it
    if (!patHandler) {
        PatHandler *handler = new PatHandler(this);
        connect(handler, SIGNAL(dataChanged()),
                this, SLOT(patFound()));
        handler->attach();
        d->handlers += handler;
    }

    // Attach all handlers
    attachHandlers();

    // Launch PAT timeout timer
    d->patTimeoutTimerId = startTimer(d->patTimeout);
}
#endif // USE_LIBDVBPSI

#ifndef USE_LIBDVBPSI
void ScanProcedure::unlocked()
{
    // Check if we are unlocked
    if (!d->locked)
        return;

    // We kill the potential running timer since the tuner is unlocked
    killRunningTimer();

    // Deattach all handlers
    dettachHandlers();

    // Now we are unlocked
    d->locked = false;
    qDebug() << "=========================================";
    qDebug() << "Unlocked!";

    // We have to re-launch the lock timeout timer
    d->lockTimeoutTimerId = startTimer(d->lockTimeout);
}
#else // USE_LIBDVBPSI
void ScanProcedure::unlocked()
{
    // We kill the potential running timer since the tuner is unlocked
    killRunningTimer();

    // Deattach all handlers
    dettachHandlers();

    // Now we are unlocked
    d->locked = false;
    qDebug() << "=========================================";
    qDebug() << "Unlocked!";

    // We have to re-launch the lock timeout timer
    d->lockTimeoutTimerId = startTimer(d->lockTimeout);
}
#endif // USE_LIBDVBPSI

void ScanProcedure::deleteSession()
{
    if (d->rtpSession) {
        d->device->removeSession(d->rtpSession, true);
        // Stop the RTSP session
        d->rtpSession->stop();
        d->rtpSession->wait();
        delete d->rtpSession;
        d->rtpSession = 0;
    }
}
