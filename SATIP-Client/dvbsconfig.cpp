#include "dvbsconfig.h"
#include "ui_dvbsconfig.h"

DvbSConfig::DvbSConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DvbSConfig)
{
    ui->setupUi(this);
}

DvbSConfig::~DvbSConfig()
{
    delete ui;
}

void DvbSConfig::hideFe()
{
    ui->fe->setVisible(false);
    ui->labelFe->setVisible(false);
}

quint8 DvbSConfig::src() const
{
    return ui->src->value();
}

quint8 DvbSConfig::fe() const
{
    return ui->fe->value();
}

quint32 DvbSConfig::freq() const
{
    return ui->freq->value();
}

bool DvbSConfig::vpol() const
{
    return (ui->pol->currentText() == "Vertical" ? true : false);
}

Mtype DvbSConfig::mtype() const
{
    return (ui->mtype->currentText() == "QPSK" ? M_QPSK : M_8PSK);
}

quint32 DvbSConfig::sr() const
{
    return ui->sr->value();
}

Fec DvbSConfig::fec() const
{
    return static_cast<Fec>(ui->fec->currentIndex());
}

QString DvbSConfig::pids() const
{
    return ui->pids->text();
}


