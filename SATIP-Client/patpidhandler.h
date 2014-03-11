#ifndef PATPIDHANDLER_H
#define PATPIDHANDLER_H

#include "abstractpidhandler.h"
#include "patprogram.h"

class PatPidHandlerPrivate;
class PatPidHandler : public AbstractPidHandler
{
    Q_OBJECT

public:
    explicit PatPidHandler(bool attach = true, QObject *parent = 0);
    ~PatPidHandler();

    quint16 tsId() const;
    quint8 version() const;
    QList<PatProgram> programs() const;
    int indexOf(quint16 programNumber) const;

protected slots:
    void parseTable(Section *section);
    virtual quint16 sectionMaxSize();
    virtual bool hasCRC();
    void clear();

private:
    PatPidHandlerPrivate *d;
};

#endif // PATPIDHANDLER_H
