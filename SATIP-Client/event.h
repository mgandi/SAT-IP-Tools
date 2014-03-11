#ifndef EVENT_H
#define EVENT_H

#include <QObject>

class EventPrivate;
class Event : public QObject
{
    Q_OBJECT

public:
    enum EventType {
        Undefined = 0x0,
        Movie     = 0x1,
        News      = 0x2,
        Show      = 0x3,
        Sports    = 0x4,
        Children  = 0x5,
        Music     = 0x6,
        Culture   = 0x7,
        Social    = 0x8,
        Education = 0x9,
        Leisure   = 0xA,
        Special   = 0xB,
        User      = 0xF
    };

    explicit Event(EventPrivate *p, QObject *parent = 0);
    ~Event();

    bool isValid();

    quint8 tableID() const;
    quint16 serviceID() const;
    quint8 versionNumber() const;
    quint16 originalNetworkID() const;
    quint16 transportStreamID() const;
    quint16 eventID() const;
    QDateTime startDateTime() const;
    QTime duration() const;
    QDateTime endDateTime() const;
    QString name() const;
    QString shortDescription() const;
    QString extendedDescription() const;
    EventType type() const;
    quint32 uid() const;

private:
    EventPrivate *d;
};

#endif // EVENT_H
