#ifndef SDTHANDLER_H
#define SDTHANDLER_H

#include "abstractdvbpsihandler.h"
#include "abstractdescriptor.h"
#include "sdtservice.h"

#include <QMap>

class DemuxHandler;
class SdtHandlerPrivate;
class SdtHandler : public AbstractDvbPsiHandler
{
    Q_OBJECT
public:
    enum WhichTS {
        CurrentTs = 0x42,
        OtherTs   = 0x46
    };

    explicit SdtHandler(const WhichTS &whichTS, const quint16 &transportStreamID, const AbstractDvbPsiHandler &other, DemuxHandler *demuxHandler);
    ~SdtHandler();

    WhichTS whichTS() const;
    quint16 transportStreamID() const;
    quint16 networkID() const;
    quint8 version() const;
    QMap<quint16, SdtService *> *services() const;

public slots:
    void attach();
    void dettach();

private slots:
    void demuxDettached();

private:
    SdtHandlerPrivate *d;

};

#endif // SDTHANDLER_H
