#ifndef SCANSETUPCOMMONWIDGET_H
#define SCANSETUPCOMMONWIDGET_H

#include <QWidget>

class ScanProcedure;
class ScanSetupCommonWidgetPrivate;

namespace Ui {
class ScanSetupCommonWidget;
}

class ScanSetupCommonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScanSetupCommonWidget(QWidget *parent = 0);
    ~ScanSetupCommonWidget();

    void setProcedure(ScanProcedure *procedure);

protected:
    void init();

protected slots:
    void on_manualStepping_stateChanged(int checkState);
    void on_defaultDvbPIDs_stateChanged(int checkState);

private:
    Ui::ScanSetupCommonWidget *ui;
    ScanSetupCommonWidgetPrivate *d;
};

#endif // SCANSETUPCOMMONWIDGET_H
