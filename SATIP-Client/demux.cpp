#include "demux.h"

class DemuxPrivate
{
public:
};

Demux::Demux(QObject *parent) :
    QObject(parent),
    d(new DemuxPrivate)
{
}

Demux::~Demux()
{
    delete d;
}


void Demux::push(QList<QByteArray> packets)
{
    foreach (QByteArray packet, packets) {
//        const char *data = packet.constData();
    }
}
