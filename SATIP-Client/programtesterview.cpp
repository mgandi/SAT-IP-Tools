#include "programtesterview.h"
#include "programtestermodel.h"

#include <QAction>

class ProgramTesterViewPrivate
{
public:
    QAction *stopTesting;
    QAction *clearError;
};

ProgramTesterView::ProgramTesterView(QWidget *parent) :
    QTreeView(parent),
    d(new ProgramTesterViewPrivate)
{
    setModel(new ProgramTesterModel(this));
    d->stopTesting = new QAction("Stop testing", this);
    d->clearError = new QAction("Clear error", this);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    addAction(d->stopTesting);
    connect(d->stopTesting, SIGNAL(triggered()),
            this, SLOT(stopTesting()));

    addAction(d->clearError);
    connect(d->clearError, SIGNAL(triggered()),
            this, SLOT(clearError()));
}

ProgramTesterView::~ProgramTesterView()
{
    delete d;
}


void ProgramTesterView::stopTesting()
{
    QModelIndex index = currentIndex();

    QObject *o = static_cast<QObject *>(index.internalPointer());
    ProgramTester *tester= qobject_cast<ProgramTester *>(o);

    tester->stop();
}

void ProgramTesterView::clearError()
{
    QModelIndex index = currentIndex();

    QObject *o = static_cast<QObject *>(index.internalPointer());
    ProgramTester *tester= qobject_cast<ProgramTester *>(o);

    tester->clearError();
}
