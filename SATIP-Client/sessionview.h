#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QTreeView>
#include <QProcess>

class RTSPSession;
class SessionViewPrivate;
class SessionView : public QTreeView
{
    Q_OBJECT
public:
    explicit SessionView(QWidget *parent = 0);
    ~SessionView();

protected slots:
    void removeSessionSlot();
    void zapChannelSlot();
    void joinSessionWithVlc();
    void forwardTo();
    void error(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void packetsAvailable(quint32 ssrc, QList<QByteArray>&packets);

signals:
    void addSession();
    void zapChannel(RTSPSession *);
    void removeSession(RTSPSession *);

private:
    SessionViewPrivate *d;
};

#endif // SESSIONVIEW_H
