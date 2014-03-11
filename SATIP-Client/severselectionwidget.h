#ifndef SEVERSELECTIONWIDGET_H
#define SEVERSELECTIONWIDGET_H

#include <QWidget>

namespace Ui {
class SeverSelectionWidget;
}

class GatewayDevice;
class SeverSelectionWidgetPrivate;
class SeverSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SeverSelectionWidget(QWidget *parent = 0);
    ~SeverSelectionWidget();

    GatewayDevice *currentDevice();
    void setCurrentDevice(GatewayDevice *device);
    QList<GatewayDevice *> devices();

protected slots:
    void currentChanged(int);
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

signals:
    void currentDeviceChanged(GatewayDevice *device);

private:
    Ui::SeverSelectionWidget *ui;
    SeverSelectionWidgetPrivate *d;
};

#endif // SEVERSELECTIONWIDGET_H
