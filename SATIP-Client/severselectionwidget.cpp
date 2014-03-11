#include "severselectionwidget.h"
#include "ui_severselectionwidget.h"
#include "objectpool.h"
#include "gatewaydevice.h"
#include "serverinformationwidget.h"

#include <QHash>
#include <QUuid>

class SeverSelectionWidgetPrivate
{
public:
    QList<QPair<QString, QString> > uidsAndHost;
    QHash<GatewayDevice *, ServerInformationWidget *> devices;
};

SeverSelectionWidget::SeverSelectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SeverSelectionWidget),
    d(new SeverSelectionWidgetPrivate)
{
    ui->setupUi(this);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(currentChanged(int)));

    connect(ObjectPool::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    connect(ObjectPool::instance(), SIGNAL(objectRemoved(QObject*)),
            this, SLOT(objectRemoved(QObject*)));
}

SeverSelectionWidget::~SeverSelectionWidget()
{
    delete d;
    delete ui;
}


GatewayDevice *SeverSelectionWidget::currentDevice()
{
    QWidget *widget = ui->tabWidget->currentWidget();
    ServerInformationWidget *server = qobject_cast<ServerInformationWidget *>(widget);
    return d->devices.key(server, 0);
}

void SeverSelectionWidget::setCurrentDevice(GatewayDevice *device)
{
    ServerInformationWidget *widget = d->devices.value(device);
    if (widget) {
        if (ui->tabWidget->currentWidget() != widget) {
            ui->tabWidget->setCurrentWidget(widget);
        } else {
            emit currentDeviceChanged(device);
        }
    }
}

QList<GatewayDevice *> SeverSelectionWidget::devices()
{
    return d->devices.keys();
}


void SeverSelectionWidget::currentChanged(int index)
{
    QWidget *widget = ui->tabWidget->widget(index);
    ServerInformationWidget *server = qobject_cast<ServerInformationWidget *>(widget);
    GatewayDevice *device = d->devices.key(server, 0);
    if (device)
        emit currentDeviceChanged(device);
}

void SeverSelectionWidget::objectAdded(QObject *object)
{
    GatewayDevice *device = qobject_cast<GatewayDevice *>(object);
    if (device) {
        QPair<QString, QString> uidAndHost = QPair<QString, QString>(device->uid(), device->host());
        if(!d->devices.contains(device) && !d->uidsAndHost.contains(uidAndHost)) {
            ServerInformationWidget *widget = new ServerInformationWidget(device, this);
            d->devices.insert(device, widget);
            d->uidsAndHost += uidAndHost;
            ui->tabWidget->addTab(widget, device->friendlyName());
        }
    }
}

void SeverSelectionWidget::objectRemoved(QObject *object)
{
    GatewayDevice *device = qobject_cast<GatewayDevice *>(object);
    if (device && d->devices.contains(device)) {
        ServerInformationWidget *widget = d->devices.value(device);
        int index = ui->tabWidget->indexOf(widget);
        ui->tabWidget->removeTab(index);
        delete widget;
        d->uidsAndHost.removeAll(QPair<QString, QString>(device->uid(), device->host()));
        d->devices.remove(device);
    }
}
