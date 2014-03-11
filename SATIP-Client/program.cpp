#include "program.h"
#include "elementarystream.h"
#include "objectpool.h"
#include "event.h"
#include "gatewaydevice.h"
#include "settings.h"

#include <QUrlQuery>
#include <QStringList>
#include <QString>

class ProgramPrivate
{
public:
    double frequency;
    quint8 bandwidth;
    quint16 symbolRate;
    Msys system;
    Mtype modulation;
    bool vPolarization;
    quint8 satPos;
    quint16 transportStreamID;
    quint16 number;
    quint16 pid;
    quint16 pcrPid;
    QString serviceProviderName;
    QString serviceName;
    QList<ElementaryStream *> es;
    QList<Event *> events;
    QList<quint16> scanPids; /* PIDs used during SCAN */
    bool scrambled;
};

Program::Program(QObject *parent) :
    QObject(parent),
    d(new ProgramPrivate())
{
    init();
}

Program::Program(const Program &other) :
    QObject(other.parent()),
    d(new ProgramPrivate())
{
    d->frequency = other.frequency();
    d->bandwidth = other.bandwidth();
    d->symbolRate = other.symbolRate();
    d->system = other.system();
    d->modulation = other.modulation();
    d->vPolarization = other.vPolarization();
    d->satPos = other.satPos();
    d->number = other.number();
    d->pid = other.pid();
    d->pcrPid = other.pcrPid();
    d->serviceProviderName = other.serviceProviderName();
    d->serviceName = other.serviceName();
    d->transportStreamID = other.transportStreamID();
    d->scrambled = 0;

    foreach (ElementaryStream *es, other.elementaryStreams()) {
        ElementaryStream *tmp = new ElementaryStream(*es);
        d->es += tmp;
    }
    d->scanPids.clear();
    init();
}

/* DVB-T */
Program::Program(double frequency, quint8 bandwidth, quint16 transportStreamID, quint16 number,
                 quint16 pid, QObject *parent) :
    QObject(parent),
    d(new ProgramPrivate())
{
    d->frequency = frequency;
    d->bandwidth = bandwidth;
    d->transportStreamID = transportStreamID;
    d->number = number;
    d->pid = pid;
    d->system = DVBT;
    d->scrambled = 0;
    d->scanPids.clear();
    init();
}

/* DVB-S, DVB-S2 */
Program::Program(double frequency, quint16 symbolRate, Msys system, Mtype modulation, bool vPol,
                 quint8 satPos, quint16 transportStreamID, quint16 number, quint16 pid, QObject *parent) :
    QObject(parent),
    d(new ProgramPrivate())
{
    d->frequency = frequency;
    d->symbolRate = symbolRate;
    d->system = system;
    d->modulation = modulation;
    d->vPolarization = vPol;
    d->satPos = satPos;
    d->transportStreamID = transportStreamID;
    d->number = number;
    d->pid = pid;
    d->scrambled = 0;
    d->scanPids.clear();
    init();
}

Program::~Program()
{
    delete d;
}


void Program::setFrequency(const double frequency)
{
    d->frequency = frequency;
}

double Program::frequency() const
{
    return d->frequency;
}

void Program::setBandwidth(const quint8 bandwidth)
{
    d->bandwidth = bandwidth;
}

quint8 Program::bandwidth() const
{
    return d->bandwidth;
}

void Program::setSymbolRate(const quint16 symbolRate)
{
    d->symbolRate = symbolRate;
}

quint16 Program::symbolRate() const
{
    return d->symbolRate;
}

void Program::setSystem(const Msys system)
{
    d->system = system;
}

Msys Program::system() const
{
    return d->system;
}

void Program::setModulation(const Mtype modulation)
{
    d->modulation = modulation;
}

Mtype Program::modulation() const
{
    return d->modulation;
}

void Program::setVPolarization(const bool vPol)
{
    d->vPolarization = vPol;
}

bool Program::vPolarization() const
{
    return d->vPolarization;
}

void Program::setSatPos(const quint8 satPos)
{
    d->satPos = satPos;
}

quint8 Program::satPos() const
{
    return d->satPos;
}

void Program::setNumber(const quint16 number)
{
    d->number = number;
}

quint16 Program::number() const
{
    return d->number;
}

void Program::setTransportStreamID(const quint16 transportStreamID)
{
    d->transportStreamID = transportStreamID;
}

quint16 Program::transportStreamID() const
{
    return d->transportStreamID;
}

void Program::setPid(const quint16 pid)
{
    d->pid = pid;
}

quint16 Program::pid() const
{
    return d->pid;
}

void Program::setPcrPid(const quint16 pcrPid)
{
    d->pcrPid = pcrPid;
    emit programChanged(this);
}

quint16 Program::pcrPid() const
{
    return d->pcrPid;
}

void Program::setServiceProviderName(const QString &serviceProviderName)
{
    d->serviceProviderName = serviceProviderName;
    emit programChanged(this);
}

QString Program::serviceProviderName() const
{
    return d->serviceProviderName;
}

void Program::setServiceName(const QString &serviceName)
{
    d->serviceName = serviceName;
    emit programChanged(this);
}

QString Program::serviceName() const
{
    return d->serviceName;
}

