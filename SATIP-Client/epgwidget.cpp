#include "epgwidget.h"
#include "ui_epgwidget.h"
#include "eventitem.h"
#include "event.h"
#include "objectpool.h"
#include "program.h"
#include "programitem.h"
#include "epgconstants.h"
#include "gatewaydevice.h"
#include "epgcollector.h"
#include "event_p.h"
#include "programrecorder.h"

#include <QDateTime>
#include <QTime>
#include <QtDebug>
#include <QScrollBar>

class EpgWidgetPrivate
{
public:
    EpgWidgetPrivate(GatewayDevice *de, EpgWidget *parent) :
        p(parent),
        device(de),
        epgCollector(device, false, p)
    {}

    EpgWidget *p;
    GatewayDevice *device;
    EpgCollector epgCollector;
    QGraphicsScene epgScene, channelScene;
    QList<Program *> prgms;
    QHash<quint16, Program *> programs;
    QHash<quint16, EventItem *> events;
    QHash<Program *, ProgramItem *> items;
};



EpgWidget::EpgWidget(GatewayDevice *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EpgWidget),
    d(new EpgWidgetPrivate(device, this))
{
    // Setup UI
    ui->setupUi(this);
    ui->epgView->setScene(&d->epgScene);
    ui->epgView->setRenderHint(QPainter::Antialiasing, true);
    resizeEpgScene();
    ui->channelListView->setScene(&d->channelScene);
    ui->channelListView->setRenderHint(QPainter::Antialiasing, true);
    resizeChannelScene();
    connect(ui->channelListView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            ui->epgView->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->epgView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            ui->channelListView->verticalScrollBar(), SLOT(setValue(int)));

    // Take care of the Gateway Device
    d->device = device;
    foreach (Program *program, d->device->programs()) {
        addProgram(program);
    }

    // Look for new program to be added dynamically
    connect(ObjectPool::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    connect(ObjectPool::instance(), SIGNAL(objectRemoved(QObject*)),
            this, SLOT(objectRemoved(QObject*)));

    // Connect EPG collector signal
    connect(&d->epgCollector, SIGNAL(currentProgramChanged(Program*,Program*)),
            this, SLOT(currentProgramChanged(Program*,Program*)));
}

EpgWidget::~EpgWidget()
{
    delete d;
    delete ui;
}

void EpgWidget::recordEvent(Event *event)
{
    ProgramRecorder *recorder = new ProgramRecorder(d->device, event, this);
    connect(recorder, SIGNAL(recordingDone()),
            this, SLOT(recordingDone()));

    ObjectPool::instance()->addObject(recorder);
}

void EpgWidget::addProgram(Program *program)
{
    d->programs.insert(program->number(), program);
    d->prgms += program;
    int index = d->prgms.indexOf(program);
    ProgramItem *item = new ProgramItem(index, program, &d->epgCollector);
    d->items.insert(program, item);
    d->channelScene.addItem(item);
    resizeEpgScene();
    resizeChannelScene();
}

void EpgWidget::removeProgram(Program *program)
{
    if (!d->prgms.contains(program))
        return;

    d->prgms.removeAll(program);
    d->programs.remove(d->programs.key(program));
    ProgramItem *item = d->items.value(program);
    d->channelScene.removeItem(item);
    delete item;
    resizeEpgScene();
    resizeChannelScene();
}

void EpgWidget::addEvent(Event *event)
{
    Program *program = d->programs.value(event->serviceID());
    int index = d->prgms.indexOf(program);
    if ((index >= 0) && event->isValid()) {
        EventItem *item = new EventItem(index, event, this);
        if (event->startDateTime().date() == QDate::currentDate()) {
            d->epgScene.addItem(item);
            d->events.insertMulti(event->serviceID(), item);
        }
    }
}

void EpgWidget::removeEvent(Event *event)
{
}

void EpgWidget::objectAdded(QObject *object)
{
    Program *program = qobject_cast<Program *>(object);
    Event *event = qobject_cast<Event *>(object);

    if (program)
        addProgram(program);

    else if (event)
        addEvent(event);
}

void EpgWidget::objectRemoved(QObject *object)
{
    Program *program = qobject_cast<Program *>(object);
    Event *event = qobject_cast<Event *>(object);

    if (program)
        removeProgram(program);

//    if (program && d->programs.contains(program->number())) {
//        d->programs.remove(program->number());
//        resizeEpgScene();
//        resizeChannelScene();
//    }

//    else if (event)  {
////        int index = d->programs.keys().indexOf(event->serviceID());
////        if (index >= 0) {
////            EventItem *item = new EventItem(index, event);
////            d->epgScene.addItem(item);
////            d->events.insertMulti(event->serviceID(), item);
////        }
//    }
}

void EpgWidget::resizeChannelScene()
{
    int x = 0, y = 0, w = PROGRAM_ITEM_WIDTH, h = (d->prgms.size() < 10 ? 10 : d->prgms.size()) * BOX_HEIGHT;
    ui->channelListView->setSceneRect(x,y,w,h);
}

void EpgWidget::resizeEpgScene()
{
    ui->epgView->setSceneRect(0, 0/*2 * BOX_HEIGHT*/, (60 * 24) * 3, (d->prgms.size() < 10 ? 10 : d->prgms.size()) * BOX_HEIGHT);
}

void EpgWidget::currentProgramChanged(Program *newProgram, Program *oldProgram)
{
    if (oldProgram) {
        ProgramItem *item = d->items.value(oldProgram, 0);
        if (item) {
            item->setColleting(false);
        }
    }

    if (newProgram) {
        ProgramItem *item = d->items.value(newProgram, 0);
        if (item) {
            item->setColleting(true);
        }
    }
}

void EpgWidget::recordingDone()
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(sender());
    ObjectPool::instance()->removeObject(recorder);
    delete recorder;
}

void EpgWidget::on_collectEpg_clicked(bool checked)
{
    if (checked)
        d->epgCollector.start();
    else
        d->epgCollector.stop();
}

void EpgWidget::on_testButton_clicked()
{
    EventPrivate *event_p = new EventPrivate;
    event_p->tableID = 0x50;
    event_p->serviceID = 0x0501;
    event_p->versionNumber = 0x00;
    event_p->transportStreamID = 0x0005;
    event_p->originalNetworkID = 0x20fa;
    event_p->eventID = 0xff;
    QTime c = QTime::currentTime();
    event_p->startDateTime = QDateTime(QDate::currentDate(), QTime(c.hour(),c.minute() + 1,c.second()));
    event_p->duration = QTime(0,1,00);
    event_p->name = "Marc's event";
    event_p->shortDescription = "Test event";
    event_p->type = Event::Undefined;
    event_p->complete = true;
    Event *event = new Event(event_p);
    ObjectPool::instance()->addObject(event);
}
