#ifndef PMTHANDLER_H
#define PMTHANDLER_H

#include "abstractdvbpsihandler.h"
#include "pmtelementarystream.h"

class PmtHandlerPrivate;
class PmtHandler : public AbstractDvbPsiHandler
{
    Q_OBJECT
public:
    explicit PmtHandler(quint16 programNumber, QObject *parent = 0);
    ~PmtHandler();

    quint16 programNumber() const;
    quint8 version() const;
    quint16 pcrPid() const;
    QList<PmtElementaryStream *> elementaryStreams() const;
    bool contains(PmtElementaryStream *elementaryStream) const;
    bool triggered() const;

public slots:
    void attach();
    void dettach();

private:
    PmtHandlerPrivate *d;
};

#endif // PMTHANDLER_H
