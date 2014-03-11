#ifndef PROGRAMTESTERWIDGET_H
#define PROGRAMTESTERWIDGET_H

#include <QWidget>

namespace Ui {
class ProgramTesterWidget;
}

class ProgramTesterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgramTesterWidget(QWidget *parent = 0);
    ~ProgramTesterWidget();

private:
    Ui::ProgramTesterWidget *ui;
};

#endif // PROGRAMTESTERWIDGET_H
