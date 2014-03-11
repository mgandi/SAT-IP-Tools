#include "serverinformationwidget.h"
#include "ui_serverinformationwidget.h"
#include "gatewaydevice.h"

class ServerInformationWidgetPrivate
{
public:
    GatewayDevice *device;
};

ServerInformationWidget::ServerInformationWidget(GatewayDevice *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerInformationWidget),
    d(new ServerInformationWidgetPrivate)
{
    d->device = device;
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

    // Display FEs
    ui->nbDvbtFe->setText(QString("%1").arg(d->device->nbFEForSystem(DVBT)));
    ui->nbDvbsFe->setText(QString("%1").arg(d->device->nbFEForSystem(DVBS)));
    ui->nbDvbt2Fe->setText(QString("%1").arg(d->device->nbFEForSystem(DVBT2)));
    ui->nbDvbs2Fe->setText(QString("%1").arg(d->device->nbFEForSystem(DVBS2)));
}

ServerInformationWidget::~ServerInformationWidget()
{
    delete d;
    delete ui;
}
