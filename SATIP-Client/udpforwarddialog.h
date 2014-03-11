#ifndef UDPFORWARDDIALOG_H
#define UDPFORWARDDIALOG_H

#include <QDialog>
#include <QHostAddress>

namespace Ui {
class UdpForwardDialog;
}

class UdpForwardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UdpForwardDialog(QWidget *parent = 0);
    ~UdpForwardDialog();

    QHostAddress address() const;
    quint16 port() const;

private:
    Ui::UdpForwardDialog *ui;
};

#endif // UDPFORWARDDIALOG_H
