#ifndef PATHANDLER_H
#define PATHANDLER_H

#include "abstractdvbpsihandler.h"
#include "patprogram.h"

class PatHandlerPrivate;
class PatHandler : public AbstractDvbPsiHandler
{
    Q_OBJECT
public:
    explicit PatHandler(QObject *parent = 0);
    ~PatHandler();

    quint16 tsId() const;
    quint8 version() const;
    QList<PatProgram> programs() const;
    int indexOf(quint16 programNumber) const;

public slots:
    void attach();
    void clear();
    void dettach();

private:
    PatHandlerPrivate *d;
};

#endif // PATHANDLER_H
