#ifndef UDPLISTMODEL_H
#define UDPLISTMODEL_H

#include "rtpsocket.h"

#include <QAbstractTableModel>

class QUdpSocket;
class UdpListModel : public QAbstractTableModel, private QList<RtpSocket *>
{
    Q_OBJECT
public:
    explicit UdpListModel(QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool addSocket(RtpSocket *socket);
    bool removeSocket(RtpSocket *socket);

    bool insertRow(int row, const QModelIndex &parent);
    bool removeRow(int row, const QModelIndex &parent);

    RtpSocket *socketAt(int index);

protected slots:
    void mbpsUpdated(const double mbps);
};

#endif // UDPLISTMODEL_H
