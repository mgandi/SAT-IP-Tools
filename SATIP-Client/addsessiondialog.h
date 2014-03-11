#ifndef ADDSESSIONDIALOG_H
#define ADDSESSIONDIALOG_H

#include "gatewaydevice.h"
#include "program.h"

#include <QDialog>

namespace Ui {
class AddSessionDialog;
}

class AddSessionDialog : public QDialog
{
    Q_OBJECT
    
public:
    AddSessionDialog(QWidget *parent, Systems systems);
    AddSessionDialog(QWidget *parent);
    ~AddSessionDialog();

    RequestParams requestParams() const;
    
signals:
    void msysUpdated(int index);

private slots:
    void on_msys_currentIndexChanged(const QString &arg1);

private:
    Ui::AddSessionDialog *ui;
};

#endif // ADDSESSIONDIALOG_H
