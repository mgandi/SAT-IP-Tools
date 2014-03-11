#ifndef SCANWIDGET_H
#define SCANWIDGET_H

#include <QWidget>

namespace Ui {
class ScanWidget;
}

class GatewayDevice;
class ScanWidgetPrivate;
class ScanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScanWidget(GatewayDevice *device, QWidget *parent = 0);
    ~ScanWidget();

private slots:
    void on_cleanButton_clicked();
    void on_settingsButton_clicked();
    void on_saveRTSP_clicked();
    void on_saveHTTP_clicked();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_stepForwardButton_clicked();
    void on_stepBackwardButton_clicked();

    void showProgressStatus();
    void hideProgressStatus();

    void scanStarted();
    void scanProgress(int percent, int frequency, int endFrequency);
    void scanDone();
    void scanStopped();
    void scanPaused();

private:
    Ui::ScanWidget *ui;
    ScanWidgetPrivate *d;
};

#endif // SCANWIDGET_H
