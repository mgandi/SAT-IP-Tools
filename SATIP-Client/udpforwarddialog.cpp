#include "udpforwarddialog.h"
#include "ui_udpforwarddialog.h"

#include <QRegExpValidator>

UdpForwardDialog::UdpForwardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UdpForwardDialog)
{
    ui->setupUi(this);

    QString octet = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    ui->address->setValidator(new QRegExpValidator(
                                  QRegExp("^" + octet + "\\." + octet + "\\." + octet + "\\." + octet + "$"), this));
}

UdpForwardDialog::~UdpForwardDialog()
{
    delete ui;
}

QHostAddress UdpForwardDialog::address() const
{
    return QHostAddress(ui->address->text());
}

quint16 UdpForwardDialog::port() const
{
    return (quint16) ui->port->value();
}
