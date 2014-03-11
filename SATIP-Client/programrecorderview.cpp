#include "programrecorderview.h"
#include "programrecordermodel.h"

#include <QAction>

class ProgramRecorderViewPrivate
{
public:
    QAction *stopRecording;
};

ProgramRecorderView::ProgramRecorderView(QWidget *parent) :
    QTreeView(parent),
    d(new ProgramRecorderViewPrivate)
{
    setModel(new ProgramRecorderModel(this));
    d->stopRecording = new QAction("Stop recording", this);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    addAction(d->stopRecording);

    connect(d->stopRecording, SIGNAL(triggered()),
            this, SLOT(stopRecording()));
}

ProgramRecorderView::~ProgramRecorderView()
{
    delete d;
}


void ProgramRecorderView::stopRecording()
{
    QModelIndex index = currentIndex();

    QObject *o = static_cast<QObject *>(index.internalPointer());
    ProgramRecorder *recorder= qobject_cast<ProgramRecorder *>(o);

    recorder->stop();
}
