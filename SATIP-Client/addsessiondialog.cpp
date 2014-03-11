#include "addsessiondialog.h"
#include "ui_addsessiondialog.h"


AddSessionDialog::AddSessionDialog(QWidget *parent, Systems systems) :
    QDialog(parent),
    ui(new Ui::AddSessionDialog)
{
    ui->setupUi(this);
    ui->msys->clear();
    if (systems & DVBT)
        ui->msys->insertItem(0, "DVB-T");
    if (systems & DVBT2)
        ui->msys->insertItem(0, "DVB-T2");
    if (systems & DVBS || systems & DVBS2)
        ui->msys->insertItem(0, "DVB-S");
    if (systems & DVBS2)
        ui->msys->insertItem(0, "DVB-S2");

    connect( this, SIGNAL(msysUpdated(int)),
             ui->stackedWidget, SLOT(setCurrentIndex(int)) );

    emit msysUpdated((systems & DVBT || systems & DVBT2) ? 0 : 1);
}

/* for DVB-S only */
AddSessionDialog::AddSessionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSessionDialog)
{
    ui->setupUi(this);
    ui->msys->clear();
    ui->msys->insertItem(0, "DVB-S");
    ui->msys->insertItem(0, "DVB-S2");
    ui->unicast->setVisible(false);
}

AddSessionDialog::~AddSessionDialog()
{
    delete ui;
}



RequestParams AddSessionDialog::requestParams() const
{
    RequestParams requestParams;

    if (ui->msys->currentText() == "DVB-S") {
        requestParams.msys = DVBS;
    } else if (ui->msys->currentText() == "DVB-T") {
        requestParams.msys = DVBT;
    } else if (ui->msys->currentText() == "DVB-T2") {
        requestParams.msys = DVBT2;
    } else if (ui->msys->currentText() == "DVB-S2") {
        requestParams.msys = DVBS2;
    }
    requestParams.rtp = true;
    requestParams.unicast = ui->unicast->isChecked();
    switch (requestParams.msys) {
    case DVBS2:
    case DVBS:
        requestParams.fe = ui->dvbs->fe();
        requestParams.src = ui->dvbs->src();
        requestParams.freq = ui->dvbs->freq();
        requestParams.vpol = ui->dvbs->vpol();
        requestParams.mtype = ui->dvbs->mtype();
        requestParams.sr = ui->dvbs->sr();
        requestParams.src = ui->dvbs->src();
        requestParams.fec = ui->dvbs->fec();
        requestParams.pids = ui->dvbs->pids();
        break;
    case DVBT:
        requestParams.fe = ui->dvbt->fe();
        requestParams.freq = ui->dvbt->freq();
        requestParams.bw = ui->dvbt->bw();
        requestParams.pids = ui->dvbt->pids();
        break;
    }

    return requestParams;
}


void AddSessionDialog::on_msys_currentIndexChanged(const QString &arg)
{
    emit msysUpdated((arg == "DVB-T" || arg == "DVB-T2") ? 0 : 1);
}
