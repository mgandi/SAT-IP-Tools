#ifndef DEMUX_H
#define DEMUX_H

#include <QObject>

class DemuxPrivate;
class Demux : public QObject
{
    Q_OBJECT

public:
    explicit Demux(QObject *parent = 0);
    ~Demux();

public slots:
    void push(QList<QByteArray> packets);

private:
    DemuxPrivate *d;
};

#endif // DEMUX_H
