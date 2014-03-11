#include "demuxer.h"
#include "abstractpidhandler.h"

#include <QHash>
#include <QDebug>

class PidHandler;
class DemuxerPrivate
{
public:
    QHash<quint16, AbstractPidHandler*> handlers;
};

Demuxer::Demuxer(QObject *parent) :
    QObject(parent),
    d(new DemuxerPrivate)
{
}

Demuxer::~Demuxer()
{
    delete d;
}

void Demuxer::addPidHandler(const quint16 pid, AbstractPidHandler *handler)
{
    if (!d->handlers.value(pid))
       d->handlers.insert(pid, handler);
}

void Demuxer::removePidHandler(const quint16 pid)
{
    d->handlers.remove(pid);
}

void Demuxer::removePidHandler(AbstractPidHandler *handler)
{
    quint16 pid = d->handlers.key(handler);
    d->handlers.remove(pid);
}

void Demuxer::clear()
{
    d->handlers.clear();
}

void Demuxer::push(QList<QByteArray> packets)
{
    foreach (QByteArray packet, packets) {
        const quint8 *data = (const quint8 *)packet.constData();
        quint16 pid = (((quint16)data[1] & 0x1F) << 8) | ((quint16)data[2] & 0xFF);
        AbstractPidHandler *handler = d->handlers.value(pid);
        if (handler)
            handler->push(packet);
    }
}
