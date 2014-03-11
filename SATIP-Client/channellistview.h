#ifndef CHANNELLISTVIEW_H
#define CHANNELLISTVIEW_H

#include <QTreeView>
#include <QProcess>

class GatewayDevice;
class ChannelListViewPrivate;
class ChannelListView : public QTreeView
{
    Q_OBJECT
public:
    explicit ChannelListView(QWidget *parent = 0);
    ~ChannelListView();

    void setGatewayDevice(GatewayDevice *device);
    void disableContextMenu(void);

protected slots:
//    void watchChannel(const QModelIndex &index);
    void collectEPG(const QModelIndex &index);
    void openMulticastSession();
    void openUnicastSession();
    void openWithVlc();
    void openRTSPWithVlc();
    void record();
    void recordRTSP();
    void streamTest();
    void streamTestRTSP();
//    void deleteChannel();

    void error(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void recordingDone(); 
    void testingDone();

private:
    ChannelListViewPrivate *d;
};

#endif // CHANNELLISTVIEW_H
