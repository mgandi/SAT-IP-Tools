#ifndef SERVERINFORMATIONWIDGET_H
#define SERVERINFORMATIONWIDGET_H

#include <QWidget>

namespace Ui {
class ServerInformationWidget;
}

class GatewayDevice;
class ServerInformationWidgetPrivate;
class ServerInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerInformationWidget(GatewayDevice *device, QWidget *parent = 0);
    ~ServerInformationWidget();

private:
    Ui::ServerInformationWidget *ui;
    ServerInformationWidgetPrivate *d;
};

#endif // SERVERINFORMATIONWIDGET_H
