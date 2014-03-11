#ifndef PROGRAMTESTER_H
#define PROGRAMTESTER_H

#include <QObject>
#include <QTime>
#include "program.h"

class Event;
class GatewayDevice;
class Program;
class ProgramTesterPrivate;
class ProgramTester : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Idle                = 0x0,
        Testing             = 0x1,
        Done                = 0x2
    };

    explicit ProgramTester(GatewayDevice *device, Program *program,
                           Program::Scheme scheme = Program::HTTP, QObject *parent = 0);
    ~ProgramTester();

    Program *program() const;

    Status status() const;
    quint32 pidRef() const;
    quint32 numTs() const;
    quint32 tsCnt() const;
    quint32 tsOffs() const;
    quint32 err() const;
    void test(QByteArray data);

    void start();
    void stop();
    void clearError();

protected slots:
    void readyRead(); /* HTTP */
    void packetsAvailable(quint32 ssrc, QList<QByteArray> &packets); /* RTSP */
    void finished();

signals:
    void statusChanged(ProgramTester::Status status);
    void tsCntChanged(quint32 tsCnt);
    void errChanged(quint32 err);
    void testingDone();

private:
    ProgramTesterPrivate *d;
};

#endif // PROGRAMTESTER_H
