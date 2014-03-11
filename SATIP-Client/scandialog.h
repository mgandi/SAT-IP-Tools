#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QDialog>

namespace Ui {
class ScanDialog;
}

class ScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScanDialog(QWidget *parent = 0);
    ~ScanDialog();

private:
    Ui::ScanDialog *ui;
};

#endif // SCANDIALOG_H
