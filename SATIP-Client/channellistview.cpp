#include "channellistview.h"
#include "program.h"
#include "gatewaydevice.h"
#include "elementarystream.h"
#include "settings.h"
#include "epgcollector.h"
#include "programrecorder.h"
#include "programrecorderrtsp.h"
#include "objectpool.h"
#include "programtester.h"
#include "programmodel.h"

#include <QUrlQuery>
#include <QProcess>
#include <QAction>
#include <QFileDialog>

class ChannelListViewPrivate
{
public:
    ChannelListViewPrivate(ChannelListView *parent) :
        p(parent),
        device(0),
        openMulticastSession("Open multicast session", p),
        openUnicastSession("Open unicast session", p),
        openWithVlc("Open with VLC", p),
        openRTSPWithVlc("Open RTSP with VLC", p),
        record("Record", p),
        recordRTSP("Record RTSP", p),
        streamTest("Stream test", p),
        streamTestRTSP("Stream test RTSP", p),
        //deleteChannel("Delete channel", p),
        epgCollector(0)
    {}

    ChannelListView *p;
    GatewayDevice *device;
    QAction openMulticastSession;
    QAction openUnicastSession;
    QAction openWithVlc;
    QAction openRTSPWithVlc;
    QAction record;
    QAction recordRTSP;
    QAction streamTest;
    QAction streamTestRTSP;
    //QAction deleteChannel;

    EpgCollector *epgCollector;
};

ChannelListView::ChannelListView(QWidget *parent) :
    QTreeView(parent),
    d(new ChannelListViewPrivate(this))
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    addAction(&d->openMulticastSession);
    addAction(&d->openUnicastSession);
    addAction(&d->openWithVlc);
    addAction(&d->openRTSPWithVlc);
    addAction(&d->record);
    addAction(&d->recordRTSP);
    addAction(&d->streamTest);
    addAction(&d->streamTestRTSP);
    //addAction(&d->deleteChannel);

    connect(&d->openMulticastSession, SIGNAL(triggered()),
            this, SLOT(openMulticastSession()));
    connect(&d->openUnicastSession, SIGNAL(triggered()),
            this, SLOT(openUnicastSession()));
    connect(&d->openWithVlc, SIGNAL(triggered()),
            this, SLOT(openWithVlc()));
    connect(&d->openRTSPWithVlc, SIGNAL(triggered()),
            this, SLOT(openRTSPWithVlc()));
    connect(&d->record, SIGNAL(triggered()),
            this, SLOT(record()));
    connect(&d->recordRTSP, SIGNAL(triggered()),
            this, SLOT(recordRTSP()));
    connect(&d->streamTest, SIGNAL(triggered()),
            this, SLOT(streamTest()));
    connect(&d->streamTestRTSP, SIGNAL(triggered()),
            this, SLOT(streamTestRTSP()));
//    connect(&d->deleteChannel, SIGNAL(triggered()),
//            this, SLOT(deleteChannel()));
//    connect(this, SIGNAL(clicked(QModelIndex)),
//            this, SLOT(collectEPG(QModelIndex)));
}

ChannelListView::~ChannelListView()
{
    delete d;
}

void ChannelListView::setGatewayDevice(GatewayDevice *device)
{
    d->device = device;
}

#if 0
void ChannelListView::watchChannel(const QModelIndex &index)
{
    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    QUrl url;
    QUrlQuery urlQuery;

    urlQuery.addQueryItem("msys", "dvbt");
    urlQuery.addQueryItem("bw", QString("%1").arg(program->bandwidth()));
    urlQuery.addQueryItem("freq", QString("%1").arg(program->frequency()));

    QStringList pids;
    pids += QString("0");
    pids += QString("%1").arg(program->pid());
    foreach (ElementaryStream *es, program->elementaryStreams()) {
        pids += QString("%1").arg(es->pid());
    }
    urlQuery.addQueryItem("pids", pids.join(","));

    url.setScheme("http");
    url.setHost(d->device->host());
    url.setPath("/");
    url.setQuery(urlQuery);

    QProcess *process = new QProcess(this);
    process->start(vlc, QStringList() << url.toString());
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished(int,QProcess::ExitStatus)));
}
#endif

