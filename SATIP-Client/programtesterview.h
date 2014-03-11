#ifndef PROGRAMTESTERVIEW_H
#define PROGRAMTESTERVIEW_H

#include <QTreeView>

class ProgramTesterViewPrivate;
class ProgramTesterView : public QTreeView
{
    Q_OBJECT
public:
    explicit ProgramTesterView(QWidget *parent = 0);
    ~ProgramTesterView();

protected slots:
    void stopTesting();
    void clearError();

private:
    ProgramTesterViewPrivate *d;
};

#endif // PROGRAMTESTERVIEW_H
