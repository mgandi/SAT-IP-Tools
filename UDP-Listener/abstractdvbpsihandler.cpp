#include "abstractdvbpsihandler.h"
#include "abstractdvbpsihandler_p.h"

#include <QCoreApplication>
#include <QEvent>
#include <QtDebug>

static void message(dvbpsi_t *, const dvbpsi_msg_level_t level, const char* msg)
{
    Q_UNUSED(level);
    Q_UNUSED(msg);
//    switch (level) {
//    case DVBPSI_MSG_ERROR:
//        qCritical() << msg;
//        break;
//    case DVBPSI_MSG_WARN:
//        qWarning() << msg;
//        break;
//    case DVBPSI_MSG_DEBUG:
//        qDebug() << msg;
//        break;
//    default:
//        return;
//    }
}

class DettachEvent: public QEvent
{
public:
    DettachEvent():
        QEvent(QEvent::User) {}
};

AbstractDvbPsiHandler::AbstractDvbPsiHandler(QObject *parent, MessageLevel messageLevel) :
    QObject(parent),
    d(new AbstractDvbPsiHandlerPrivate(messageLevel))
{
    dvbpsi_msg_level_t dvbpsimsglvl = DVBPSI_MSG_ERROR;

    switch (messageLevel) {
    case MSG_NONE:
        dvbpsimsglvl = DVBPSI_MSG_NONE;
        break;
    case MSG_ERROR:
        dvbpsimsglvl = DVBPSI_MSG_ERROR;
        break;
    case MSG_WARN:
        dvbpsimsglvl = DVBPSI_MSG_WARN;
        break;
    case MSG_DEBUG:
        dvbpsimsglvl = DVBPSI_MSG_DEBUG;
        break;
    }

    if (!d->dvbpsi)
        Q_ASSERT((d->dvbpsi = dvbpsi_new(&message, dvbpsimsglvl)));
}

AbstractDvbPsiHandler::AbstractDvbPsiHandler(const AbstractDvbPsiHandler &other) :
    QObject(other.parent()),
    d(new AbstractDvbPsiHandlerPrivate(other.abstractDvbPsiHandlerPrivate()->msgLvl, other.abstractDvbPsiHandlerPrivate()->dvbpsi))
{
}

AbstractDvbPsiHandler::~AbstractDvbPsiHandler()
{
    delete d;
}

bool AbstractDvbPsiHandler::attached() const
{
    return d->attached;
}


void AbstractDvbPsiHandler::push(QByteArray &packet)
{
    dvbpsi_packet_push(d->dvbpsi, (uint8_t *)packet.data());
}

void AbstractDvbPsiHandler::push(QList<QByteArray> &packets)
{
    foreach (QByteArray packet, packets) {
        push(packet);
    }
}

void AbstractDvbPsiHandler::operator <<(QByteArray &packet)
{
    push(packet);
}

void AbstractDvbPsiHandler::operator <<(QList<QByteArray> &packets)
{
    push(packets);
}

void AbstractDvbPsiHandler::dettachLater()
{
    QCoreApplication::postEvent(this, new DettachEvent());
}

AbstractDvbPsiHandlerPrivate *AbstractDvbPsiHandler::abstractDvbPsiHandlerPrivate() const
{
    return d;
}

bool AbstractDvbPsiHandler::event(QEvent *e)
{
    DettachEvent *dettachEvent = dynamic_cast<DettachEvent *>(e);
    if (dettachEvent) {
        dettach();
        e->accept();
        return true;
    } else {
        return QObject::event(e);
    }
}
