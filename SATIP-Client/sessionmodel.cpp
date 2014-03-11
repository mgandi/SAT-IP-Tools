#include "sessionmodel.h"
#include "rtpsocket.h"
#include "rtcpsocket.h"

#include <QHash>
#include <QIcon>

class SessionModelPrivate
{
public:
    QList<RTSPSession *> sessions;
    QHash<RtpSocket *, RTSPSession *> rtpSockets;
    QHash<RtcpSocket *, RTSPSession *> rtcpSockets;
};

SessionModel::SessionModel(QObject *parent) :
    QAbstractListModel(parent),
    d(new SessionModelPrivate)
{
}

SessionModel::~SessionModel()
{
    delete d;
}


bool SessionModel::addSession(RTSPSession *session)
{
    int index = d->sessions.count();
    d->sessions += session;
    d->rtpSockets.insert(session->rtpSocket(), session);
    d->rtcpSockets.insert(session->rtcpSocket(), session);
    connect(session->rtpSocket(), SIGNAL(mbpsUpdated(double)),
            this, SLOT(mbpsUpdated(double)));
    connect(session->rtcpSocket(), SIGNAL(reportAvailable(RTCPReport)),
            this, SLOT(reportAvailable(RTCPReport)));
    connect(session, SIGNAL(sessionUpdated()),
            this, SLOT(updateRow()));
    return insertRows(index, 1, QModelIndex());
}

bool SessionModel::removeSession(RTSPSession *session)
{
    int row = d->sessions.indexOf(session);
    if (row == -1)
        return false;
    return removeRows(row, 1, QModelIndex());
}


RTSPSession *SessionModel::sessionAtIndex(int row) const
{
    RTSPSession *session = d->sessions.at(row);
    return session;
}


QModelIndex SessionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < d->sessions.count())
            return createIndex(row, column, (void *)sessionAtIndex(row));
    }

    return QModelIndex();
}

int SessionModel::columnCount(const QModelIndex &) const
{
    return 19;
}

int SessionModel::rowCount(const QModelIndex &) const
{
    return d->sessions.count();
}

QVariant SessionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        switch (orientation) {
        case Qt::Horizontal:
            switch (section) {
            case 0:
                return "Session ID";
            case 1:
                return "Stream ID";
            case 2:
                return "Session type";
            case 3:
                return "Source";
            case 4:
                return "Destination";
            case 5:
                return "RTP port";
            case 6:
                return "RTCP port";
            case 7:
                return "Instant bit rate";
            case 8:
                return "Average bit rate";
            case 9:
                return "Max bit rate";
            case 10:
                return "Front end ID";
            case 11:
                return "Locked";
            case 12:
                return "Signal level";
            case 13:
                return "Signal quality";
            case 14:
                return "Frequency";
            case 15:
                return "SSRC";
            case 16:
                return "Sent packet count";
            case 17:
                return "Received packet count";
            case 18:
                return "Nb lost packets";
            }
            break;
        default:
            break;
        }
        break;
    }

    return QVariant();
}

QVariant SessionModel::data(const QModelIndex &index, int role) const
{
    RTSPSession *session = d->sessions.at(index.row());

    switch (role) {
    case Qt::DisplayRole: {
        switch (index.column()) {
        case 0:
            return session->session();
        case 1:
            return session->streamid();
        case 2:
            return session->unicast() ? "unicast" : "multicast";
        case 3:
            return session->source();
        case 4:
            return session->destination();
        case 5:
            return session->rtpPort();
        case 6:
            return session->rtcpPort();
        case 7:
            return QString("%1 Mbps").arg(session->rtpSocket()->mbps());
        case 8:
            return QString("%1 Mbps").arg(session->rtpSocket()->averageMbps());
        case 9:
            return QString("%1 Mbps").arg(session->rtpSocket()->maxMbps());
        case 10:
            return session->rtcpSocket()->lastReport()->feID();
        case 12:
            return session->rtcpSocket()->lastReport()->signalLevel();
        case 14:
            return session->rtcpSocket()->lastReport()->frequency();
        case 15:
            return session->rtcpSocket()->lastReport()->ssrc();
        case 16:
            return session->rtcpSocket()->lastReport()->packetCount();
        case 17:
            return session->rtpSocket()->packetCount();
        case 18:
            return session->rtpSocket()->nbLostPackets();
        }
        break;
    case Qt::ForegroundRole:
        if (parent() != session->parent())
            return QColor(153,153,153);
        break;
    }

    case Qt::DecorationRole: {
        switch (index.column()) {
        case 11:
            return session->rtcpSocket()->lastReport()->locked() ? QIcon(":/images/LOCK") : QIcon(":/images/UNLOCK");
        case 13:
            switch (session->rtcpSocket()->lastReport()->signalQuality()) {
            case 0:
                return QIcon(":/images/0");
            case 1:
            case 2:
            case 3:
                return QIcon(":/images/1");
            case 4:
            case 5:
            case 6:
                return QIcon(":/images/2");
            case 7:
            case 8:
            case 9:
                return QIcon(":/images/3");
            case 10:
            case 11:
            case 12:
                return QIcon(":/images/4");
            case 13:
            case 14:
            case 15:
                return QIcon(":/images/5");
            }
        }
        break;
    }
    }

    return QVariant();
}


bool SessionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1)
        return false;

    int first = row, last = row + count - 1;
    beginInsertRows(parent, first, last);
    endInsertRows();
    return true;
}

bool SessionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1)
        return false;

    int first = row, last = row + count - 1;
    beginRemoveRows(parent, first, last);
    for (int i = row; i < (row + count); i++) {
        RTSPSession *session = d->sessions.at(i);
        d->rtpSockets.remove(session->rtpSocket());
        d->rtcpSockets.remove(session->rtcpSocket());
        d->sessions.removeAt(row);
    }
    endRemoveRows();
    return true;
}


void SessionModel::mbpsUpdated(const double)
{
    RtpSocket *socket = qobject_cast<RtpSocket *>(sender());
    RTSPSession *session = d->rtpSockets.value(socket);
    int row = d->sessions.indexOf(session);
    emit dataChanged(index(row, 7, QModelIndex()), index(row, 9, QModelIndex()), QVector<int>() << Qt::DisplayRole);
}

void SessionModel::reportAvailable(const RTCPReport &)
{
    RtcpSocket *socket = qobject_cast<RtcpSocket *>(sender());
    RTSPSession *session = d->rtcpSockets.value(socket);
    int row = d->sessions.indexOf(session);
    emit dataChanged(index(row, 10, QModelIndex()), index(row, 18, QModelIndex()), QVector<int>() << Qt::DisplayRole << Qt::DecorationRole);
}

void SessionModel::updateRow()
{
    RTSPSession *session = qobject_cast<RTSPSession *>(sender());
    int row = d->sessions.indexOf(session);
    emit dataChanged(index(row, 7, QModelIndex()), index(row, 18, QModelIndex()), QVector<int>() << Qt::DisplayRole);
}
