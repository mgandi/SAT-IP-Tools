#ifndef SCANSETUPDVBSWIDGET_H
#define SCANSETUPDVBSWIDGET_H

#include <QWidget>

namespace Ui {
class ScanSetupDvbsWidget;
}

class ScanProcedureDvbs;
class ScanSetupDvbsWidgetPrivate;
class ScanSetupDvbsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScanSetupDvbsWidget(ScanProcedureDvbs *procedure, QWidget *parent = 0);
    ~ScanSetupDvbsWidget();

protected:
    void init();

private slots:
    void on_startFrequency_editingFinished();
    void on_endFrequency_editingFinished();
    void on_scanStep_editingFinished();
    void on_lockTimeout_editingFinished();
    void on_patTimeout_editingFinished();
    void updateNbSteps();
    void on_manualStepping_stateChanged(int);

private:
    Ui::ScanSetupDvbsWidget *ui;
    ScanSetupDvbsWidgetPrivate *d;
};

#endif // SCANSETUPDVBSWIDGET_H
