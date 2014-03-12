#include "scansetupcommonwidget.h"
#include "ui_scansetupcommonwidget.h"
#include "scanprocedure.h"

class ScanSetupCommonWidgetPrivate
{
public:
    ScanProcedure *procedure;
};

ScanSetupCommonWidget::ScanSetupCommonWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScanSetupCommonWidget),
    d(new ScanSetupCommonWidgetPrivate)
{
    ui->setupUi(this);
}

ScanSetupCommonWidget::~ScanSetupCommonWidget()
{
    delete d;
    delete ui;
}

void ScanSetupCommonWidget::setProcedure(ScanProcedure *procedure)
{
    d->procedure = procedure;

    connect(ui->lockTimeout, SIGNAL(valueChanged(int)),
            procedure, SLOT(setLockTimeout(int)));
    connect(ui->patTimeout, SIGNAL(valueChanged(int)),
            procedure, SLOT(setPatTimeout(int)));

    init();
}


void ScanSetupCommonWidget::init()
{
    ui->lockTimeout->setValue(d->procedure->lockTimeout());
    ui->patTimeout->setValue(d->procedure->patTimeout());
    d->procedure->isAuto() ? ui->manualStepping->setCheckState(Qt::Unchecked) : ui->manualStepping->setCheckState(Qt::Checked);
    d->procedure->isInitFrequencyScanWithDefaultDvbPids() ? ui->defaultDvbPIDs->setCheckState(Qt::Checked) : ui->defaultDvbPIDs->setCheckState(Qt::Unchecked);
}


void ScanSetupCommonWidget::on_manualStepping_stateChanged(int checkState)
{
    bool isAuto;
    switch (checkState) {
    case Qt::Checked:
        isAuto = false;
        break;
    case Qt::Unchecked:
        isAuto = true;
        break;
    case Qt::PartiallyChecked:
        isAuto = false;
        break;
    }

    d->procedure->setAuto(isAuto);
}

void ScanSetupCommonWidget::on_defaultDvbPIDs_stateChanged(int checkState)
{
    bool b;
    switch (checkState) {
    case Qt::Checked:
        b = true;
        break;
    case Qt::Unchecked:
        b = false;
        break;
    case Qt::PartiallyChecked:
        b = false;
        break;
    }

    d->procedure->setInitFrequencyScanWithDefaultDvbPids(b);
}
