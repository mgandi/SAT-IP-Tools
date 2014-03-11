#include "table_p.h"
#include "event_p.h"

Table::Table(quint8 payload[], const quint8 payloadSize, QObject *parent) :
    QObject(parent),
    valid(false)
{
    if (payloadSize < 14)
        return;
    else
        valid = true;

    tableID = payload[0];
    sectionSyntaxIndicator = (payload[1] & 0x80) >> 7;
    sectionLength = (((quint16)payload[1] & 0x0F) << 8) | ((quint16)payload[2] & 0xFF);
    serviceID = (((quint16)payload[3] & 0xFF) << 8) | ((quint16)payload[4] & 0xFF);
    versionNumber = (payload[5] & 0x3E) >> 1;
    currentNextIndicator = payload[5] & 0x01;
    sectionNumber = payload[6];
    lastSectionNumber = payload[7];
    transportStreamID = ((((quint16)payload[8] & 0xFF) << 8) | ((quint16)payload[9] & 0xFF));
    originalNetworkID = ((((quint16)payload[10] & 0xFF) << 8) | ((quint16)payload[11] & 0xFF));
    segmentLastSectionNumber = payload[12];
    lastTableId = payload[13];

//    quint8 available = payloadSize - 14;
//    if ((available >= 12) && ((sectionLength - 14) > 12)) {
//        parseEvent(&payload[14], available);
//    }
}

quint64 Table::uid() const
{
    return ((quint64)originalNetworkID << 48 |
            (quint64)transportStreamID << 32 |
            (quint64)serviceID << 16 |
            (quint64)tableID << 8 |
            (quint64)sectionNumber);
}

//void Table::appendSection(quint8 payload[], const quint8 payloadSize, quint8 section)
//{
//    // Find a way to not miss sections ...

//    if (section == lastSectionNumber) {
//        complete = true;
//    }

//    parseEvent(payload, payloadSize);
//}

//void Table::parseEvent(quint8 payload[], const quint8 payloadSize)
//{
//    EventPrivate *event = new EventPrivate(payload, payloadSize, *this);
//    if (event->complete)
//        emit newEventAvailable(new Event(event));
//    else
//        emit newIncompleteEventAvailable(event);
//}
