#include "rtpfifo.h"

#include <QMap>

class RtpFifoPrivate
{
public:
    RtpFifoPrivate() :
        current(&buffer0),
        nbPackets(0),
        buffering(0),
        currentKey(0),
    {}

    QMap<quint16, QByteArray> buffer0, buffer1;
    QMap<quint16, QByteArray> *current;
    quint32 nbPackets, buffering;
    quint16 currentKey;
};

RtpFifo::RtpFifo(quint32 buffering, QObject *parent) :
    QObject(parent),
    d(new RtpFifoPrivate)
{
    setBuffering(buffering);
}

RtpFifo::~RtpFifo()
{
    delete d;
}

void RtpFifo::setBuffering(quint32 buffering)
{
    d->buffering = buffering;
}

quint32 RtpFifo::buffering() const
{
    return d->buffering;
}


bool RtpFifo::push(quint16 packetNumber, QByteArray datagram)
{
    if (d->current->lastKey())
    d->current->insert(packetNumber, datagram);
    ++d->nbPackets;
}

QByteArray RtpFifo::pop()
{
    if (d->nbPackets >= d->buffering) {
        --d->nbPackets;
        return d->current->take(d->current->firstKey());
    }
}
