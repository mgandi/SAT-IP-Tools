#include "udptableview.h"
#include "settings.h"

#include <QProcess>

UdpTableView::UdpTableView(QWidget *parent) :
    QTableView(parent),
    removeRowAction("Remove listener", this),
    openWithVlc("Open with VLC", this)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    addAction(&removeRowAction);
    connect(&removeRowAction, SIGNAL(triggered()),
            this, SLOT(removeRowSlot()));

    addAction(&openWithVlc);
    connect(&openWithVlc, SIGNAL(triggered()),
            this, SLOT(openWithVlcSlot()));
}


void UdpTableView::error(QProcess::ProcessError error)
{
    Q_UNUSED(error);
}

void UdpTableView::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    QProcess *process = qobject_cast<QProcess *>(sender());
    process->deleteLater();
}


void UdpTableView::removeRowSlot()
{
    int row = currentIndex().row();
    emit removeRow(row);
}

void UdpTableView::openWithVlcSlot()
{
    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    int row = currentIndex().row();
    QVariant url = model()->data(model()->index(row, 0));
    QProcess *process = new QProcess(this);
    process->start(vlc, url.toStringList());
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished(int,QProcess::ExitStatus)));
}
