#ifndef GATEWAYWIDGET_H
#define GATEWAYWIDGET_H

#include <QWidget>

namespace Ui {
class GatewayWidget;
}

class ProgramModel;
class GatewayDevice;
class RTSPSession;
class GatewayWidgetPrivate;
class GatewayWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit GatewayWidget(GatewayDevice *device, QWidget *parent = 0);
    ~GatewayWidget();

    GatewayDevice *device() const;
    
private slots:
    void addSession();
    void zapChannel(RTSPSession *);
    void removeSession(RTSPSession *);

private:
    Ui::GatewayWidget *ui;
    GatewayWidgetPrivate *d;
};

#endif // GATEWAYWIDGET_H