void ChannelListView::collectEPG(const QModelIndex &index)
{
    if (d->epgCollector) {
        delete d->epgCollector;
        d->epgCollector = 0;
    }

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    d->epgCollector = new EpgCollector(d->device, true, this);
}

void ChannelListView::openMulticastSession()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    // Create the session
    d->device->addSession(program, false);
}

void ChannelListView::openUnicastSession()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    // Create the session
    d->device->addSession(program, true);
}

void ChannelListView::openWithVlc()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    QUrl url = program->toUrl(Program::HTTP, d->device->host());

    QProcess *process = new QProcess(this);
    process->start(vlc, QStringList() << url.toString());
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished(int,QProcess::ExitStatus)));
}

void ChannelListView::openRTSPWithVlc()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    QUrl url = program->toUrl(Program::RTSP, d->device->host());

    QProcess *process = new QProcess(this);
    process->start(vlc, QStringList() << url.toString());
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(finished(int,QProcess::ExitStatus)));
}

void ChannelListView::record()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save recording to file...", QDir::homePath(), "*.ts");
    if (fileName.isEmpty())
        return;

    ProgramRecorder *recorder = new ProgramRecorder(d->device, program, fileName, this);
    recorder->start();
    connect(recorder, SIGNAL(recordingDone()),
            this, SLOT(recordingDone()));

    ObjectPool::instance()->addObject(recorder);
}

void ChannelListView::recordRTSP()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    Settings settings;
    QString vlc = settings.vlc();
    if (vlc.isEmpty())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save recording to file...", QDir::homePath(), "*.ts");
    if (fileName.isEmpty())
        return;

    ProgramRecorderRTSP *recorder = new ProgramRecorderRTSP(d->device, program, fileName, this);
    recorder->start();
    connect(recorder, SIGNAL(recordingDone()),
            this, SLOT(recordingDone()));

    ObjectPool::instance()->addObject(recorder);
}

void ChannelListView::streamTest()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    ProgramTester *tester = new ProgramTester(d->device, program, Program::HTTP, this);
    connect(tester, SIGNAL(testingDone()),
            this, SLOT(testingDone()));

    ObjectPool::instance()->addObject(tester);

    tester->start();
}

void ChannelListView::streamTestRTSP()
{
    QModelIndex index = currentIndex();

    // Make sure there a current index
    if (!index.isValid())
        return;

    if (!d->device)
        return;

    QObject *o = static_cast<QObject *>(index.internalPointer());
    Program *program = qobject_cast<Program *>(o);

    if (!program)
        return;

    ProgramTester *tester = new ProgramTester(d->device, program, Program::RTSP, this);
    connect(tester, SIGNAL(testingDone()),
            this, SLOT(testingDone()));

    ObjectPool::instance()->addObject(tester);
    tester->start();
}

//void ChannelListView::deleteChannel()
//{
//    QModelIndex index = currentIndex();

//    // Make sure there a current index
//    if (!index.isValid())
//        return;

//    QObject *o = static_cast<QObject *>(index.internalPointer());
//    Program *program = qobject_cast<Program *>(o);

//    if (!program)
//        return;

//    //ProgramModel *programModel = static_cast<const ProgramModel *>(index.model());
//    //programModel->removeProgram(program);
//    //delete program;
//}

void ChannelListView::error(QProcess::ProcessError error)
{
    Q_UNUSED(error);
}

void ChannelListView::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    QProcess *process = qobject_cast<QProcess *>(sender());
    process->deleteLater();
}

void ChannelListView::recordingDone()
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(sender());
    ObjectPool::instance()->removeObject(recorder);
    delete recorder;
}

void ChannelListView::testingDone()
{
    ProgramTester *tester = qobject_cast<ProgramTester *>(sender());
    ObjectPool::instance()->removeObject(tester);
    delete tester;
}
