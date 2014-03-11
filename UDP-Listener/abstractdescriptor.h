#ifndef ABSTRACTDESCRIPTOR_H
#define ABSTRACTDESCRIPTOR_H

#include <QObject>
#include <QMap>

class AbstractDescriptorPrivate;
class AbstractDescriptor : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDescriptor(quint8 tag, const QByteArray &descriptor, QObject *parent = 0);
    ~AbstractDescriptor();

protected:
    quint8 tag() const;
    QByteArray &descriptor() const;

private:
    AbstractDescriptorPrivate *d;
};

#endif // ABSTRACTDESCRIPTOR_H
