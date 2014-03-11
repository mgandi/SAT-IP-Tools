#ifndef CONTENTDISCOVERYMODEL_H
#define CONTENTDISCOVERYMODEL_H

#include "rtpsocket.h"

#include <QAbstractItemModel>

class AbstractDvbPsiHandler;
class ContentDiscoveryModelPrivate;
class ContentDiscoveryModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ContentDiscoveryModel(RtpSocket *socket, QObject *parent = 0);
    ~ContentDiscoveryModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

protected slots:
    void packetsAvailable(quint32, QList<QByteArray> &packets);
    void patFound();
    void pmtFound();
    void sdtFound();
    void newHandler(AbstractDvbPsiHandler *handler);

private:
    ContentDiscoveryModelPrivate *d;
};

#endif // CONTENTDISCOVERYMODEL_H
