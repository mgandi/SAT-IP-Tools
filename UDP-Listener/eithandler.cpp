#include "eithandler.h"
#include "abstractdvbpsihandler_p.h"
#include "dvbpsihelper.h"
#include "demuxhandler.h"
#include "descriptor.h"
#include "tables/eit.h"
#include "descriptors/dr_4d.h"
#include "descriptors/dr_4e.h"
#include "descriptors/dr_50.h"
#include "descriptors/dr_54.h"
#include "descriptors/dr_55.h"
#include "event.h"

#include <QtDebug>

class EventPrivate
{
public:
    EventPrivate() :
        tableID(0),
        lastTableID(0),
        serviceID(0),
        versionNumber(0),
        networkID(0),
        tsID(0),
        eventID(0)
    {}

    quint8 tableID;
    quint8 lastTableID;
    quint16 serviceID;
    quint8 versionNumber;
    quint16 networkID;
    quint16 tsID;
    quint16 eventID;
    QDateTime startDateTime;
    QTime duration;
    QString name;
    QString shortDescription;
    Event::EventType type;
};

//Event::Event(EventPrivate *p, QObject *parent) :
//    QObject(parent),
//    d(p)
//{
//}

//Event::Event(const QString &name, const QString &shortDescription, const QDateTime &startDateTime, const QTime &duration, QObject *parent) :
//    QObject(parent),
//    d(new EventPrivate)
//{
//    d->name = name;
//    d->shortDescription = shortDescription;
//    d->startDateTime = startDateTime;
//    d->duration = duration;
//}

//Event::~Event()
//{
//    delete d;
//}


//bool Event::isValid()
//{
//    return (d->serviceID > 0) && (!d->startDateTime.isNull()) && (!d->duration.isNull());
//}


//quint8 Event::tableID() const
//{
//    return d->tableID;
//}

//quint8 Event::lastTableID() const
//{
//    return d->lastTableID;
//}

//quint16 Event::serviceID() const
//{
//    return d->serviceID;
//}


//quint8 Event::versionNumber() const
//{
//    return d->versionNumber;
//}

//quint16 Event::networkID() const
//{
//    return d->networkID;
//}

//quint16 Event::tsID() const
//{
//    return d->tsID;
//}

//quint16 Event::eventID() const
//{
//    return d->eventID;
//}

//QDateTime Event::startDateTime() const
//{
//    return d->startDateTime;
//}

//QTime Event::duration() const
//{
//    return d->duration;
//}

//QString Event::name() const
//{
//    return d->name;
//}

//QString Event::shortDescription() const
//{
//    return d->shortDescription;
//}

//Event::EventType Event::type() const
//{
//    return d->type;
//}



class EitHandlerPrivate
{
public:
    EitHandlerPrivate(EitHandler *parent):
        p(parent) {}

    EitHandler *p;

    quint8 tableID;
    quint16 serviceID;
    QHash<quint16, Event *> events;
    QList<Event *> latestEvents;
};

static void dumpDescriptors(dvbpsi_descriptor_t* descriptor, EventPrivate *e = 0);
static void dumpDescriptors(dvbpsi_descriptor_t* descriptor, EventPrivate *e)
{
    while(descriptor) {
//        qDebug() << "| Descriptor " << qPrintable(QString("(0x%1)").arg(descriptor->i_tag, 2, 16));
        switch (descriptor->i_tag) {
        case 0x4D: { // Short event descriptor
            dvbpsi_short_event_dr_t *sed = dvbpsi_DecodeShortEventDr(descriptor);
//            qDebug() << "| Event name        : " << qPrintable(QString::fromLatin1(QByteArray((const char *)sed->i_event_name, sed->i_event_name_length)));
//            qDebug() << "| Event text        : " << qPrintable(QString::fromLatin1(QByteArray((const char *)sed->i_text, sed->i_text_length)));
            if (e && sed) {
                e->name = QString::fromLatin1(QByteArray((const char *)sed->i_event_name, sed->i_event_name_length));
                e->shortDescription = QString::fromLatin1(QByteArray((const char *)sed->i_text, sed->i_text_length));
            }
            break;
        }
        case 0x4E: { // Extended event descriptor
//            dvbpsi_extended_event_dr_t *eed = dvbpsi_DecodeExtendedEventDr(descriptor);
//            qDebug() << "| ISO 693 lang code : " << eed->i_iso_639_code;
//            qDebug() << "| Event name        : " << qPrintable(QString(QByteArray((const char *)eed->i_text, eed->i_text_length)));
//            eed->
//            break;
        }
        case 0x50: // Component descriptor
            break;
        case 0x54: { // Content descriptor
            dvbpsi_content_dr_t *cd = dvbpsi_DecodeContentDr(descriptor);
            if (e && cd && cd->i_contents_number) {
                e->type = (Event::EventType)DVDPSI_GetContentCategoryFromType(cd->p_content[0].i_type);
            }
            break;
        }
        case 0x55: // Parental rating descriptor
            break;
        }
        descriptor = descriptor->p_next;
    }
}

