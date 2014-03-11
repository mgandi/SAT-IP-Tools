#include "programmodel.h"
#include "program.h"
#include "elementarystream.h"
#include "gatewaydevice.h"
#include "settings.h"
#include "objectpool.h"

#include <QIcon>
#include <QUrlQuery>

class ProgramModelPrivate
{
public:
    QList<Program *> programs;

    GatewayDevice *device;
    ProgramModel::NetworkType networkType;
    bool showEs;
};

ProgramModel::ProgramModel(GatewayDevice *device, NetworkType networkType, bool showEs, QObject *parent) :
    QAbstractItemModel(parent),
    d(new ProgramModelPrivate())
{
    d->device = device;
    d->networkType = networkType;
    d->showEs = showEs;
    d->programs = d->device->programs();
    foreach (Program *program, d->programs) {
        addProgram(program);
    }

    connect(ObjectPool::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    connect(ObjectPool::instance(), SIGNAL(objectRemoved(QObject*)),
            this, SLOT(objectRemoved(QObject*)));
}

ProgramModel::~ProgramModel()
{
    delete d;
}

QList<Program *> ProgramModel::programs() const
{
    return d->programs;
}

int ProgramModel::columnCount(const QModelIndex &) const
{
    return 4;
}

int ProgramModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->programs.size();
    } else {
        QObject *o = (QObject *)parent.internalPointer();
        Program *program = qobject_cast<Program *>(o);
        if (program && d->showEs) {
            return program->elementaryStreams().size();
        } else {
            return 0;
        }
    }
}

QModelIndex ProgramModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, (void *)d->programs.at(row));
    else {
        QObject *o = (QObject *)parent.internalPointer();
        Program *program = qobject_cast<Program *>(o);
        if (program && d->showEs) {
            return createIndex(row, column, (void *)program->elementaryStreams().at(row));
        } else {
            return QModelIndex();
        }
    }
}

QModelIndex ProgramModel::parent(const QModelIndex &child) const
{
    QObject *o = (QObject *)child.internalPointer();
    ElementaryStream *es = qobject_cast<ElementaryStream *>(o);

    if (es && d->showEs) {
        Program *program = qobject_cast<Program *>(es->parent());
        return createIndex(d->programs.indexOf(program), 0, (void *)program);
    }

    return QModelIndex();
}

QVariant ProgramModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        switch (orientation) {
        case Qt::Horizontal:
            switch (section) {
            case 0:
                return "Channel";
            case 1:
                return "Service ID";
            case 2:
                return "Frequency";
            case 3:
                return "Scrambled";
            }
            break;
        default:
            break;
        }
        break;
    }

    return QVariant();
}

QVariant ProgramModel::data(const QModelIndex &index, int role) const
{

    QObject *o = (QObject *)index.internalPointer();
    Program *program = qobject_cast<Program *>(o);
    ElementaryStream *es = qobject_cast<ElementaryStream *>(o);

    switch (role) {

    case Qt::DisplayRole:

        if (program) {
            switch (index.column()) {
            case 0: {
                if (program->serviceName().isEmpty()) {
                    return program->number();
                } else {
                    return program->serviceName();
                }
            }
            case 1: {
                return QString("0x%1").arg(program->number(), 4, 16, QLatin1Char('0'));
            }
            case 2: {
                return QString("%1 MHz").arg(program->frequency());
            }
            case 3: {
                return QString("%1").arg(program->scrambled());
            }
            }
        } else if (es && d->showEs) {
            switch (index.column()) {
            case 0: {
                return QString("PID 0x%1 (%2)").arg(es->pid(), -4, 16).arg(es->pid());
                break;
            }
            case 1: {
                return es->typeToString();
            }
            }
        }

        break;

    case Qt::DecorationRole:
        if (program && (index.column() == 0)) {
            if (program->containsVideo()) {
                return QIcon(":/images/VIDEO");
            } else if (program->containsAudio()) {
                return QIcon(":/images/AUDIO");
            }
        }
        break;
    }

    return QVariant();
}


void ProgramModel::addProgram(Program *program)
{
    if (d->programs.contains(program))
        return;

    if (!program->containsAudio() && !program->containsVideo())
        return;

    int index = d->programs.size();
    beginInsertRows(QModelIndex(), index, index);
    d->programs += program;
    endInsertRows();
}

void ProgramModel::removeProgram(Program *program)
{
    if (!d->programs.contains(program))
        return;

    int index = d->programs.indexOf(program);
    beginRemoveRows(QModelIndex(), index, index);
    d->programs.takeAt(index);
    endRemoveRows();
}

void ProgramModel::setShowES(bool showES)
{
    d->showEs = showES;
    beginRemoveRows(QModelIndex(), 0, d->programs.size() - 1);
    endRemoveRows();
    beginInsertRows(QModelIndex(), 0, d->programs.size() - 1);
    endInsertRows();
}

QString ProgramModel::toM3U(int type) const
{
    QStringList progs;
    progs += "#EXTM3U\n";

    foreach (Program *program, d->programs) {
        QString prog;
        prog += "#EXTINF:0,0. ";
        prog += program->serviceName();
        prog += "\n";

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

        switch (type) {
        case Program::RTSP:
            url.setScheme("rtsp");
            break;
        case Program::HTTP:
            url.setScheme("http");
            break;
        }

        url.setHost(d->device->host());
        url.setPath("/");
        url.setQuery(urlQuery);

        prog += url.toString();
        prog += "\n";

        progs += prog;
    }

    progs += "\n";

    return progs.join("\n");
}

void ProgramModel::objectAdded(QObject *object)
{
    Program *program = qobject_cast<Program *>(object);
    if (program)
        addProgram(program);
}

void ProgramModel::objectRemoved(QObject *object)
{
    Program *program = qobject_cast<Program *>(object);
    if (program)
        removeProgram(program);
}
