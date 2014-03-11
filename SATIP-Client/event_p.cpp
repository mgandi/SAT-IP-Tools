#include "event_p.h"
#include "dvbpsihelper.h"

#include <QtDebug>

EventPrivate::EventPrivate()
{
}

EventPrivate::EventPrivate(quint8 payload[], const quint8 payloadSize, const Table &table) :
    type(Event::Undefined),
    complete(false)
{
    tableID = table.tableID;
    serviceID = table.serviceID;
    versionNumber = table.versionNumber;
    transportStreamID = table.transportStreamID;
    originalNetworkID = table.originalNetworkID;

    quint8 available = payloadSize;
    eventID = (((quint16)payload[0] & 0xFF) << 8) | ((quint16)payload[1] & 0xFF);
    startDateTime = DvbPsiHelper::dateTimeFromMJD_BCD((((quint64)payload[2] & 0xFF) << 32) | (((quint64)payload[3] & 0xFF) << 24) | (((quint64)payload[4] & 0xFF) << 16) | (((quint64)payload[5] & 0xFF) << 8) | ((quint64)payload[6] & 0xFF));
    duration = DvbPsiHelper::timeFromBCD((((quint32)payload[7] & 0xFF) << 16) | (((quint32)payload[8] & 0xFF) << 8) | ((quint32)payload[9] & 0xFF));
    endDateTime = startDateTime.addMSecs((duration.hour() * 3600000) + (duration.minute() * 60000) + (duration.second() * 1000));
    descriptorsLoopLength = (((quint16)payload[10] & 0x0F) << 8) | ((quint16)payload[11] & 0xFF);

    quint8 *descriptor = (quint8 *)&payload[12];
    available = available + payload - descriptor;

    if (descriptorsLoopLength > available) {
        descriptors += QByteArray((const char *)descriptor, (int)available);
        descriptorsLoopLength -= available;
    } else {
        descriptors += QByteArray((const char *)descriptor, (int)descriptorsLoopLength);
        descriptorsLoopLength = 0;
        complete = true;
        parseDescriptors();
    }
}

void EventPrivate::completeDescriptors(quint8 payload[], const quint8 payloadSize)
{
    if (descriptorsLoopLength > payloadSize) {
        descriptors += QByteArray((const char *)payload, (int)payloadSize);
        descriptorsLoopLength -= payloadSize;
    } else {
        descriptors += QByteArray((const char *)payload, (int)descriptorsLoopLength);
        descriptorsLoopLength = 0;
        complete = true;
        parseDescriptors();
    }
}

void EventPrivate::parseDescriptors()
{
    quint16 available = descriptors.size();
    quint8 *payload = (quint8 *)descriptors.data();
    while (available) {
        quint8 tag = payload[0];
        quint8 length = payload[1];
        quint8 *data = &payload[2];

        switch (tag) {
        case 0x4D: {
            quint32 iso639LanguageCode = ((quint32)data[0] << 16) | ((quint32)data[1] << 8) | (quint32)data[2];
            quint32 eventNameLength = data[3];
            quint8 *eventName = &data[4];
            quint8 eventTextLength = data[4 + eventNameLength];
            quint8 *eventText = &data[4 + eventNameLength + 1];
            name = DvbPsiHelper::stringFromAnnexA((const char *)eventName, (int)eventNameLength);
            shortDescription = DvbPsiHelper::stringFromAnnexA((const char *)eventText, (int)eventTextLength);
            break;
        }
        case 0x4E: {
            quint8 remaining = length;
            quint8 descriptorNumber = data[0] >> 4;
            quint8 lastDescriptorNumber = data[0] & 0x0F;
            quint32 iso639LanguageCode = ((quint32)data[1] << 16) | ((quint32)data[2] << 8) | (quint32)data[3];
            quint8 lengthOfItems = data[4];
            remaining -= 5;
            quint32 offset = 0;
            if (remaining >= lengthOfItems + 1) {
                while (lengthOfItems - offset) {
                    quint8 itemDescriptionLength = data[5 + offset];
                    quint8 *itemDescription = (quint8 *)&data[6 + offset];
                    quint8 itemLength = data[6 + offset + itemDescriptionLength];
                    quint8 *item = (quint8 *)&data[6 + offset + itemDescriptionLength + 1];
                    offset += 2 + itemDescriptionLength + itemLength;
                }
            }
            remaining -= lengthOfItems;
            quint8 textLength = data[5 + offset];
            quint8 *text = (quint8 *)&data[6 + offset];
            --remaining;
            if (remaining >= textLength)
                extendedDescription = DvbPsiHelper::stringFromAnnexA((const char *)text, (int)textLength);
            break;
        }
        case 0x54:
            type = (Event::EventType)(data[0] >> 4);
            break;
        default:
            break;
        }

        available = (length + 2) < available ? available - length -2 : 0;
        payload += length + 2;
    }
}


