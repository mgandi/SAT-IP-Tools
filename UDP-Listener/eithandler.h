#ifndef EITHANDLER_H
#define EITHANDLER_H

#include "abstractdvbpsihandler.h"

class Event;
class DemuxHandler;
class EitHandlerPrivate;
class EitHandler : public AbstractDvbPsiHandler
{
    Q_OBJECT

public:
    explicit EitHandler(const quint8 tableID, const quint16 serviceID, const AbstractDvbPsiHandler &other, DemuxHandler *demuxHandler);
    ~EitHandler();

    QList<Event *> events() const;
    Event *popEvent();

public slots:
    void attach();
    void dettach();

signals:
    void newEventAvailable(Event *event);

private slots:
    void demuxDettached();

private:
    EitHandlerPrivate *d;
};

#endif // EITHANDLER_H
