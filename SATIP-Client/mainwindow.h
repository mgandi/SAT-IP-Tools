#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>

namespace Ui {
class MainWindow;
}

class GatewayDevice;
class MainWindowPrivate;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void addDevice(GatewayDevice *device);
    void removeDevice(GatewayDevice *device);
    void currentDeviceChanged(GatewayDevice *device);
    void checkForDevices();

private:
    Ui::MainWindow *ui;
    MainWindowPrivate *d;
};

#endif // MAINWINDOW_H
