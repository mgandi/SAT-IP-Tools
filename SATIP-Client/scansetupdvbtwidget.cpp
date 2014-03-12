#include "scansetupdvbtwidget.h"
#include "ui_scansetupdvbtwidget.h"
#include "scanprocedure.h"
#include "scanproceduredvbt.h"

class ScanSetupDvbtWidgetPrivate
{
public:
    int nbSteps;
    ScanProcedureDvbt *procedure;
};

ScanSetupDvbtWidget::ScanSetupDvbtWidget(ScanProcedureDvbt *procedure, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScanSetupDvbtWidget),
    d(new ScanSetupDvbtWidgetPrivate)
{
    ui->setupUi(this);
    d->procedure = procedure;

    connect(ui->startFrequency, SIGNAL(valueChanged(int)),
            procedure, SLOT(setStartFrequency(int)));
    connect(ui->endFrequency, SIGNAL(valueChanged(int)),
            procedure, SLOT(setEndFrequency(int)));
    connect(ui->scanStep, SIGNAL(valueChanged(int)),
            procedure, SLOT(setScanStep(int)));
    connect(ui->bandwidth, SIGNAL(valueChanged(int)),
            procedure, SLOT(setBandwidth(int)));

    init();
}

ScanSetupDvbtWidget::~ScanSetupDvbtWidget()
{
    delete d;
    delete ui;
}


void ScanSetupDvbtWidget::init()
{
    ui->startFrequency->setValue(d->procedure->startFrequency());
    ui->endFrequency->setValue(d->procedure->endFrequency());
    ui->scanStep->setValue(d->procedure->scanStep());
    ui->bandwidth->setValue(d->procedure->bandwidth());
    ui->commonScanSetup->setProcedure(d->procedure);
    updateNbSteps();
}


void ScanSetupDvbtWidget::on_startFrequency_editingFinished()
{
    // Let's make sure the start frequency is not greater than the end frequency
    if (ui->startFrequency->value() > ui->endFrequency->value()) {
        ui->startFrequency->setValue(ui->endFrequency->value());
    }
    updateNbSteps();
}

void ScanSetupDvbtWidget::on_endFrequency_editingFinished()
{
    // Let's make sure the end frequency is not less than the start frequency
    if (ui->endFrequency->value() < ui->startFrequency->value()) {
        ui->endFrequency->setValue(ui->startFrequency->value());
    }
    updateNbSteps();
}

void ScanSetupDvbtWidget::on_bandwidth_editingFinished()
{
}

void ScanSetupDvbtWidget::on_scanStep_editingFinished()
{
    updateNbSteps();
}

void ScanSetupDvbtWidget::updateNbSteps()
{
    ui->numberOfSteps->setText(QString("%1").arg(d->procedure->nbSteps()));
}
