#include "programrecorderrtsp.h"
#include "gatewaydevice.h"
#include "rtspsession.h"

class ProgramRecorderRTSPPrivate
{
public:
    ProgramRecorderRTSPPrivate() :
        session(0)
    {}

    GatewayDevice *device;
    Program *program;
    RTSPSession *session;
};

ProgramRecorderRTSP::ProgramRecorderRTSP(GatewayDevice *device, Program *program, const QString &fileName, QObject *parent) :
    ProgramRecorder(device, program, fileName, parent),
    d(new ProgramRecorderRTSPPrivate)
{
    d->device = device;
    d->program = program;
}

ProgramRecorderRTSP::~ProgramRecorderRTSP()
{
    delete d;
}


void ProgramRecorderRTSP::stop()
{
    if (d->session) {
        d->device->removeSession(d->session);
        d->session = 0;
    }
    finished();
}


void ProgramRecorderRTSP::setupConnection()
{
    d->session = d->device->addSession(d->program);
    connect(d->session, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
            this, SLOT(packetsAvailable(quint32,QList<QByteArray>&)));
}


void ProgramRecorderRTSP::packetsAvailable(quint32 /*ssrc*/, QList<QByteArray> &packets)
{
    foreach (QByteArray packet, packets) {
        write(packet);
    }
}
