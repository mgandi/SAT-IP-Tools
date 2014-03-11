#include "abstractdescriptor.h"

class AbstractDescriptorPrivate
{
public:
    quint8 tag;
    QByteArray descriptor;
};

AbstractDescriptor::AbstractDescriptor(quint8 tag, const QByteArray &descriptor, QObject *parent) :
    QObject(parent),
    d(new AbstractDescriptorPrivate)
{
    d->tag = tag;
    d->descriptor = descriptor;
}

AbstractDescriptor::~AbstractDescriptor()
{
    delete d;
}

quint8 AbstractDescriptor::tag() const
{
    return d->tag;
}


QByteArray &AbstractDescriptor::descriptor() const
{
    return d->descriptor;
}
