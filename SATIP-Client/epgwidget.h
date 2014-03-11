#ifndef EPGWIDGET_H
#define EPGWIDGET_H

#include <QWidget>

namespace Ui {
class EpgWidget;
}

class Program;
class Event;
class GatewayDevice;
class EpgWidgetPrivate;
class EpgWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EpgWidget(GatewayDevice *device, QWidget *parent = 0);
    ~EpgWidget();

    void recordEvent(Event *event);

protected slots:
    void addProgram(Program *program);
    void removeProgram(Program *program);
    void addEvent(Event *event);
    void removeEvent(Event * event);
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);
    void resizeChannelScene();
    void resizeEpgScene();
    void currentProgramChanged(Program *newProgram, Program *oldProgram);
    void recordingDone();

private slots:
    void on_collectEpg_clicked(bool checked);
    void on_testButton_clicked();

private:
    Ui::EpgWidget *ui;
    EpgWidgetPrivate *d;
};

#endif // EPGWIDGET_H
