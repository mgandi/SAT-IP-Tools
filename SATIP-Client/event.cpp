#include "event.h"
#include "event_p.h"

Event::Event(EventPrivate *p, QObject *parent) :
    QObject(parent),
    d(p)
{
}

Event::~Event()
{
    delete d;
}


bool Event::isValid()
{
    return (d->serviceID > 0) && (!d->startDateTime.isNull()) && (!d->duration.isNull());
}


quint8 Event::tableID() const
{
    return d->tableID;
}

quint16 Event::serviceID() const
{
    return d->serviceID;
}

quint8 Event::versionNumber() const
{
    return d->versionNumber;
}

quint16 Event::originalNetworkID() const
{
    return d->originalNetworkID;
}

quint16 Event::transportStreamID() const
{
    return d->transportStreamID;
}

quint16 Event::eventID() const
{
    return d->eventID;
}

QDateTime Event::startDateTime() const
{
    return d->startDateTime;
}

QTime Event::duration() const
{
    return d->duration;
}

QDateTime Event::endDateTime() const
{
    return d->endDateTime;
}

QString Event::name() const
{
    return d->name;
}

QString Event::shortDescription() const
{
    return d->shortDescription;
}

QString Event::extendedDescription() const
{
    return d->extendedDescription;
}

Event::EventType Event::type() const
{
    return d->type;
}

quint32 Event::uid() const
{
    return ((quint32)d->transportStreamID << 16 |
            (quint32)d->serviceID);
}

