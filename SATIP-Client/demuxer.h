#ifndef DEMUXER_H
#define DEMUXER_H

#include <QObject>

class AbstractPidHandler;
class DemuxerPrivate;
class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = 0);
    ~Demuxer();

    void addPidHandler(const quint16 pid, AbstractPidHandler *handler);
    void removePidHandler(const quint16 pid);
    void removePidHandler(AbstractPidHandler *handler);
    void clear();

private:
    DemuxerPrivate *d;

public slots:
    void push(QList<QByteArray> packets);
};

#endif // DEMUXER_H
