#ifndef EVENT_P_H
#define EVENT_P_H

#include "event.h"
#include "table_p.h"

#include <QtGlobal>
#include <QDateTime>
#include <QMap>

class EventPrivate
{
public:
    EventPrivate();
    EventPrivate(quint8 payload[], const quint8 payloadSize, const Table &table);

    void completeDescriptors(quint8 payload[], const quint8 payloadSize);
    void parseDescriptors();

    quint8  tableID;
    quint16 serviceID;
    quint8  versionNumber;
    quint16 transportStreamID;
    quint16 originalNetworkID;

    quint16 eventID;
    QDateTime startDateTime;
    QTime duration;
    QDateTime endDateTime;
    quint8 runningStatus;
    quint8 freeCAMode;
    quint16 descriptorsLoopLength;
    QByteArray descriptors;

    QString name;
    QString shortDescription;
    QString extendedDescription;
    Event::EventType type;
    QMap<QString, QString> items;

    bool complete;
};

#endif // EVENT_P_H
