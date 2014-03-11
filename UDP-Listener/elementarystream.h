#ifndef ELEMENTARYSTREAM_H
#define ELEMENTARYSTREAM_H

#include <QObject>

class ElementaryStreamPrivate;
class ElementaryStream : public QObject
{
    Q_OBJECT
public:
    explicit ElementaryStream(const ElementaryStream &other);
    explicit ElementaryStream(quint16 pid, quint8 type, QObject *parent = 0);
    ~ElementaryStream();

    quint16 pid() const;
    quint8 type() const;
    QString typeToString() const;

private:
    ElementaryStreamPrivate *d;
};

#endif // ELEMENTARYSTREAM_H
