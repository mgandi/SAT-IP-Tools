#ifndef TABLE_H
#define TABLE_H

#include <QObject>
#include <QHash>

class Event;
class EventPrivate;
class Table : public QObject
{
    Q_OBJECT
public:
    explicit Table(quint8 payload[], const quint8 payloadSize, QObject *parent = 0);

    quint64 uid() const;
//    void appendSection(quint8 payload[], const quint8 payloadSize, quint8 section);
//    void parseEvent(quint8 payload[], const quint8 payloadSize);

    quint8  tableID;
    quint8  sectionSyntaxIndicator;
    quint16 sectionLength;
    quint16 serviceID;
    quint8  versionNumber;
    quint8  currentNextIndicator;
    quint8  sectionNumber;
    quint8  lastSectionNumber;
    quint16 transportStreamID;
    quint16 originalNetworkID;
    quint8  segmentLastSectionNumber;
    quint8  lastTableId;

    bool valid;

//signals:
//    void newEventAvailable(Event *);
//    void newIncompleteEventAvailable(EventPrivate *);
};

#endif // TABLE_H
