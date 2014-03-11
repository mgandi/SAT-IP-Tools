#include "programtesterwidget.h"
#include "ui_programtesterwidget.h"

ProgramTesterWidget::ProgramTesterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgramTesterWidget)
{
    ui->setupUi(this);
}

ProgramTesterWidget::~ProgramTesterWidget()
{
    delete ui;
}
