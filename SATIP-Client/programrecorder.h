#ifndef PROGRAMRECORDER_H
#define PROGRAMRECORDER_H

#include <QObject>
#include <QTime>

class Event;
class GatewayDevice;
class Program;
class ProgramRecorderPrivate;
class ProgramRecorder : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Idle                = 0x0,
        WaitingForStartTime = 0x1,
        Recording           = 0x2,
        Done                = 0x3
    };

    explicit ProgramRecorder(GatewayDevice *device, Event *event, QObject *parent = 0);
    explicit ProgramRecorder(GatewayDevice *device, Program *program, const QString &fileName, QObject *parent = 0);
    ~ProgramRecorder();

    Program *program() const;
    Status status() const;
    QDateTime startTime() const;
    QDateTime endTime() const;
    QTime timeToStart() const;
    QTime timeToEnd() const;

    void start();
    virtual void stop();

protected slots:
    void readyRead();
    void finished();

    void timerEvent(QTimerEvent *e);

    virtual void setupConnection();
    void write(const QByteArray &data);

signals:
    void statusChanged(ProgramRecorder::Status status);
    void recordingStarted();
    void recordingDone();
    void timeToStartChanged(QTime timeToStart);
    void timeToEndChanged(QTime timeToEnd);

private:
    ProgramRecorderPrivate *d;
};

#endif // PROGRAMRECORDER_H
