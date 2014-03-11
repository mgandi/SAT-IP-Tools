#include "sessionwidget.h"
#include "ui_sessionwidget.h"
#include "sessionmodel.h"

#include <QDebug>

SessionWidget::SessionWidget(SessionModel *model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionWidget)
{
    ui->setupUi(this);
    ui->sessionView->setModel(model);
    connect(ui->sessionView, SIGNAL(addSession()),
            this, SIGNAL(addSession()));
    connect(ui->sessionView, SIGNAL(removeSession(RTSPSession*)),
            this, SIGNAL(removeSession(RTSPSession*)));
    connect(ui->sessionView, SIGNAL(zapChannel(RTSPSession*)),
            this, SIGNAL(zapChannel(RTSPSession*)));
}

SessionWidget::~SessionWidget()
{
    delete ui;
}

void SessionWidget::statisticsAvailable(quint64 dataRate, quint64 nbPacketReceived)
{
    ui->dataRate->setText(QString("Data rate: %1 - # packets received: %2").arg(dataRate).arg(nbPacketReceived));
}
