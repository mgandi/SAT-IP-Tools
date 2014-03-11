#include "epgparser.h"
#include "event.h"
#include "dvbpsihelper.h"
#include "table_p.h"
#include "event_p.h"

#include <QMutex>
#include <QMutexLocker>
#include <QHash>
#include <QtDebug>

class EpgParserPrivate
{
public:
    EpgParserPrivate() :
        incompleteEvent(0)
    {}

    QList<quint64> uids;
    EventPrivate *incompleteEvent;
};

EpgParser::EpgParser(QObject *parent) :
    QObject(parent),
    d(new EpgParserPrivate)
{
}

EpgParser::~EpgParser()
{
    delete d;
}


void EpgParser::push(const QByteArray &packet)
{
    if (packet.isNull())
        return;

    quint8 *data = (quint8 *)packet.data(), *payload = 0;
    quint8 afe = data[3] & 0x20;
    quint8 pusi = data[1] & 0x40;

    // Skip the adaptation field if present
    if (afe) //  Check if "Adaptation field exists" bit is set
        payload = data + 5 + data[4]; // Point after adaptation field
    else
        payload = data + 4; // Point to first byte after header

    // Skip new section begins if present
    if (pusi) // Check if "Payload unit start" bit is set
        payload = payload + *payload + 1;

    // If pusi is set then it shall be the begining of a new section
    if (pusi) {
        quint8 available = 188 + data - payload;

        // So look for table ID
        switch (payload[0]) {
        case 0x4E: // EIT event_information_section - actual_transport_stream, present/following
        case 0x4F: // EIT event_information_section - other_transport_stream, present/following
        //0x50 to 0x5F event_information_section - actual_transport_stream, schedule
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
        //0x60 to 0x6F event_information_section - other_transport_stream, schedule
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F: {
            Table table(payload, available);

            if (!table.valid)
                return;

            if (d->incompleteEvent) {
                qDebug() << "New table before event could be completed";
                delete d->incompleteEvent;
                d->incompleteEvent = 0;
            }

            if (!d->uids.contains(table.uid())) {
                qDebug() << "=========================================";
                qDebug() << "New table" << qPrintable(QString("0x%1").arg(table.uid(), 16, 16, QLatin1Char('0')));
                d->uids += table.uid();

                available -= 14;
                payload += 14;
                if ((available >= 12) && ((table.sectionLength - 14) > 12)) {
                    EventPrivate *event = new EventPrivate(payload, available, table);
                    if (event->complete) {
                        qDebug() << "Event complete";
                        emit newEventAvailable(new Event(event));
                    } else {
                        qDebug() << "Event incomplete";
                        d->incompleteEvent = event;
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    } else { // If no discontinuity then this packet contains remaining bytes of descriptors of last treated event of last treated table
        if (d->incompleteEvent) {
            qDebug() << "Event complementary data";
            quint8 available = 188 + data - payload;
            d->incompleteEvent->completeDescriptors(payload, available);
            if (d->incompleteEvent->complete) {
                qDebug() << "Event complete";
                emit newEventAvailable(new Event(d->incompleteEvent));
                d->incompleteEvent = 0;
            }
        }
    }
}

void EpgParser::discontinuity()
{
    if (d->incompleteEvent) {
        qDebug() << "Discontinuity before event could be completed";
        delete d->incompleteEvent;
        d->incompleteEvent = 0;
    }
}

void EpgParser::resetIncompleteEvent()
{
    if (d->incompleteEvent) {
        delete d->incompleteEvent;
        d->incompleteEvent = 0;
    }
}
