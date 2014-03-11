#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <QObject>

class ObjectPoolPrivate;
class ObjectPool : public QObject
{
    Q_OBJECT

public:
    explicit ObjectPool(QObject *parent = 0);
    ~ObjectPool();

    static ObjectPool *instance();

    bool addObject(QObject *object);
    bool removeObject(QObject *object);

protected slots:
    void objectDestroyed(QObject *object);

signals:
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

private:
    ObjectPoolPrivate *d;
    static ObjectPool *me;
};

#endif // OBJECTPOOL_H
