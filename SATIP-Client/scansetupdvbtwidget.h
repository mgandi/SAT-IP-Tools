#ifndef SCANSETUPDVBTWIDGET_H
#define SCANSETUPDVBTWIDGET_H

#include <QWidget>

namespace Ui {
class ScanSetupDvbtWidget;
}

class ScanProcedureDvbt;
class ScanSetupDvbtWidgetPrivate;
class ScanSetupDvbtWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScanSetupDvbtWidget(ScanProcedureDvbt *procedure, QWidget *parent = 0);
    ~ScanSetupDvbtWidget();

protected:
    void init();

private slots:
    void on_startFrequency_editingFinished();
    void on_endFrequency_editingFinished();
    void on_bandwidth_editingFinished();
    void on_scanStep_editingFinished();
    void on_lockTimeout_editingFinished();
    void on_patTimeout_editingFinished();
    void updateNbSteps();
    void on_manualStepping_stateChanged(int);

private:
    Ui::ScanSetupDvbtWidget *ui;
    ScanSetupDvbtWidgetPrivate *d;
};

#endif // SCANSETUPDVBTWIDGET_H
