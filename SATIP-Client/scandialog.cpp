#include "scandialog.h"
#include "ui_scandialog.h"

ScanDialog::ScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScanDialog)
{
    ui->setupUi(this);
}

ScanDialog::~ScanDialog()
{
    delete ui;
}
