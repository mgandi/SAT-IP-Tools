#include "programtestermodel.h"
#include "programtester.h"
#include "objectpool.h"
#include "program.h"

#include <QIcon>

#define TEST_COLUMN_COUNT 6

class ProgramTesterModelPrivate
{
public:
    QList<ProgramTester *> testers;
};

ProgramTesterModel::ProgramTesterModel(QObject *parent) :
    QAbstractItemModel(parent),
    d(new ProgramTesterModelPrivate)
{
    connect(ObjectPool::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    connect(ObjectPool::instance(), SIGNAL(objectRemoved(QObject*)),
            this, SLOT(objectRemoved(QObject*)));
}

ProgramTesterModel::~ProgramTesterModel()
{
    delete d;
}


int ProgramTesterModel::columnCount(const QModelIndex &/*parent*/) const
{
    return TEST_COLUMN_COUNT;
}

int ProgramTesterModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->testers.size();
    }

    return 0;
}

QModelIndex ProgramTesterModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && row < d->testers.size())
        return createIndex(row, column, (void *)d->testers.at(row));

    return QModelIndex();
}

QModelIndex ProgramTesterModel::parent(const QModelIndex &/*child*/) const
{
    return QModelIndex();
}

QVariant ProgramTesterModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return "PID";
            case 3:
                return "Num of TS";
            case 4:
                return "TS counter";
            case 5:
                return "Error";
            case 6:
                return "progress";
            }
            break;
        default:
            break;
        }
        break;
    }

    return QVariant();
}

QVariant ProgramTesterModel::data(const QModelIndex &index, int role) const
{
    QObject *o = (QObject *)index.internalPointer();
    ProgramTester *tester = qobject_cast<ProgramTester *>(o);

    switch (role) {

    case Qt::DisplayRole:

        if (tester)
            switch (index.column()) {
            case 0: {
                Program *program = tester->program();
                if (program->serviceName().isEmpty())
                    return program->number();
                else
                    return program->serviceName();
                }
            case 2:
                return QString("%1").arg(tester->pidRef());
            case 3:
                return QString("%1").arg(tester->numTs());
            case 4: 
                return QString("%1").arg(tester->tsCnt() - tester->tsCnt() % 1000);
            case 5:
                return QString("%1").arg(tester->err());
            case 6:
                return QString("%1").arg((tester->tsCnt() / 1000) % 100);
            }
        break;

    case Qt::DecorationRole:
       if (tester)
          switch (index.column()) {
          case 1:
             switch (tester->status()) {
             case ProgramTester::Testing:
                return QIcon(":/images/TEST");
             case ProgramTester::Done:
                return QIcon(":/images/OK");
             }
             break;
          }
       break;
    }

    return QVariant();
}


void ProgramTesterModel::addTester(ProgramTester *tester)
{
    if (d->testers.contains(tester))
        return;

    int index = d->testers.size();
    beginInsertRows(QModelIndex(), index, index);
    d->testers += tester;
    connect(tester, SIGNAL(statusChanged(ProgramTester::Status)),
            this, SLOT(programTesterStatusChanged(ProgramTester::Status)));
    connect(tester, SIGNAL(tsCntChanged(quint32)),
            this, SLOT(tsCntChanged(quint32)));
    connect(tester, SIGNAL(errChanged(quint32)),
            this, SLOT(errChanged(quint32)));
    endInsertRows();
}

void ProgramTesterModel::removeTester(ProgramTester *tester)
{
    if (!d->testers.contains(tester))
        return;

    int index = d->testers.indexOf(tester);
    beginRemoveRows(QModelIndex(), index, index);
    d->testers.takeAt(index);
    endRemoveRows();
}

void ProgramTesterModel::programTesterStatusChanged(ProgramTester::Status /*status*/)
{
    ProgramTester *tester = qobject_cast<ProgramTester *>(sender());
    int row = d->testers.indexOf(tester);
    emit dataChanged(index(row, 1, QModelIndex()), index(row, 1, QModelIndex()), QVector<int>() << Qt::DecorationRole);
}

void ProgramTesterModel::tsCntChanged(quint32)
{
    ProgramTester *tester = qobject_cast<ProgramTester *>(sender());
    int row = d->testers.indexOf(tester);
    emit dataChanged(index(row, 4, QModelIndex()), index(row, 4, QModelIndex()), QVector<int>() << Qt::DisplayRole);
}

void ProgramTesterModel::errChanged(quint32)
{
    ProgramTester *tester = qobject_cast<ProgramTester *>(sender());
    int row = d->testers.indexOf(tester);
    emit dataChanged(index(row, 5, QModelIndex()), index(row, 5, QModelIndex()), QVector<int>() << Qt::DisplayRole);
}

void ProgramTesterModel::objectAdded(QObject *object)
{
    ProgramTester *tester = qobject_cast<ProgramTester *>(object);
    if (tester)
        addTester(tester);
}

void ProgramTesterModel::objectRemoved(QObject *object)
{
    ProgramTester *tester = qobject_cast<ProgramTester *>(object);
    if (tester)
        removeTester(tester);
}
