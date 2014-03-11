#ifndef PMTPIDHANDLER_H
#define PMTPIDHANDLER_H

#include "abstractpidhandler.h"
#include "pmtelementarystream.h"

class PmtPidHandlerPrivate;
class PmtPidHandler : public AbstractPidHandler
{
    Q_OBJECT

public:
    explicit PmtPidHandler(quint16 programNumber, bool attach = true, QObject *parent = 0);
    ~PmtPidHandler();

    quint16 programNumber() const;
    quint8 version() const;
    quint16 pcrPid() const;
    QList<PmtElementaryStream *> elementaryStreams() const;
    bool contains(PmtElementaryStream *elementaryStream) const;
    bool triggered() const;

protected slots:
    void parseTable(Section *section);
    virtual quint16 sectionMaxSize();
    virtual bool hasCRC();

private:
    PmtPidHandlerPrivate *d;
};

#endif // PMTPIDHANDLER_H
