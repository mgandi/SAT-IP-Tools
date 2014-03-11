#ifndef ZAPCHANNELDIALOG_H
#define ZAPCHANNELDIALOG_H

#include "program.h"
#include <QDialog>

namespace Ui {
class ZapChannelDialog;
}

class GatewayDevice;
class ZapChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZapChannelDialog(GatewayDevice *device, QWidget *parent);
    ~ZapChannelDialog();

    Program *selectedChannel();
    int zapPeriod();

private slots:
    void on_channelList_doubleClicked(const QModelIndex &index);

private:
    Ui::ZapChannelDialog *ui;
    Program *selectedProgram;
};

#endif // ZAPCHANNELDIALOG_H
