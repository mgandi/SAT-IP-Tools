#include "epgcollector.h"
#include "gatewaydevice.h"
#include "program.h"
#include "demuxhandler.h"
#include "rtspsession.h"
#include "eithandler.h"
#include "event.h"
#include "objectpool.h"
#include "dvbpsihelper.h"
#include "epgparser.h"

#include <QTimerEvent>
#include <QtDebug>
#include <QtEndian>

#define TIMEOUT     5000
#define INVALID_CC  0x10

class EpgCollectorPrivate
{
public:
    EpgCollectorPrivate(EpgCollector *parent) :
        p(parent),
        epgSession(0),
        currentProgram(0),
        index(0),
        timerID(0)
    {
        QObject::connect(&parser, SIGNAL(newEventAvailable(Event*)),
                         p, SLOT(newEventAvailable(Event*)));
    }

    EpgCollector *p;
    GatewayDevice *device;
    RTSPSession *epgSession;
    Program *currentProgram;
    QList<Program *> programs;
    int index;
    QHash<quint16, Event *> events;
    int timerID;

    EpgParser parser;
};

EpgCollector::EpgCollector(GatewayDevice *device, bool s, QObject *parent) :
    QObject(parent),
    d(new EpgCollectorPrivate(this))
{
    d->device = device;
    d->programs = device->programs();
    if (s) start();
}

EpgCollector::~EpgCollector()
{
    if (d->epgSession) {
        d->device->removeSession(d->epgSession);
        d->epgSession->stop();
//        d->epgSession->wait();
        delete d->epgSession;
        d->epgSession = 0;
    }
    delete d;
}


void EpgCollector::start(Program *program)
{
    int index = 0;

    if (program && d->programs.contains(program)) {
        index = d->programs.indexOf(program);
    }

    // Point to first program
    d->index = index;

    // Let's go!!!
    step();
}

void EpgCollector::stop()
{
    // Stop timer if one running
    if (d->timerID) {
        killTimer(d->timerID);
        d->timerID = 0;
    }

    // Clear current program
    emit currentProgramChanged(0, d->currentProgram);
    d->currentProgram = 0;

    // Stop and delete RTSP session if have one
    if (d->epgSession) {
        d->device->removeSession(d->epgSession, true);
        d->epgSession->stop();
//        d->epgSession->wait();
        delete d->epgSession;
        d->epgSession = 0;
    }
}

void EpgCollector::step()
{
    // Loop to first program if currently pointing to out of range index
    if (d->index >= d->programs.size())
        d->index = 0;

    // Extract program corresponding to current index
    Program *program = d->programs[d->index];
    QUrl url = program->toUrl(Program::RTSP, d->device->host(), Program::EPG);

    // Notify that current program changed
    emit currentProgramChanged(program, d->currentProgram);
    d->currentProgram = program;

    // If no session, create it
    if (!d->epgSession) {
        RTSPSession *session = new RTSPSession(d->device->localAddress(), url, true, true, this);
        d->epgSession = d->device->addSession(session);
        if (!d->epgSession) {
            qWarning() << "Unable to add RTSP session";
            session->stop();
//            session->wait();
            delete session;
            return;
        } else {
            connect(d->epgSession, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
                    this, SLOT(packetsAvailable(quint32,QList<QByteArray>&)));
        }
    } else { // Otherwise just set new url to point to
        d->epgSession->setUrl(url);
    }

    // Point to next program
    ++d->index;

    // Launch timeout timer
    d->timerID = startTimer(TIMEOUT);
}


void EpgCollector::packetsAvailable(quint32, QList<QByteArray> &packets)
{
    static quint8 ecc = INVALID_CC;

    foreach (QByteArray packet, packets) {
        quint8 *data = (quint8 *)packet.data();
        quint16 pid = (((quint16)data[1] & 0x1F) << 8) | ((quint16)data[2] & 0xFF);
        if (pid == 0x12) {
            quint8 ccc = data[3] & 0x0F;

            // Check validity of continuity counter
            if (ecc == INVALID_CC) { // Conintuity counter has not been yet initialized
                ecc = ccc; // Initialize continuity counter with currrent value
            } else {
                ecc = ++ecc & 0x0F; // Compute next contonuity counter
                if (ecc != ccc) { // Check if discontinuity
                    qDebug() << "Discontinuity!\n";
                    ecc = ccc;
                    d->parser.discontinuity();
                }
            }

            d->parser.push(packet);
        }
    }
}

void EpgCollector::newEventAvailable(Event *e)
{
    if (!d->events.contains(e->eventID()) && e->isValid()) {
        // Re-arm timer
        killTimer(d->timerID);
        d->timerID = startTimer(TIMEOUT);

        // Add event to list of events
        d->events.insert(e->eventID(), e);

        // Print content of event for debug
        qDebug() << "+++++++++++++++++++++++++++++++++++++++++";
        qDebug() << "Table ID          : " << qPrintable(QString("0x%1").arg(e->tableID(), 2, 16, QLatin1Char('0')));
        qDebug() << "Service ID        : " << qPrintable(QString("0x%1").arg(e->serviceID(), 4, 16, QLatin1Char('0')));
        qDebug() << "Event ID          : " << e->eventID();
        qDebug() << "Start time        : " << qPrintable(e->startDateTime().toString("ddd MMMM d yyyy - hh:mm"));
        qDebug() << "Duration          : " << qPrintable(e->duration().toString("hh:mm"));
        qDebug() << "Name              : " << qPrintable(e->name());
        qDebug() << "Short description : " << qPrintable(e->shortDescription());

        // Add event to object pool
        ObjectPool::instance()->addObject(e);
    }
}

void EpgCollector::timerEvent(QTimerEvent *e)
{
    // Kill timer
    killTimer(e->timerId());

    // Step to next program
    step();
}
