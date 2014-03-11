#include "demuxhandler.h"
#include "abstractdvbpsihandler_p.h"
#include "sdthandler.h"
#include "nithandler.h"
#include "eithandler.h"
#include "demux.h"

#include <QtDebug>
#include <QMap>
#include <QDateTime>

class DemuxHandlerPrivate
{
public:
    DemuxHandlerPrivate(DemuxHandler *demuxHandler) :
        p(demuxHandler) {}

    DemuxHandler *p;
    QMap<quint8, AbstractDvbPsiHandler *> handlers;
};



static void callback(dvbpsi_t *, uint8_t tableId, uint16_t extension, void * data)
{
    DemuxHandlerPrivate *dhp = (DemuxHandlerPrivate *)data;

    // If already a handler then just ignore
    if (dhp->handlers.contains(tableId))
        return;

    switch (tableId) {
    case 0x40: { // Network Information Table for actual network
//        qDebug() << "=========================================";
//        qDebug() << qPrintable(QDateTime::currentDateTime().toString("hh:mm:ss:zzz")) << " - NIT " << qPrintable(QString("(0x%1)").arg(tableId, 2, 16, QLatin1Char('0')));
        NitHandler *handler = new NitHandler((NitHandler::WhichTS)tableId, extension, *dhp->p, dhp->p);
        handler->attach();
        dhp->handlers.insert(tableId, handler);
        emit dhp->p->newHandler(handler);
        break;
    }
    case 0x42: { // Service Description Table for actual network
        SdtHandler *handler = new SdtHandler((SdtHandler::WhichTS)tableId, extension, *dhp->p, dhp->p);
        handler->attach();
        dhp->handlers.insert(tableId, handler);
        emit dhp->p->newHandler(handler);
        return;
    }
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
    case 0x6F:{ // Event Information Table
//        qDebug() << "=========================================";
//        qDebug() << "EIT handler for" << qPrintable(QString("(0x%1)").arg(tableId, 2, 16, QLatin1Char('0')));
        EitHandler *handler = new EitHandler(tableId, extension, *dhp->p, dhp->p);
        handler->attach();
        dhp->handlers.insert(tableId, handler);
        emit dhp->p->newHandler(handler);
        return;
    }
    default:
        break;
    }
}



DemuxHandler::DemuxHandler(QObject *parent) :
    AbstractDvbPsiHandler(parent),
    d(new DemuxHandlerPrivate(this))
{
}

DemuxHandler::~DemuxHandler()
{
    delete d;
}


void DemuxHandler::attach()
{
//    qDebug() << "=========================================";
//    qDebug() << "Demux handler attached";
//    Q_ASSERT(dvbpsi_AttachDemux(abstractDvbPsiHandlerPrivate()->dvbpsi, callback, (void *)d));
    AbstractDvbPsiHandler::d->attached = true;
}

void DemuxHandler::clear()
{
    d->handlers.clear();
}

void DemuxHandler::dettach()
{
//    dvbpsi_DetachDemux(AbstractDvbPsiHandler::d->dvbpsi);
    AbstractDvbPsiHandler::d->attached = false;
    emit dettached();
}
