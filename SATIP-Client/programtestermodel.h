#ifndef PROGRAMTESTERMODEL_H
#define PROGRAMTESTERMODEL_H

#include "programtester.h"

#include <QAbstractItemModel>

class ProgramTesterModelPrivate;
class ProgramTesterModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ProgramTesterModel(QObject *parent = 0);
    ~ProgramTesterModel();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    void addTester(ProgramTester *recorder);
    void removeTester(ProgramTester *recorder);

protected slots:
    void programTesterStatusChanged(ProgramTester::Status status);
    void tsCntChanged(quint32 tsCnt);
    void errChanged(quint32 err);
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

private:
    ProgramTesterModelPrivate *d;
};

#endif // PROGRAMTESTERMODEL_H
