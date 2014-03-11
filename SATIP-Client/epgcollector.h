#ifndef EPGCOLLECTOR_H
#define EPGCOLLECTOR_H

#include <QObject>

class Event;
class AbstractDvbPsiHandler;
class Program;
class GatewayDevice;
class EpgCollectorPrivate;
class EpgCollector : public QObject
{
    Q_OBJECT

public:
    explicit EpgCollector(GatewayDevice *device, bool start = true, QObject *parent = 0);
    ~EpgCollector();

    void start(Program *program = 0);
    void stop();

protected:
    void step();

protected slots:
    void packetsAvailable(quint32,QList<QByteArray>&);
    void newEventAvailable(Event *);
    void timerEvent(QTimerEvent *e);

signals:
    void currentProgramChanged(Program *newProgram, Program *oldProgram);

private:
    EpgCollectorPrivate *d;
};

#endif // EPGCOLLECTOR_H
