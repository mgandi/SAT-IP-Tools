#ifndef PROGRAMRECORDERWIDGET_H
#define PROGRAMRECORDERWIDGET_H

#include <QWidget>

namespace Ui {
class ProgramRecorderWidget;
}

class ProgramRecorderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgramRecorderWidget(QWidget *parent = 0);
    ~ProgramRecorderWidget();

private:
    Ui::ProgramRecorderWidget *ui;
};

#endif // PROGRAMRECORDERWIDGET_H
