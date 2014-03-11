#include "zapchanneldialog.h"
#include "ui_zapchanneldialog.h"
#include "programmodel.h"
#include "program.h"

#include <QDebug>

ZapChannelDialog::ZapChannelDialog(GatewayDevice *device, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZapChannelDialog),
    selectedProgram(NULL)
{
    ui->setupUi(this);
    ui->channelList->setModel(new ProgramModel(device, ProgramModel::DVBT /* unused parameter */, false, this));
}

ZapChannelDialog::~ZapChannelDialog()
{
    delete ui;
}


void ZapChannelDialog::on_channelList_doubleClicked(const QModelIndex &index)
{
    ProgramModel *m = qobject_cast<ProgramModel *>(ui->channelList->model());
    selectedProgram = m->programs().at(index.row());
    accept();
}

Program * ZapChannelDialog::selectedChannel()
{
    return selectedProgram;
}

int ZapChannelDialog::zapPeriod()
{
    return ui->zapPeriod->value();
}
