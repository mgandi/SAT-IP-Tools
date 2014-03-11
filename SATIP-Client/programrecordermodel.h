#ifndef PROGRAMRECORDERMODEL_H
#define PROGRAMRECORDERMODEL_H

#include "programrecorder.h"

#include <QAbstractItemModel>

class ProgramRecorderModelPrivate;
class ProgramRecorderModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ProgramRecorderModel(QObject *parent = 0);
    ~ProgramRecorderModel();

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    void addRecorder(ProgramRecorder *recorder);
    void removeRecorder(ProgramRecorder *recorder);

protected slots:
    void programRecorderStatusChanged(ProgramRecorder::Status status);
    void timeToStartChanged(QTime timeToStart);
    void timeToEndChanged(QTime timeToEnd);
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

private:
    ProgramRecorderModelPrivate *d;
};

#endif // PROGRAMRECORDERMODEL_H
