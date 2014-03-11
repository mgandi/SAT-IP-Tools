#include "channellistwidget.h"
#include "ui_channellistwidget.h"
#include "programmodel.h"

#include <QDebug>

ChannelListWidget::ChannelListWidget(GatewayDevice *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelListWidget)
{
    ui->setupUi(this);
    ui->channelListView->setGatewayDevice(device);
    ui->channelListView->setModel(new ProgramModel(device, ProgramModel::DVBT, false, this));
}

ChannelListWidget::~ChannelListWidget()
{
    delete ui;
}
