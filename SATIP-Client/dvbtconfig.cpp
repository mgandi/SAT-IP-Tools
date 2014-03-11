#include "dvbtconfig.h"
#include "ui_dvbtconfig.h"

DvbTConfig::DvbTConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DvbTConfig)
{
    ui->setupUi(this);
}

DvbTConfig::~DvbTConfig()
{
    delete ui;
}


quint8 DvbTConfig::fe() const
{
    return ui->fe->value();
}

quint32 DvbTConfig::freq() const
{
    return ui->freq->value();
}

quint8 DvbTConfig::bw() const
{
    return ui->bw->value();
}

QString DvbTConfig::pids() const
{
    return ui->pids->text();
}
