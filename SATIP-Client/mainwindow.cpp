#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gatewaydevice.h"
#include "gatewaywidget.h"
#include "ssdpclient.h"
#include "severselectionwidget.h"
#include "objectpool.h"
#include "settings.h"
#include "programrecorderwidget.h"
#include "programtesterwidget.h"

#include <QNetworkInterface>
#include <QTimer>
#include <QScrollBar>
#include <QDockWidget>
#include <QMenu>

QPlainTextEdit *plainTextEdit = 0;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg: {
        plainTextEdit->insertPlainText(msg);
        plainTextEdit->insertPlainText("\n");
        QScrollBar *sb = plainTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
        break;
    }
    case QtWarningMsg: {
        plainTextEdit->insertPlainText("Warning : ");
        plainTextEdit->insertPlainText(msg);
        plainTextEdit->insertPlainText("\n");
        QScrollBar *sb = plainTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
        break;
    }
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    default:
        break;
    }
}

class MainWindowPrivate
{
public:
    MainWindowPrivate(MainWindow *parent) :
        p(parent)
    {}

    MainWindow *p;

    QList<QHostAddress> validAddresses, retry;
    QList<SSDPClient *> ssdpClients;
    GatewayDevice *currentDevice;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d(new MainWindowPrivate(this))
{
    ui->setupUi(this);

    // Server selection dock widget
    ui->serverSelectionDockWidget->setVisible(false);
    ui->menuView->addAction(ui->serverSelectionDockWidget->toggleViewAction());

    // UDP listener dock widget
    tabifyDockWidget(ui->logDockWidget, ui->udpListenerDockWidget);
    ui->udpListenerDockWidget->setVisible(false);
    ui->menuView->addAction(ui->udpListenerDockWidget->toggleViewAction());

    // Program recorder dock widget
    tabifyDockWidget(ui->udpListenerDockWidget, ui->programRecorderDockWidget);
    //ui->programRecorderDockWidget->setVisible(false);
    ui->menuView->addAction(ui->programRecorderDockWidget->toggleViewAction());

    // Program tester dock widget
    tabifyDockWidget(ui->udpListenerDockWidget, ui->programTesterDockWidget);
    ui->programTesterDockWidget->setVisible(false);
    ui->menuView->addAction(ui->programTesterDockWidget->toggleViewAction());

    // Set global plain text edit pointer
    plainTextEdit = ui->plainTextEdit;
    qInstallMessageHandler(myMessageOutput);

    // Create list of valid IP addresses
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, interfaces) {
        QNetworkInterface::InterfaceFlags flags = interface.flags();
        if ((flags & QNetworkInterface::IsUp) &&
            (flags & QNetworkInterface::IsRunning) &&
            (flags & QNetworkInterface::CanBroadcast) &&
            (flags & QNetworkInterface::CanMulticast) &&
            !(flags & QNetworkInterface::IsLoopBack) &&
            !(flags & QNetworkInterface::IsPointToPoint) &&
            interface.isValid()) {
                ui->plainTextEdit->insertPlainText(interface.humanReadableName().append("\n"));
                QList<QNetworkAddressEntry> addressEntries = interface.addressEntries();
                foreach (QNetworkAddressEntry addressEntry, addressEntries) {
                    if (addressEntry.ip().protocol() != QAbstractSocket::IPv4Protocol)
                        continue;
                    d->validAddresses += addressEntry.ip();
                    ui->plainTextEdit->insertPlainText(QString("- ").append(addressEntry.ip().toString()).append("\n"));
                }
        }
    }

    // For each valid network interfaces create an SSDP client
    foreach (QHostAddress address, d->validAddresses) {
        SSDPClient *ssdpClient = new SSDPClient(address.toString(), this);
        connect(ssdpClient, SIGNAL(newGatewayDeviceFound(GatewayDevice*)),
                this, SLOT(addDevice(GatewayDevice*)));
        connect(ssdpClient, SIGNAL(gatewayDeviceLeft(GatewayDevice*)),
                this, SLOT(removeDevice(GatewayDevice*)));
        d->ssdpClients += ssdpClient;
    }

    // Start single shot timer to check for devices
    QTimer::singleShot(2000, this, SLOT(checkForDevices()));
}

MainWindow::~MainWindow()
{
    qInstallMessageHandler(0);
    delete d;
    delete ui;
}


void MainWindow::addDevice(GatewayDevice *device)
{
    ObjectPool::instance()->addObject(device);
}

void MainWindow::removeDevice(GatewayDevice *device)
{
    ObjectPool::instance()->removeObject(device);
}

void MainWindow::currentDeviceChanged(GatewayDevice *device)
{
    Settings settings;

    GatewayWidget *widget = qobject_cast<GatewayWidget *>(centralWidget());
    if (widget)
        delete widget;

    d->currentDevice = device;

    if (d->currentDevice) {
        settings.setLastUsedDevice(d->currentDevice->uid());
        setCentralWidget(new GatewayWidget(d->currentDevice, this));
    }
}

void MainWindow::checkForDevices()
{
    Settings settings;
    QString lastUsedDevice = settings.lastUsedDevice();

    if (!lastUsedDevice.isEmpty()) {
        foreach (GatewayDevice *device, ui->serverSelectionWidget->devices()) {
            if (device->uid() == lastUsedDevice) {
                d->currentDevice = device;
                break;
            }
        }
    }

    if (!d->currentDevice) {
        d->currentDevice = ui->serverSelectionWidget->currentDevice();
    }

    connect(ui->serverSelectionWidget, SIGNAL(currentDeviceChanged(GatewayDevice*)),
            this, SLOT(currentDeviceChanged(GatewayDevice*)));

    if (d->currentDevice) {
        ui->serverSelectionWidget->setCurrentDevice(d->currentDevice);
    }
}
