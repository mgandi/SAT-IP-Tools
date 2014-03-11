#ifndef PROGRAMRECORDERVIEW_H
#define PROGRAMRECORDERVIEW_H

#include <QTreeView>

class ProgramRecorderViewPrivate;
class ProgramRecorderView : public QTreeView
{
    Q_OBJECT
public:
    explicit ProgramRecorderView(QWidget *parent = 0);
    ~ProgramRecorderView();

protected slots:
    void stopRecording();

private:
    ProgramRecorderViewPrivate *d;
};

#endif // PROGRAMRECORDERVIEW_H
