#include "objectpool.h"

ObjectPool *ObjectPool::me = 0;



class ObjectPoolPrivate
{
public:
    QList<QObject *> objects;
};



ObjectPool::ObjectPool(QObject *parent) :
    QObject(parent),
    d(new ObjectPoolPrivate)
{
    me = this;
}

ObjectPool::~ObjectPool()
{
    delete d;
}


ObjectPool *ObjectPool::instance()
{
    return me;
}


bool ObjectPool::addObject(QObject *object)
{
    if (d->objects.contains(object))
        return false;

    d->objects += object;
    connect(object, SIGNAL(destroyed(QObject*)),
            this, SLOT(objectDestroyed(QObject*)));
    emit objectAdded(object);
    return true;
}

bool ObjectPool::removeObject(QObject *object)
{
    if (d->objects.contains(object)) {
        d->objects.removeAll(object);
        emit objectRemoved(object);
        return true;
    }

    return false;
}

void ObjectPool::objectDestroyed(QObject *object)
{
    if (d->objects.contains(object)) {
        d->objects.removeAll(object);
        emit objectRemoved(object);
    }
}
