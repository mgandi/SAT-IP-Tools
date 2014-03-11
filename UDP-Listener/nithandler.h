#ifndef NITHANDLER_H
#define NITHANDLER_H

#include "abstractdvbpsihandler.h"

class DemuxHandler;
class NitHandlerPrivate;
class NitHandler : public AbstractDvbPsiHandler
{
    Q_OBJECT
public:
    enum WhichTS {
        ActualNetwork = 0x40,
        OtherNetwork  = 0x41
    };

    explicit NitHandler(const WhichTS &whichTS, const quint16 &serviceID, const AbstractDvbPsiHandler &other, DemuxHandler *demuxHandler);
    ~NitHandler();

public slots:
    void attach();
    void dettach();

private slots:
    void demuxDettached();

private:
    NitHandlerPrivate *d;
};

#endif // NITHANDLER_H
