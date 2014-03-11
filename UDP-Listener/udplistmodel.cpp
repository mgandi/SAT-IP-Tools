#include "udplistmodel.h"

#include <QUdpSocket>

UdpListModel::UdpListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

QVariant UdpListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            switch (section) {
            case 0:
                return "Stream";
                break;
            case 1:
                return "Instant bit rate";
                break;
            case 2:
                return "Avergae bit rate";
                break;
            case 3:
                return "Max bit rate";
                break;
            }
            break;
        }
    }

    return QVariant();
}

int UdpListModel::columnCount(const QModelIndex &) const
{
    return 4;
}

int UdpListModel::rowCount(const QModelIndex &) const
{
    return count();
}

QVariant UdpListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        RtpSocket *socket = at(index.row());
        switch (index.column()) {
        case 0:
            return QString("rtp://%1:%2").arg(socket->address()).arg(socket->port());
        case 1:
            return QString("%1 Mbps").arg(socket->mbps());
        case 2:
            return QString("%1 Mbps").arg(socket->averageMbps());
        case 3:
            return QString("%1 Mbps").arg(socket->maxMbps());
        }
        break;
    }

    return QVariant();
}

bool UdpListModel::addSocket(RtpSocket *socket)
{
    append(socket);
    connect(socket, SIGNAL(mbpsUpdated(double)),
            this, SLOT(mbpsUpdated(double)));
    return insertRow(count() - 1, QModelIndex());
}

bool UdpListModel::removeSocket(RtpSocket *socket)
{
    int row = indexOf(socket);
    if (row == -1)
        return false;
    removeAt(row);
    return removeRow(row, QModelIndex());
}


bool UdpListModel::insertRow(int row, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + 1);
    endInsertRows();
    return true;
}

bool UdpListModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + 1);
    endRemoveRows();
    return true;
}

RtpSocket *UdpListModel::socketAt(int index)
{
    return at(index);
}

void UdpListModel::mbpsUpdated(const double)
{
    RtpSocket *socket = qobject_cast<RtpSocket *>(sender());
    int row = indexOf(socket);
    emit dataChanged(index(row, 1), index(row, 3), QVector<int>() << Qt::DisplayRole);
}
