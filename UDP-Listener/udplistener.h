#ifndef WIDGET_H
#define WIDGET_H

#include "udplistmodel.h"
#include "rtpsocket.h"

#include <QWidget>
#include <QTreeView>

namespace Ui {
class UdpListener;
}

class UdpListener : public QWidget
{
    Q_OBJECT
    
public:
    explicit UdpListener(QWidget *parent = 0);
    ~UdpListener();

protected:
    void timerEvent(QTimerEvent *);
    
private slots:
    void on_bind_clicked();
    void write(const QString &str);
    void removeRow(int row);

    void on_udpTableView_clicked(const QModelIndex &index);

private:
    Ui::UdpListener *ui;
    QList<RtpSocket *> sockets;
    QHash<RtpSocket *, QTreeView *> discoveryViews;
    quint64 nbBytesRx;
    double totalMaxBitRate;
    UdpListModel model;
};

#endif // WIDGET_H
