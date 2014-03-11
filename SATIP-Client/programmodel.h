#ifndef PROGRAMMODEL_H
#define PROGRAMMODEL_H

#include <QAbstractListModel>

class GatewayDevice;
class Program;
class ProgramModelPrivate;
class ProgramModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum NetworkType {
        DVBT  = 0x0,
        DVBS  = 0x01
    };

    explicit ProgramModel(GatewayDevice *device, NetworkType networkType, bool showEs, QObject *parent = 0);
    ~ProgramModel();

    QList<Program *> programs() const;

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    void addProgram(Program *program);
    void removeProgram(Program *program);

    void setShowES(bool showES);

    QString toM3U(int type = 0) const;

protected slots:
    void objectAdded(QObject *object);
    void objectRemoved(QObject *object);

private:
    ProgramModelPrivate *d;
};

#endif // PROGRAMMODEL_H
