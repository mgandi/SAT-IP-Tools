#include "gatewaywidget.h"
#include "ui_gatewaywidget.h"
#include "gatewaydevice.h"
#include "sessionmodel.h"
#include "addsessiondialog.h"
#include "programmodel.h"
#include "scanwidget.h"
#include "sessionwidget.h"
#include "epgwidget.h"
#include "channellistwidget.h"
#include "zapchanneldialog.h"

#include <QtNetwork/QNetworkReply>
#include <QDir>
#include <QDialog>
#include <QMessageBox>

class GatewayWidgetPrivate
{
public:
    GatewayWidgetPrivate(GatewayDevice *de, GatewayWidget *parent) :
        p(parent),
        device(de),
//        channelListWidget(device, p),
        scanWidget(device, p),
        epgWidget(device, p),
        sessionWidget(device->sessionModel(), p)
    {}

    GatewayWidget *p;

    GatewayDevice *device;
//    ChannelListWidget channelListWidget;
    ScanWidget scanWidget;
    EpgWidget epgWidget;
    SessionWidget sessionWidget;
};

GatewayWidget::GatewayWidget(GatewayDevice *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GatewayWidget),
    d(new GatewayWidgetPrivate(device, this))
{
    ui->setupUi(this);

    // Set information section with information of device
    ui->friendlyName->setText(device->friendlyName());
    ui->manufacturer->setText(device->manufacturer());
    ui->modelDescription->setText(device->modelDescription());
    ui->deviceType->setText(device->deviceType());
    ui->modelNumber->setText(device->modelNumber());
    ui->modelName->setText(device->modelName());
    ui->host->setText(device->host());
    ui->serialNumber->setText(device->serialNumber());

    // Add the different tools to the central tab widget
//    ui->tabWidget->addTab(&d->channelListWidget, "Channel List");
    ui->tabWidget->addTab(&d->epgWidget, "EPG");
    ui->tabWidget->addTab(&d->scanWidget, "Scan");
    ui->tabWidget->addTab(&d->sessionWidget, "Sessions");
    connect(&d->sessionWidget, SIGNAL(addSession()),
            this, SLOT(addSession()));
    connect(&d->sessionWidget, SIGNAL(removeSession(RTSPSession*)),
            this, SLOT(removeSession(RTSPSession*)));
    connect(&d->sessionWidget, SIGNAL(zapChannel(RTSPSession*)),
            this, SLOT(zapChannel(RTSPSession*)));
    connect(d->device, SIGNAL(statisticsAvailable(quint64,quint64)),
            &d->sessionWidget, SLOT(statisticsAvailable(quint64,quint64)));
}

GatewayWidget::~GatewayWidget()
{
    delete d;
    delete ui;
}


GatewayDevice *GatewayWidget::device() const
{
    return d->device;
}


void GatewayWidget::addSession()
{
    AddSessionDialog dialog(this, d->device->capabilities());
    if (dialog.exec() == QDialog::Accepted) {
        d->device->addSession(dialog.requestParams());
    }
}

void GatewayWidget::zapChannel(RTSPSession *session)
{
    ZapChannelDialog dialog(d->device, this);

    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "zap to" << dialog.selectedChannel()->serviceName();
        if (!d->device->updateSession(session, dialog.selectedChannel(), dialog.zapPeriod()))
            QMessageBox::question(this, "Session",
                                  "Issue to zap channel",
                                  QMessageBox::Ok);
    }
}
void GatewayWidget::removeSession(RTSPSession *session)
{
    d->device->removeSession(session);
}
