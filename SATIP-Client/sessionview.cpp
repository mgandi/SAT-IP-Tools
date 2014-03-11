#include "sessionview.h"
#include "sessionmodel.h"
#include "settings.h"
#include "udpforwarddialog.h"

#include <QAction>
#include <QUrl>
#include <QContextMenuEvent>
#include <QMenu>
#include <QUdpSocket>
#include <QtDebug>
#include <QMessageBox>

struct Forward {
    Forward(QHostAddress a, quint16 p) :
        address(a), port(p) {}
    Forward() {}

    QHostAddress address;
    quint16 port;
};

class SessionViewPrivate
{
public:
    QAction *addSessionAction;
    QAction *removeSessionAction;
    QAction *joinSessionWithVlc;
    QAction *forwardTo;
    QAction *zapChannelAction;

    QHash<RTSPSession *, Forward> forwards;
    QHash<RTSPSession *, QUdpSocket *> sockets;
};

SessionView::SessionView(QWidget *parent) :
    QTreeView(parent),
    d(new SessionViewPrivate)
{
    d->addSessionAction = new QAction("Add session", this);
    d->removeSessionAction = new QAction("Remove session", this);
    d->joinSessionWithVlc = new QAction("Join session with VLC", this);
    d->forwardTo = new QAction("Forward to...", this); // NOT WORKING !!!
    d->zapChannelAction = new QAction("Change channel", this);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    addAction(d->addSessionAction);
    addAction(d->removeSessionAction);
    addAction(d->joinSessionWithVlc);
//    addAction(d->forwardTo);
    addAction(d->zapChannelAction);

    connect(d->addSessionAction, SIGNAL(triggered()),
            this, SIGNAL(addSession()));
    connect(d->removeSessionAction, SIGNAL(triggered()),
            this, SLOT(removeSessionSlot()));
    connect(d->joinSessionWithVlc, SIGNAL(triggered()),
            this, SLOT(joinSessionWithVlc()));
    connect(d->forwardTo, SIGNAL(triggered()),
            this, SLOT(forwardTo()));
    connect(d->zapChannelAction, SIGNAL(triggered()),
            this, SLOT(zapChannelSlot()));
}

SessionView::~SessionView()
{
    delete d;
}


void SessionView::removeSessionSlot()
{
    QModelIndex index = currentIndex();
    int row = index.row();
    SessionModel *m = qobject_cast<SessionModel *>(model());
    emit removeSession(m->sessionAtIndex(row));
}

void SessionView::zapChannelSlot()
{
    QModelIndex index = currentIndex();
    int row = index.row();
    SessionModel *m = qobject_cast<SessionModel *>(model());
    emit zapChannel(m->sessionAtIndex(row));
}

void SessionView::joinSessionWithVlc()
{
    QModelIndex index = currentIndex();

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    RTSPSession *session = qobject_cast<RTSPSession *>(o);

    if (!session || session->unicast()) {
        QMessageBox::question(this, "Session request error",
                             "Only multicast session can be joined",
                             QMessageBox::Ok);
        return;
    }
    QUrl url;
    url.setScheme("rtp");
    url.setHost(session->destination());
    url.setPort(session->rtpPort());

    QProcess *process = new QProcess(this);
    process->start(vlc, QStringList() << url.toString());
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished(int,QProcess::ExitStatus)));
}


void SessionView::forwardTo()
{
    QModelIndex index = currentIndex();

    QObject *o = static_cast<QObject *>(index.internalPointer());
    RTSPSession *session = qobject_cast<RTSPSession *>(o);

    if (!session || session->unicast())
        return;

    UdpForwardDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        d->forwards.insert(session, Forward(dialog.address(), dialog.port()));
        d->sockets.insert(session, new QUdpSocket(this));
        connect(session, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
                this, SLOT(packetsAvailable(quint32,QList<QByteArray>&)));
    }
}

void SessionView::error(QProcess::ProcessError error)
{
    Q_UNUSED(error);
}

void SessionView::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    QProcess *process = qobject_cast<QProcess *>(sender());
    process->deleteLater();
    qDebug() << "Finished: " << qPrintable(process->program()) << qPrintable(process->arguments().join(" "));
}

void SessionView::packetsAvailable(quint32, QList<QByteArray> &packets)
{
    RTSPSession *session = qobject_cast<RTSPSession *>(sender());
    Forward f = d->forwards.value(session);
    QUdpSocket *socket = d->sockets.value(session);

    foreach (QByteArray packet, packets) {
        if (socket->writeDatagram(packet, f.address, f.port) != packet.size())
            qDebug() << "Failed to send all data";
    }
}
