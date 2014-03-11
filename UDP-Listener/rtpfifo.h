#ifndef RTPFIFO_H
#define RTPFIFO_H

#include <QObject>

class RtpFifoPrivate;
class RtpFifo : public QObject
{
    Q_OBJECT
public:
    explicit RtpFifo(quint32 buffering, QObject *parent = 0);
    ~RtpFifo();

    void setBuffering(quint32 buffering);
    quint32 buffering() const;

    bool push(quint16 packetNumber, QByteArray datagram);
    QByteArray pop();

private:
    RtpFifoPrivate *d;
};

#endif // RTPFIFO_H
