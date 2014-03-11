#ifndef CHANNELLISTWIDGET_H
#define CHANNELLISTWIDGET_H

#include <QWidget>

namespace Ui {
class ChannelListWidget;
}

class GatewayDevice;
class ChannelListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelListWidget(GatewayDevice *device, QWidget *parent = 0);
    ~ChannelListWidget();

private:
    Ui::ChannelListWidget *ui;
};

#endif // CHANNELLISTWIDGET_H
