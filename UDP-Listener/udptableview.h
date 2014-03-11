#ifndef UDPTABLEVIEW_H
#define UDPTABLEVIEW_H

#include <QTableView>
#include <QAction>
#include <QProcess>

class UdpTableView : public QTableView
{
    Q_OBJECT
public:
    explicit UdpTableView(QWidget *parent = 0);

protected slots:
    void error(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void removeRow(int row);

private slots:
    void removeRowSlot();
    void openWithVlcSlot();

private:
    QAction removeRowAction;
    QAction openWithVlc;
};

#endif // UDPTABLEVIEW_H
