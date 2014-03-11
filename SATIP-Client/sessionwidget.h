#ifndef SESSIONWIDGET_H
#define SESSIONWIDGET_H

#include <QWidget>

namespace Ui {
class SessionWidget;
}

class RTSPSession;
class SessionModel;
class SessionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SessionWidget(SessionModel *model, QWidget *parent = 0);
    ~SessionWidget();

public slots:
    void statisticsAvailable(quint64 dataRate, quint64 nbPacketReceived);

signals:
    void addSession();
    void zapChannel(RTSPSession *);
    void removeSession(RTSPSession *);

private:
    Ui::SessionWidget *ui;
};

#endif // SESSIONWIDGET_H
