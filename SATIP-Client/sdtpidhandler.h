#ifndef SDTPIDHANDLER_H
#define SDTPIDHANDLER_H

#include "abstractpidhandler.h"
#include "sdtservice.h"

class SdtPidHandlerPrivate;
class SdtPidHandler : public AbstractPidHandler
{
    Q_OBJECT

public:
    enum WhichTS {
        CurrentTs = 0x42,
        OtherTs   = 0x46
    };

    explicit SdtPidHandler(const WhichTS &whichTS, bool attach = true, QObject *parent = 0);
    ~SdtPidHandler();

    WhichTS whichTS() const;
    quint16 transportStreamID() const;
    quint16 networkID() const;
    quint8 version() const;
    QMap<quint16, SdtService *> *services() const;

protected slots:
    static void dumpDescriptor(SdtService *service, const quint8 payload[], const quint16 payloadSize);
    void parseTable(Section *section);
    virtual quint16 sectionMaxSize();
    virtual bool hasCRC();

private:
    SdtPidHandlerPrivate *d;
};

#endif // SDTPIDHANDLER_H
