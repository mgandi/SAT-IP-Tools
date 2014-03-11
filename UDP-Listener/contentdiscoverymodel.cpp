#include "contentdiscoverymodel.h"
#include "pathandler.h"
#include "pmthandler.h"
#include "sdthandler.h"
#include "demuxhandler.h"

#include <QMap>
#include <QtDebug>

class ContentDiscoveryModelPrivate
{
public:
    ContentDiscoveryModelPrivate(ContentDiscoveryModel *parent);

    ContentDiscoveryModel *p;
    PatHandler patHandler;
    QMap<quint16, PmtHandler *> pmtHandlers;
    DemuxHandler demuxHandler;
    QList<AbstractDvbPsiHandler *> handlers;
    SdtHandler *sdtHandler;
};

ContentDiscoveryModelPrivate::ContentDiscoveryModelPrivate(ContentDiscoveryModel *parent):
    p(parent),
    patHandler(parent),
    demuxHandler(parent),
    sdtHandler(0)
{
    patHandler.attach();
    QObject::connect(&patHandler, SIGNAL(dataChanged()),
                     p, SLOT(patFound()));

    demuxHandler.attach();
    QObject::connect(&demuxHandler, SIGNAL(newHandler(AbstractDvbPsiHandler*)),
                     p, SLOT(newHandler(AbstractDvbPsiHandler*)));

    handlers += &patHandler;
    handlers += &demuxHandler;
}


ContentDiscoveryModel::ContentDiscoveryModel(RtpSocket *socket, QObject *parent) :
    QAbstractItemModel(parent),
    d(new ContentDiscoveryModelPrivate(this))
{
    connect(socket, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
            this, SLOT(packetsAvailable(quint32,QList<QByteArray>&)));
}

ContentDiscoveryModel::~ContentDiscoveryModel()
{
    delete d;
}


QModelIndex ContentDiscoveryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, (void *)&d->patHandler);
    } else {
        QObject *o = static_cast<QObject *>(parent.internalPointer());
        PatHandler *patHandler = qobject_cast<PatHandler *>(o);
        PmtHandler *pmtHandler = qobject_cast<PmtHandler *>(o);
        if (patHandler) {
            quint16 number = d->patHandler.programs().at(row).number;
            return createIndex(row, column, (void *)d->pmtHandlers.value(number));
        } else if (pmtHandler) {
            PmtElementaryStream *elementaryStream = pmtHandler->elementaryStreams().at(row);
            return createIndex(row, column, elementaryStream);
        } else {
            return QModelIndex();
        }
    }
}

QModelIndex ContentDiscoveryModel::parent(const QModelIndex &child) const
{
    QObject *o = static_cast<QObject *>(child.internalPointer());
    PatHandler *patHandler = qobject_cast<PatHandler *>(o);
    PmtHandler *pmtHandler = qobject_cast<PmtHandler *>(o);
    PmtElementaryStream *elementaryStream = qobject_cast<PmtElementaryStream *>(o);

    if (patHandler) {
        return QModelIndex();
    } else if (pmtHandler) {
        return createIndex(0, 0, (void *)&d->patHandler);
    } else if (elementaryStream) {
        foreach (PmtHandler *handler, d->pmtHandlers) {
            if (handler->contains(elementaryStream)) {
                pmtHandler = handler;
                break;
            }
        }
        return createIndex(d->pmtHandlers.values().indexOf(pmtHandler), 0, (void *)(pmtHandler));
    } else {
        return QModelIndex();
    }
}


int ContentDiscoveryModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 1;
    } else {
        QObject *o = static_cast<QObject *>(parent.internalPointer());
        PatHandler *patHandler = qobject_cast<PatHandler *>(o);
        PmtHandler *pmtHandler = qobject_cast<PmtHandler *>(o);
        if (patHandler) {
            int nbPrograms = patHandler->programs().count();
            return nbPrograms;
        } else if (pmtHandler) {
            return pmtHandler->elementaryStreams().count();
        } else {
            return 0;
        }
    }
}

int ContentDiscoveryModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant ContentDiscoveryModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        QObject *o = static_cast<QObject *>(index.internalPointer());
        PatHandler *patHandler = qobject_cast<PatHandler *>(o);
        PmtHandler *pmtHandler = qobject_cast<PmtHandler *>(o);
        PmtElementaryStream *elementaryStream = qobject_cast<PmtElementaryStream *>(o);

        if (patHandler) {
            switch (index.column()) {
            case 0:
                return QString("TS ID: %1").arg(patHandler->tsId());
            case 1:
                return QString("Version: %1").arg(patHandler->version());
            }
        } else if (pmtHandler) {
            switch (index.column()) {
            case 0: {
                QString str = QString("Program number: %1").arg(pmtHandler->programNumber());
                if (d->sdtHandler && d->sdtHandler->services()->contains(pmtHandler->programNumber())) {
                    str = d->sdtHandler->services()->value(pmtHandler->programNumber())->serviceName;
                }
                return str;
            }
            case 1:
                return QString("PMT PID: 0x%1 (%2)").arg(d->patHandler.programs().at(index.row()).pid, -4, 16).arg(d->patHandler.programs().at(index.row()).pid);
            case 2:
                return QString("Version: %1").arg(pmtHandler->version());
            case 3:
                return QString("PCR PID: 0x%1 (%2)").arg(pmtHandler->pcrPid(), -4, 16).arg(pmtHandler->pcrPid());
            }
        } else if (elementaryStream) {
            switch (index.column()) {
            case 0:
                return QString("Type: %1").arg(elementaryStream->type);
            case 1:
                return QString("ES PID: 0x%1 (%2)").arg(elementaryStream->pid, -4, 16).arg(elementaryStream->pid);
            }
        } else {
            return QVariant();
        }
        break;
    }

    return QVariant();
}

void ContentDiscoveryModel::packetsAvailable(quint32, QList<QByteArray> &packets)
{
    foreach (AbstractDvbPsiHandler *handler, d->handlers) {
        handler->push(packets);
    }
}

void ContentDiscoveryModel::patFound()
{
    PatHandler *handler = qobject_cast<PatHandler *>(sender());
    foreach (PatProgram program, handler->programs()) {
        PmtHandler *pmtHandler = new PmtHandler(program.number, this);
        d->pmtHandlers.insert(program.number, pmtHandler);
        d->handlers += pmtHandler;
        pmtHandler->attach();
        connect(pmtHandler, SIGNAL(dataChanged()),
                this, SLOT(pmtFound()));
    }
    beginInsertRows(createIndex(0, 0, (void *)&d->patHandler), 0, d->patHandler.programs().count() - 1);
    endInsertRows();
    emit dataChanged(createIndex(0, 0, (void *)&d->patHandler), createIndex(0, 1, (void *)&d->patHandler), QVector<int>() << Qt::DisplayRole);
}

void ContentDiscoveryModel::pmtFound()
{
    PmtHandler *handler = qobject_cast<PmtHandler *>(sender());
    int index = d->patHandler.indexOf(handler->programNumber());
    beginInsertRows(createIndex(index, 0, (void *)handler), 0, handler->elementaryStreams().count() - 1);
    endInsertRows();
    emit dataChanged(createIndex(index, 0, (void *)handler), createIndex(index, 3, (void *)handler), QVector<int>() << Qt::DisplayRole);
}

void ContentDiscoveryModel::sdtFound()
{
    emit dataChanged(createIndex(0, 0, (void *)d->pmtHandlers.first()), createIndex(d->pmtHandlers.count() - 1, 3, (void *)d->pmtHandlers.last()), QVector<int>() << Qt::DisplayRole);
}

void ContentDiscoveryModel::newHandler(AbstractDvbPsiHandler *handler)
{
    SdtHandler *sdtHandler = qobject_cast<SdtHandler *>(handler);
    d->handlers += handler;

    if (sdtHandler) {
        d->sdtHandler = sdtHandler;
        connect(sdtHandler, SIGNAL(dataChanged()),
                this, SLOT(sdtFound()));
        return;
    }
}
