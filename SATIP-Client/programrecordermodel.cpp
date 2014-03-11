#include "programrecordermodel.h"
#include "programrecorder.h"
#include "objectpool.h"
#include "program.h"

#include <QIcon>

class ProgramRecorderModelPrivate
{
public:
    QList<ProgramRecorder *> recorders;
};

ProgramRecorderModel::ProgramRecorderModel(QObject *parent) :
    QAbstractItemModel(parent),
    d(new ProgramRecorderModelPrivate)
{
    connect(ObjectPool::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    connect(ObjectPool::instance(), SIGNAL(objectRemoved(QObject*)),
            this, SLOT(objectRemoved(QObject*)));
}

ProgramRecorderModel::~ProgramRecorderModel()
{
    delete d;
}


int ProgramRecorderModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 6;
}

int ProgramRecorderModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->recorders.size();
    }

    return 0;
}

QModelIndex ProgramRecorderModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && row < d->recorders.size())
        return createIndex(row, column, (void *)d->recorders.at(row));

    return QModelIndex();
}

QModelIndex ProgramRecorderModel::parent(const QModelIndex &/*child*/) const
{
    return QModelIndex();
}

QVariant ProgramRecorderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        switch (orientation) {
        case Qt::Horizontal:
            switch (section) {
            case 0:
                return "Name";
            case 1:
                return "Status";
            case 2:
                return "Time to start";
            case 3:
                return "Time to end";
            case 4:
                return "Start date time";
            case 5:
                return "End date time";
            }
            break;
        default:
            break;
        }
        break;
    }

    return QVariant();
}

QVariant ProgramRecorderModel::data(const QModelIndex &index, int role) const
{

    QObject *o = (QObject *)index.internalPointer();
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(o);

    switch (role) {

    case Qt::DisplayRole:

        if (recorder) {
            switch (index.column()) {
            case 0: {
                Program *program = recorder->program();
                if (program->serviceName().isEmpty()) {
                    return program->number();
                } else {
                    return program->serviceName();
                }
            }
            case 2: {
                if (!recorder->timeToStart().isNull()) {
                    return recorder->timeToStart().toString("hh:mm:ss");
                } else {
                    return "N/A";
                }
            }
            case 3: {
                if (!recorder->timeToEnd().isNull()) {
                    return recorder->timeToEnd().toString("hh:mm:ss");
                } else {
                    return "N/A";
                }
            }
            case 4: {
                if (!recorder->startTime().isNull()) {
                    return recorder->startTime().toString("ddd MMMM d yyyy - hh:mm");
                } else {
                    return "N/A";
                }
            }
            case 5: {
                if (!recorder->endTime().isNull()) {
                    return recorder->endTime().toString("ddd MMMM d yyyy - hh:mm");
                } else {
                    return "N/A";
                }
            }
            }
        }

        break;

    case Qt::DecorationRole:
        if (recorder && (index.column() == 1)) {
            switch (recorder->status()) {
            case ProgramRecorder::WaitingForStartTime:
                return QIcon(":/images/CLOCK");
            case ProgramRecorder::Recording:
                return QIcon(":/images/RECORD");
            case ProgramRecorder::Done:
                return QIcon(":/images/OK");
            }
        }
        break;
    }

    return QVariant();
}


void ProgramRecorderModel::addRecorder(ProgramRecorder *recorder)
{
    if (d->recorders.contains(recorder))
        return;

    int index = d->recorders.size();
    beginInsertRows(QModelIndex(), index, index);
    d->recorders += recorder;
    connect(recorder, SIGNAL(statusChanged(ProgramRecorder::Status)),
            this, SLOT(programRecorderStatusChanged(ProgramRecorder::Status)));
    connect(recorder, SIGNAL(timeToStartChanged(QTime)),
            this, SLOT(timeToStartChanged(QTime)));
    connect(recorder, SIGNAL(timeToEndChanged(QTime)),
            this, SLOT(timeToEndChanged(QTime)));
    endInsertRows();
}

void ProgramRecorderModel::removeRecorder(ProgramRecorder *recorder)
{
    if (!d->recorders.contains(recorder))
        return;

    int index = d->recorders.indexOf(recorder);
    beginRemoveRows(QModelIndex(), index, index);
    d->recorders.takeAt(index);
    endRemoveRows();
}

void ProgramRecorderModel::programRecorderStatusChanged(ProgramRecorder::Status /*status*/)
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(sender());
    int row = d->recorders.indexOf(recorder);
    emit dataChanged(index(row, 1, QModelIndex()), index(row, 1, QModelIndex()), QVector<int>() << Qt::DecorationRole);
}

void ProgramRecorderModel::timeToStartChanged(QTime /*timeToStart*/)
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(sender());
    int row = d->recorders.indexOf(recorder);
    emit dataChanged(index(row, 2, QModelIndex()), index(row, 2, QModelIndex()), QVector<int>() << Qt::DisplayRole);
}

void ProgramRecorderModel::timeToEndChanged(QTime /*timeToEnd*/)
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(sender());
    int row = d->recorders.indexOf(recorder);
    emit dataChanged(index(row, 3, QModelIndex()), index(row, 3, QModelIndex()), QVector<int>() << Qt::DisplayRole);
}


void ProgramRecorderModel::objectAdded(QObject *object)
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(object);
    if (recorder)
        addRecorder(recorder);
}

void ProgramRecorderModel::objectRemoved(QObject *object)
{
    ProgramRecorder *recorder = qobject_cast<ProgramRecorder *>(object);
    if (recorder)
        removeRecorder(recorder);
}
