#ifndef EPGPARSER_H
#define EPGPARSER_H

#include <QObject>

class Event;
class EventPrivate;
class EpgParserPrivate;
class EpgParser : public QObject
{
    Q_OBJECT

public:
    explicit EpgParser(QObject *parent = 0);
    ~EpgParser();

    void push(const QByteArray &packet);

public slots:
    void discontinuity();

protected slots:
    void resetIncompleteEvent();

signals:
    void newEventAvailable(Event *);

private:
    EpgParserPrivate *d;
};

#endif // EPGPARSER_H
