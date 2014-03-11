#include "udplistener.h"
#include "ui_udplistener.h"
#include "contentdiscoverymodel.h"

#include <QScrollBar>
#include <QDir>
#include <QtDebug>

UdpListener::UdpListener(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UdpListener),
    nbBytesRx(0),
    totalMaxBitRate(0),
    model(this)
{
    ui->setupUi(this);
    ui->udpTableView->setModel(&model);
    connect(ui->udpTableView, SIGNAL(removeRow(int)),
            this, SLOT(removeRow(int)));

//    on_bind_clicked();
    startTimer(1000);
}

UdpListener::~UdpListener()
{
    foreach (RtpSocket *socket, sockets) {
        delete socket;
    }
    delete ui;
}

void UdpListener::timerEvent(QTimerEvent *)
{
    double mbps = 0, averageMbps = 0;
    foreach (RtpSocket *socket, sockets) {
        mbps += socket->mbps();
        averageMbps += socket->averageMbps();
    }
    totalMaxBitRate = mbps > totalMaxBitRate ? mbps : totalMaxBitRate;
    ui->instantBitRate->setText(QString("Total instant bit rate: %1 Mbps").arg(mbps));
    ui->maxBitRate->setText(QString("Total max bit rate: %1 Mbps").arg(totalMaxBitRate));
    ui->averageBitRate->setText(QString("Total average bit rate: %1 Mbps").arg(averageMbps));
}

void UdpListener::on_bind_clicked()
{
    QString address = ui->ipAddress->text();
    int port = ui->port->value();

    write(QString("\n=====================================================================\n"));
    write(QString("=====================================================================\n\n"));

    RtpSocket *socket = new RtpSocket(QHostAddress(address), port, false, this);
    sockets += socket;
    model.addSocket(socket);
    ui->udpTableView->resizeRowsToContents();

    QTreeView *discoveryView = new QTreeView;
    discoveryViews.insert(socket, discoveryView);

//    ContentDiscoveryModel *model = new ContentDiscoveryModel(socket, this);
//    discoveryView->setModel(model);

    ui->stackedDiscoveryView->addWidget(discoveryView);
}

void UdpListener::write(const QString &str)
{
    qDebug() << qPrintable(str);

}

void UdpListener::removeRow(int row)
{
    RtpSocket *socket = model.socketAt(row);
    model.removeSocket(socket);
    sockets.removeAll(socket);
    QTreeView *discoveryView = discoveryViews.value(socket);
    delete discoveryView;
    discoveryViews.remove(socket);
    delete socket;
}

void UdpListener::on_udpTableView_clicked(const QModelIndex &index)
{
    ui->stackedDiscoveryView->setCurrentIndex(index.row());
}