static void callback(void* data, dvbpsi_eit_t* eit)
{
    EitHandlerPrivate *ehp = (EitHandlerPrivate *)data;
    bool dataChanged = false;
    dvbpsi_eit_event_t *event = eit->p_first_event;

//    // If current next indicator is false then data has changed
//    dataChanged = !eit->b_current_next;

    QString tableDescription;
    switch (eit->i_table_id) {
    case 0x4E: // EIT event_information_section - actual_transport_stream, present/following
        tableDescription = "current TS - present/following";
        break;
    case 0x4F: // EIT event_information_section - other_transport_stream, present/following
        tableDescription = "other TS - present/following";
        break;
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
        tableDescription = "current TS - schedule";
        break;
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
    case 0x6F:
        tableDescription = "other TS - schedule";
        break;
    default:
        tableDescription = "unknown";
        break;
    }

//    qDebug() << "=========================================";
//    qDebug() << "EIT: Event Information Table for" << qPrintable(tableDescription);
//    qDebug() << "Table ID          : " << qPrintable(QString("0x%1").arg(eit->i_table_id, 2, 16, QLatin1Char('0')));
//    qDebug() << "Last table ID     : " << qPrintable(QString("0x%1").arg(eit->i_last_table_id, 2, 16, QLatin1Char('0')));
//    qDebug() << "Version number    : " << eit->i_version;
//    qDebug() << "Current next      : " << (eit->b_current_next ? "yes" : "no");
//    qDebug() << "TS id             : " << eit->i_ts_id;
//    qDebug() << "Network id        : " << eit->i_network_id;
//    qDebug() << "Seg last sec #    : " << eit->i_segment_last_section_number;
//    qDebug() << "Last table ID     : " << eit->i_last_table_id;

    while (event) {
//        qDebug() << "=========================================";
//        qDebug() << "| Table ID          : " << qPrintable(QString("0x%1").arg(eit->i_table_id, 2, 16, QLatin1Char('0')));
//        qDebug() << "| Last table ID     : " << qPrintable(QString("0x%1").arg(eit->i_last_table_id, 2, 16, QLatin1Char('0')));
//        qDebug() << "| Service ID        : " << eit->i_extension;
//        qDebug() << "| Event ID          : " << event->i_event_id;
//        qDebug() << "| Start time        : " << DvbPsiHelper::fromMJD_BCD(event->i_start_time).toString("ddd MMMM d yyyy - h:m");
//        qDebug() << "| Duration          : " << DvbPsiHelper::timeFromBCD(event->i_duration).toString("h:m");
        if (!ehp->events.contains(event->i_event_id)) {
            EventPrivate *e = new EventPrivate;
            e->tableID = eit->i_table_id;
            e->lastTableID = eit->i_last_table_id;
            e->serviceID = eit->i_extension;
            e->versionNumber = eit->i_version;
            e->networkID = eit->i_network_id;
            e->tsID = eit->i_ts_id;
            e->eventID = event->i_event_id;
            e->startDateTime = DvbPsiHelper::dateTimeFromMJD_BCD(event->i_start_time);
            e->duration = DvbPsiHelper::timeFromBCD(event->i_duration);
            dumpDescriptors(event->p_first_descriptor, e);

            Event *ev = new Event(e);
            ehp->events.insert(event->i_event_id, ev);
            ehp->latestEvents += ev;
            dataChanged = true;
            emit ehp->p->newEventAvailable(ev);
        } else {
            dumpDescriptors(event->p_first_descriptor);
        }
        event = event->p_next;
    }

    // Signal that data changed
    if (dataChanged)
        emit ehp->p->dataChanged();

    // Delete eit
    dvbpsi_eit_delete(eit);
}

EitHandler::EitHandler(const quint8 tableID, const quint16 serviceID, const AbstractDvbPsiHandler &other, DemuxHandler *demuxHandler) :
    AbstractDvbPsiHandler(other),
    d(new EitHandlerPrivate(this))
{
    connect(demuxHandler, SIGNAL(dettached()),
            this, SLOT(demuxDettached()));

    d->tableID = tableID;
    d->serviceID = serviceID;
}

EitHandler::~EitHandler()
{
    delete d;
}

QList<Event *> EitHandler::events() const
{
    return d->events.values();
}

Event *EitHandler::popEvent()
{
    if (!d->latestEvents.isEmpty())
        return d->latestEvents.takeFirst();
    else
        return 0;
}


void EitHandler::attach()
{
    Q_ASSERT(dvbpsi_eit_attach(abstractDvbPsiHandlerPrivate()->dvbpsi, (quint8)d->tableID, (quint16)d->serviceID, callback, (void *)d));
    AbstractDvbPsiHandler::d->attached = true;
}

void EitHandler::dettach()
{
    dvbpsi_eit_detach(AbstractDvbPsiHandler::d->dvbpsi, (quint8)d->tableID, (quint16)d->serviceID);
    AbstractDvbPsiHandler::d->attached = false;
}

void EitHandler::demuxDettached()
{
    AbstractDvbPsiHandler::d->attached = false;
}
