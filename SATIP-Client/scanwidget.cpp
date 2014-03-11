#include "scanwidget.h"
#include "ui_scanwidget.h"
#include "scanprocedure.h"
#include "scanproceduredvbt.h"
#include "scanproceduredvbs.h"
#include "gatewaywidget.h"
#include "programmodel.h"
#include "scansetupdvbtwidget.h"
#include "scansetupdvbswidget.h"
#include "epgcollector.h"
#include "gatewaydevice.h"
#include "addsessiondialog.h"

#include <QFileDialog>

class ScanWidgetPrivate
{
public:
    ScanWidgetPrivate(GatewayDevice *de, ScanWidget *parent) :
        p(parent),
        device(de),
        procedure(NULL),
        scanSetupDvbtWidget(NULL),
        scanSetupDvbsWidget(NULL)
    {
    }

    ScanWidget *p;
    GatewayDevice *device;
    ScanProcedure *procedure;
    ScanSetupDvbtWidget *scanSetupDvbtWidget;
    ScanSetupDvbsWidget *scanSetupDvbsWidget;
    ProgramModel *model;
};

ScanWidget::ScanWidget(GatewayDevice *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScanWidget),
    d(new ScanWidgetPrivate(device, this))
{
    ui->setupUi(this);
    ui->channelList->setGatewayDevice(d->device);
    ui->channelList->setModel((d->model = new ProgramModel(device, ProgramModel::DVBT, true, this)));

    if (d->device->capabilities() & (DVBT | DVBT2)) {
        d->procedure = (ScanProcedure *) new ScanProcedureDvbt(d->device, this);
        d->scanSetupDvbtWidget = new ScanSetupDvbtWidget((ScanProcedureDvbt *)d->procedure);
    }
    else {
        d->procedure = (ScanProcedure *) new ScanProcedureDvbs(d->device, this);
        d->scanSetupDvbsWidget = new ScanSetupDvbsWidget((ScanProcedureDvbs *)d->procedure);
    }

    connect(d->procedure, SIGNAL(scanStarted()),
            this, SLOT(scanStarted()));
    connect(d->procedure, SIGNAL(scanProgress(int,int,int)),
            this, SLOT(scanProgress(int,int,int)));
    connect(d->procedure, SIGNAL(scanDone()),
            this, SLOT(scanDone()));
    connect(d->procedure, SIGNAL(scanStopped()),
            this, SLOT(scanStopped()));
    connect(d->procedure, SIGNAL(scanPaused()),
            this, SLOT(scanPaused()));

    ui->progressBar->setVisible(false);
    ui->currentFrequency->setVisible(false);
    ui->settingsButton->setVisible(true);
}

ScanWidget::~ScanWidget()
{
    delete d->model;
    delete d->procedure;
    if (d->scanSetupDvbtWidget)
        delete d->scanSetupDvbtWidget;
    if (d->scanSetupDvbsWidget)
        delete d->scanSetupDvbsWidget;
    delete d;
    delete ui;
}


void ScanWidget::on_cleanButton_clicked()
{
    switch (d->procedure->scanStatus()) {
    case ScanProcedure::Idle:
    case ScanProcedure::Completed:
        d->device->clearPrograms();
        d->device->savePrograms();
        break;
    case ScanProcedure::Started:
    case ScanProcedure::Paused:
    default:
        break;
    }
}

void ScanWidget::on_settingsButton_clicked()
{
    if (d->device->capabilities() & (DVBT | DVBT2)) {
        d->scanSetupDvbtWidget->show();
    } else {
        d->scanSetupDvbsWidget->show();
    }
}


void ScanWidget::on_saveRTSP_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as RTSP M3U playlist", QDir::currentPath(), QString("*.m3u"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << qPrintable(file.errorString());
    } else {
        QTextStream stream(&file);
        stream << d->model->toM3U(Program::RTSP);
        file.close();
    }
}

void ScanWidget::on_saveHTTP_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as HTTP M3U playlist", QDir::currentPath(), QString("*.m3u"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << qPrintable(file.errorString());
    } else {
        QTextStream stream(&file);
        stream << d->model->toM3U(Program::HTTP);
        file.close();
    }
}


void ScanWidget::on_startButton_clicked()
{
    switch (d->procedure->scanStatus()) {
    case ScanProcedure::Idle:
    case ScanProcedure::Completed:
        d->procedure->start();
        break;
    case ScanProcedure::Started:
        d->procedure->pause();
        break;
    case ScanProcedure::Paused:
        d->procedure->resume();
        break;
    default:
        break;
    }
}

void ScanWidget::on_stopButton_clicked()
{
    switch (d->procedure->scanStatus()) {
    case ScanProcedure::Paused:
    case ScanProcedure::Started:
        d->procedure->stop();
        break;
    case ScanProcedure::Idle:
    case ScanProcedure::Completed:
    default:
        break;
    }
}

void ScanWidget::on_stepForwardButton_clicked()
{
    switch (d->procedure->scanStatus()) {
    case ScanProcedure::Started:
        d->procedure->stepForward();
        break;
    case ScanProcedure::Paused:
    case ScanProcedure::Idle:
    case ScanProcedure::Completed:
    default:
        break;
    }
}

void ScanWidget::on_stepBackwardButton_clicked()
{
    switch (d->procedure->scanStatus()) {
    case ScanProcedure::Started:
        d->procedure->stepBackward();
        break;
    case ScanProcedure::Paused:
    case ScanProcedure::Idle:
    case ScanProcedure::Completed:
    default:
        break;
    }
}


void ScanWidget::showProgressStatus()
{
    ui->progressBar->setVisible(true);
    ui->currentFrequency->setVisible(true);
}

void ScanWidget::hideProgressStatus()
{
    ui->progressBar->setVisible(false);
    ui->currentFrequency->setVisible(false);
}


void ScanWidget::scanStarted()
{
    ui->startButton->setIcon(QIcon(":/images/PAUSE"));
    ui->stopButton->setEnabled(true);
    ui->stepForwardButton->setEnabled(true);
    ui->stepBackwardButton->setEnabled(true);
    ui->cleanButton->setEnabled(false);
    showProgressStatus();
}

void ScanWidget::scanProgress(int percent, int frequency, int endFrequency)
{
    ui->progressBar->setValue(percent);
    ui->currentFrequency->setText(QString("%1/%2 MHz").arg(frequency).arg(endFrequency));
}

void ScanWidget::scanDone()
{
    d->device->savePrograms();
    ui->startButton->setIcon(QIcon(":/images/PLAY"));
    ui->stopButton->setEnabled(false);
    ui->stepForwardButton->setEnabled(false);
    ui->stepBackwardButton->setEnabled(false);
    ui->cleanButton->setEnabled(true);
    hideProgressStatus();
}

void ScanWidget::scanStopped()
{
    d->device->savePrograms();
    ui->startButton->setIcon(QIcon(":/images/PLAY"));
    ui->stopButton->setEnabled(false);
    ui->stepForwardButton->setEnabled(false);
    ui->stepBackwardButton->setEnabled(false);
    ui->cleanButton->setEnabled(true);
    hideProgressStatus();
}

void ScanWidget::scanPaused()
{
    ui->startButton->setIcon(QIcon(":/images/PLAY"));
    ui->stepForwardButton->setEnabled(false);
    ui->stepBackwardButton->setEnabled(false);
}
