#include "programrecorder.h"
#include "gatewaydevice.h"
#include "event.h"
#include "program.h"

#include <QDateTime>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QDir>
#include <QTimerEvent>

class ProgramRecorderPrivate
{
public:
    ProgramRecorderPrivate(GatewayDevice *d, Event *e, ProgramRecorder *parent) :
        p(parent),
        device(d),
        event(e),
        manager(p),
        reply(0),
        fileName(QString("%1/%2%3.ts").arg(QDir::homePath()).arg(e->uid()).arg(e->startDateTime().toString("yyyyMMdd"))),
        file(fileName),
        status(ProgramRecorder::Idle)
    {}

    ProgramRecorderPrivate(GatewayDevice *d, Program *prog, const QString &f, ProgramRecorder *parent) :
        p(parent),
        device(d),
        event(0),
        program(prog),
        manager(p),
        reply(0),
        fileName(f),
        file(fileName),
        status(ProgramRecorder::Idle)
    {}

    ProgramRecorder *p;
    GatewayDevice *device;
    Event *event;
    Program *program;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QString fileName;
    QFile file;
    ProgramRecorder::Status status;
    int startTimeId;
    int endTimerId;
};

ProgramRecorder::ProgramRecorder(GatewayDevice *device, Event *event, QObject *parent) :
    QObject(parent),
    d(new ProgramRecorderPrivate(device, event, this))
{
    // Look for associated program
    foreach (Program *prgm, d->device->programs()) {
        if (prgm->uid() == d->event->uid()) {
            d->program = prgm;
        }
    }

    // If no associated program do nothing
    if (!d->program)
        return;

    // Start timer
    if (d->event) {
        d->startTimeId = startTimer(1000);
    }

    // change status to wait for start time
    d->status = WaitingForStartTime;
    emit statusChanged(d->status);
}

ProgramRecorder::ProgramRecorder(GatewayDevice *device, Program *program, const QString &fileName, QObject *parent) :
    QObject(parent),
    d(new ProgramRecorderPrivate(device, program, fileName, this))
{
}


ProgramRecorder::~ProgramRecorder()
{
    delete d;
}

Program *ProgramRecorder::program() const
{
    return d->program;
}

ProgramRecorder::Status ProgramRecorder::status() const
{
    return d->status;
}

QDateTime ProgramRecorder::startTime() const
{
    if (d->event) {
        return d->event->startDateTime();
    } else {
        return QDateTime();
    }
}

QDateTime ProgramRecorder::endTime() const
{
    if (d->event) {
        return d->event->endDateTime();
    } else {
        return QDateTime();
    }
}

QTime ProgramRecorder::timeToStart() const
{
    if (d->event) {
        QDateTime now = QDateTime::currentDateTime();
        quint64 msec = now.msecsTo(d->event->startDateTime());
        int h = msec / 3600000;
        msec = msec % 3600000;
        int m = msec / 60000;
        msec = msec % 60000;
        int s = msec / 1000;
        msec = msec % 1000;
        return QTime(h, m, s, msec);
    } else {
        return QTime();
    }
}

QTime ProgramRecorder::timeToEnd() const
{
    if (d->event) {
        QDateTime now = QDateTime::currentDateTime();
        quint64 msec = now.msecsTo(d->event->endDateTime());
        int h = msec / 3600000;
        msec = msec % 3600000;
        int m = msec / 60000;
        msec = msec % 60000;
        int s = msec / 1000;
        msec = msec % 1000;
        return QTime(h, m, s, msec);
    } else {
        return QTime();
    }
}


void ProgramRecorder::start()
{
    // Open file to record
    if (!d->file.open(QIODevice::ReadWrite)) {
        qWarning() << qPrintable(d->file.errorString());
        return;
    }

    setupConnection();

    // Change status to recording
    d->status = Recording;
    emit statusChanged(d->status);

    // Compute end time of event and launch a timer to stop recording if the record is based on an event
    if (d->event) {
        d->endTimerId = startTimer(1000);
    }

    // Notify recording has started
    emit recordingStarted();
}

void ProgramRecorder::stop()
{
    if (d->reply)
        d->reply->close();
    else
        finished();
}

void ProgramRecorder::readyRead()
{
    write(d->reply->readAll());
}

void ProgramRecorder::finished()
{
    if (d->file.isOpen())
        d->file.close();

    // Notify recording is done
    d->status = Done;
    emit statusChanged(d->status);
    emit recordingDone();
}

void ProgramRecorder::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == d->startTimeId) {
        QDateTime now = QDateTime::currentDateTime();
        if (now >= d->event->startDateTime()) {
            start();
            killTimer(e->timerId());
            d->startTimeId = 0;
        } else {
            emit timeToStartChanged(timeToStart());
            emit timeToEndChanged(timeToEnd());
        }
    } else if (e->timerId() == d->endTimerId) {
        QDateTime now = QDateTime::currentDateTime();
        if (now >= d->event->endDateTime()) {
            stop();
            killTimer(e->timerId());
            d->endTimerId = 0;
        } else {
            emit timeToEndChanged(timeToEnd());
        }
    }
}

void ProgramRecorder::setupConnection()
{
    // Start network upload
    QUrl url = d->program->toUrl(Program::HTTP, d->device->host());
    QNetworkRequest request(url);
    d->reply = d->manager.get(request);
    connect(d->reply, SIGNAL(readyRead()),
            this, SLOT(readyRead()));
    connect(d->reply, SIGNAL(finished()),
            this, SLOT(finished()));
}

void ProgramRecorder::write(const QByteArray &data)
{
    d->file.write(data);
}
