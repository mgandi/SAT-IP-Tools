#include "programrecorderwidget.h"
#include "ui_programrecorderwidget.h"

ProgramRecorderWidget::ProgramRecorderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgramRecorderWidget)
{
    ui->setupUi(this);
}

ProgramRecorderWidget::~ProgramRecorderWidget()
{
    delete ui;
}
