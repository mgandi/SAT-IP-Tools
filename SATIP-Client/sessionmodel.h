#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include <rtspsession.h>

#include <QAbstractListModel>

class GatewayDevice;
class SessionModelPrivate;
class SessionModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SessionModel(QObject *parent = 0);
    ~SessionModel();

    bool addSession(RTSPSession *session);
    bool removeSession(RTSPSession *session);
    RTSPSession *sessionAtIndex(int row) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

protected slots:
    void mbpsUpdated(const double);
    void reportAvailable(const RTCPReport &);
    void updateRow();

private:
    SessionModelPrivate *d;
};

#endif // SESSIONMODEL_H