void Program::appendElementaryStream(quint16 pid, quint8 type)
{
    ElementaryStream *es = new ElementaryStream(pid, type, this);
    d->es += es;
    emit elementaryStreamAdded(es);
}

QList<ElementaryStream *> &Program::elementaryStreams() const
{
    return d->es;
}

void Program::clearElementaryStreams()
{
    d->es.clear();
}

void Program::setScrambled(bool value)
{
    d->scrambled = value;
}

bool Program::scrambled() const
{
    return d->scrambled;
}

bool Program::containsVideo() const
{
    bool so = false;
    foreach (ElementaryStream *es, d->es) {
        switch (es->type()) {
        case 0x01:
        case 0x02:
        case 0x1B:
            so = true;
            break;
        }
    }

    return so;
}

bool Program::containsAudio() const
{
    bool so = false;
    foreach (ElementaryStream *es, d->es) {
        switch (es->type()) {
        case 0x03:
        case 0x04:
        case 0x0f:
        case 0x11:
            so = true;
            break;
        }
    }

    return so;
}

quint32 Program::uid() const
{
    return ((quint32)d->transportStreamID << 16 |
            (quint32)d->number);
}


Program &Program::operator =(const Program &other)
{
    setParent(other.parent());

    d->frequency = other.frequency();
    d->bandwidth = other.bandwidth();
    d->symbolRate = other.symbolRate();
    d->system = other.system();
    d->modulation = other.modulation();
    d->vPolarization = other.vPolarization();
    d->satPos = other.satPos();
    d->number = other.number();
    d->pid = other.pid();
    d->pcrPid = other.pcrPid();
    d->serviceProviderName = other.serviceProviderName();
    d->serviceName = other.serviceName();

    foreach (ElementaryStream *es, other.elementaryStreams()) {
        ElementaryStream *tmp = new ElementaryStream(*es);
        d->es += tmp;
    }

    return *this;
}

void Program::appendScanPids(QList<quint16> pids)
{
    d->scanPids += pids;
}

void Program::appendScanPid(quint16 pid)
{
    d->scanPids += pid;
}

void Program::clearScanPids(void)
{
    d->scanPids.clear();
}

QUrl Program::toUrl(const Program::Scheme scheme, const QString &host,
                    const Program::Purposes &purposes, RTSPSession *session)
{
    QUrl url;
    QUrlQuery urlQuery;

    if (!purposes.testFlag(Program::UpdateOnlyPid)) {

        urlQuery.addQueryItem("freq", QString("%1").arg(frequency()));

        /* select frontend 5 if Dektec frequency is used */
        //if (frequency() == 10750)
        //   urlQuery.addQueryItem("fe", QString("%1").arg(5));

        switch (system()) {
        case DVBT:
           urlQuery.addQueryItem("msys", "dvbt");
           urlQuery.addQueryItem("bw", QString("%1").arg(bandwidth()));
           break;
        case DVBS:
        case DVBS2:
           urlQuery.addQueryItem("msys", system() == DVBS ? "dvbs" : "dvbs2");
           urlQuery.addQueryItem("sr", QString("%1").arg(symbolRate()));
           urlQuery.addQueryItem("pol", vPolarization() ? "v" : "h");
           urlQuery.addQueryItem("src", QString("%1").arg(satPos()));
           urlQuery.addQueryItem("mtype", modulation() == M_QPSK ? "qpsk" : "8psk");
        }
    }

    if (!purposes.testFlag(Program::UpdateNoPid)) {

        QStringList pids;
        pids += QString("0");

        if (purposes.testFlag(Program::EPG)) {
            pids += QString("18");
            pids += QString("20");
        }

        if (purposes.testFlag(Program::Watch)) {
            pids += QString("%1").arg(pid());
            foreach (ElementaryStream *es, elementaryStreams()) {
                pids += QString("%1").arg(es->pid());
            }
        }

        if (purposes.testFlag(Program::Scan)) {

            foreach (quint16 pid, d->scanPids) {
                pids += QString("%1").arg(pid);
            }
        }

        urlQuery.addQueryItem("pids", pids.join(","));
    }

    switch (scheme) {
    case RTSP:
        url.setScheme("rtsp");
        break;
    case HTTP:
        url.setScheme("http");
        Settings settings;
        url.setPort(settings.httpPort());
        break;
    }

    url.setHost(host);
    if (session)
        url.setPath(QString("/stream=%1").arg(session->streamid()));
    else
        url.setPath("/");
    url.setQuery(urlQuery);

    return url;
}

QList<quint16> Program::pidList(const Program::Purposes &purposes)
{
    QList<quint16> pids;
    pids += 0;

    if (purposes & Program::EPG) {
        pids += 18;
        pids += 20;
    }

    if (purposes & Program::Watch) {
        pids += pid();
        foreach (ElementaryStream *es, elementaryStreams()) {
            pids += es->pid();
        }
    }

    if (purposes & Program::Scan) {
        pids.clear();
        pids += 8191;
    }

    return pids;
}

void Program::init()
{
    connect(ObjectPool::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
}

void Program::objectAdded(QObject *object)
{
    Event *event = qobject_cast<Event *>(object);
    if (event && (event->serviceID() == number())) {
        d->events += event;
        event->setParent(this);
        emit newEventAvailable(event);
    }
}
