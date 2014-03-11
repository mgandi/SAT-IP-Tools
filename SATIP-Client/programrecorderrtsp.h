#ifndef PROGRAMRECORDERRTSP_H
#define PROGRAMRECORDERRTSP_H

#include "programrecorder.h"

class ProgramRecorderRTSPPrivate;
class ProgramRecorderRTSP : public ProgramRecorder
{
    Q_OBJECT

public:
    explicit ProgramRecorderRTSP(GatewayDevice *device, Program *program, const QString &fileName, QObject *parent = 0);
    ~ProgramRecorderRTSP();

    void stop();

protected slots:
    virtual void setupConnection();
    void packetsAvailable(quint32 ssrc, QList<QByteArray> &packets);

private:
    ProgramRecorderRTSPPrivate *d;
};

#endif // PROGRAMRECORDERRTSP_H
