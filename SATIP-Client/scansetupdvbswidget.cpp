#include "scansetupdvbswidget.h"
#include "ui_scansetupdvbswidget.h"
#include "scanprocedure.h"
#include "scanproceduredvbs.h"

class ScanSetupDvbsWidgetPrivate
{
public:
    int nbSteps;
    ScanProcedureDvbs *procedure;
};

ScanSetupDvbsWidget::ScanSetupDvbsWidget(ScanProcedureDvbs *procedure, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScanSetupDvbsWidget),
    d(new ScanSetupDvbsWidgetPrivate)
{
    ui->setupUi(this);
    d->procedure = procedure;

    connect(ui->startFrequency, SIGNAL(valueChanged(int)),
            procedure, SLOT(setStartFrequency(int)));
    connect(ui->endFrequency, SIGNAL(valueChanged(int)),
            procedure, SLOT(setEndFrequency(int)));
    connect(ui->scanStep, SIGNAL(valueChanged(int)),
            procedure, SLOT(setScanStep(int)));
    connect(ui->symbolRate, SIGNAL(valueChanged(int)),
            procedure, SLOT(setSymbolRate(int)));
    connect(ui->modulation, SIGNAL(currentIndexChanged(QString)),
            procedure, SLOT(setModulation(QString)));
    connect(ui->polarity, SIGNAL(currentIndexChanged(QString)),
            procedure, SLOT(setPolarity(QString)));
    connect(ui->system, SIGNAL(currentIndexChanged(QString)),
            procedure, SLOT(setSystem(QString)));
    connect(ui->satPos, SIGNAL(valueChanged(int)),
            procedure, SLOT(setSatPos(int)));
    connect(ui->lockTimeout, SIGNAL(valueChanged(int)),
            procedure, SLOT(setLockTimeout(int)));
    connect(ui->patTimeout, SIGNAL(valueChanged(int)),
            procedure, SLOT(setPatTimeout(int)));

    init();
}

ScanSetupDvbsWidget::~ScanSetupDvbsWidget()
{
    delete d;
    delete ui;
}


void ScanSetupDvbsWidget::init()
{
    ui->startFrequency->setValue(d->procedure->startFrequency());
    ui->endFrequency->setValue(d->procedure->endFrequency());
    ui->scanStep->setValue(d->procedure->scanStep());
    ui->modulation->setCurrentText(d->procedure->modulation());
    ui->polarity->setCurrentText(d->procedure->polarity());
    ui->system->setCurrentText(d->procedure->system());
    ui->symbolRate->setValue(d->procedure->symbolRate());
    ui->satPos->setValue(d->procedure->satPos());
    ui->lockTimeout->setValue(d->procedure->lockTimeout());
    ui->patTimeout->setValue(d->procedure->patTimeout());
    d->procedure->isAuto() ? ui->manualStepping->setCheckState(Qt::Unchecked) : ui->manualStepping->setCheckState(Qt::Checked);
}


void ScanSetupDvbsWidget::on_startFrequency_editingFinished()
{
    // Let's make sure the start frequency is not greater than the end frequency
    if (ui->startFrequency->value() > ui->endFrequency->value()) {
        ui->startFrequency->setValue(ui->endFrequency->value());
    }
    updateNbSteps();
}

void ScanSetupDvbsWidget::on_endFrequency_editingFinished()
{
    // Let's make sure the end frequency is not less than the start frequency
    if (ui->endFrequency->value() < ui->startFrequency->value()) {
        ui->endFrequency->setValue(ui->startFrequency->value());
    }
    updateNbSteps();
}

void ScanSetupDvbsWidget::on_scanStep_editingFinished()
{
    updateNbSteps();
}

void ScanSetupDvbsWidget::on_lockTimeout_editingFinished()
{
}

void ScanSetupDvbsWidget::on_patTimeout_editingFinished()
{
}

void ScanSetupDvbsWidget::updateNbSteps()
{
    ui->numberOfSteps->setText(QString("%1").arg(d->procedure->nbSteps()));
}

void ScanSetupDvbsWidget::on_manualStepping_stateChanged(int checkState)
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
