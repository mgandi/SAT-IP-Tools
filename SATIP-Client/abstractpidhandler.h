#ifndef ABSTRACTPIDHANDLER_H
#define ABSTRACTPIDHANDLER_H

#include <QObject>

#ifndef checkChanged
#define checkChanged(a, b, c) if (a != b) { a = b; c = true; }
#endif // checkChanged

class Section;
class AbstractPidHandlerPrivate;
class AbstractPidHandler : public QObject
{
    Q_OBJECT
public:
    explicit AbstractPidHandler(bool attach = true, QObject *parent = 0);
    ~AbstractPidHandler();

    void push(const QByteArray &packet);
    void attach();
    void dettach();
    bool isAttached();

protected:
    bool checkContinuity(const QByteArray &packet);
    void parse(const QByteArray &packet);
    virtual void parseTable(Section *section) = 0;
    virtual quint16 sectionMaxSize() = 0;
    virtual bool hasCRC() = 0;
    bool checkCRC();

protected slots:
    void deleteCurrentSection();
    void tableComplete();

signals:
    void discontinuity();
    void dataChanged();

private:
    AbstractPidHandlerPrivate *d;
};

#endif // ABSTRACTPIDHANDLER_H
