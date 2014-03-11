#ifndef DEMUXHANDLER_H
#define DEMUXHANDLER_H

#include "abstractdvbpsihandler.h"

class DemuxHandlerPrivate;
class DemuxHandler : public AbstractDvbPsiHandler
{
    Q_OBJECT
public:
    explicit DemuxHandler(QObject *parent = 0);
    ~DemuxHandler();

public slots:
    void attach();
    void clear();
    void dettach();

signals:
    void newHandler(AbstractDvbPsiHandler *handler);
    void dettached();

private:
    DemuxHandlerPrivate *d;
};

#endif // DEMUXHANDLER_H
