#ifndef ABSTRACTDVBPSIHANDLER_P_H
#define ABSTRACTDVBPSIHANDLER_P_H

#include <QtGlobal>

#include "dvbpsi.h"
#include "abstractdvbpsihandler.h"

#define checkChanged(a, b, c) if (a != b) { a = b; c = true; }

class AbstractDvbPsiHandlerPrivate
{
public:
    AbstractDvbPsiHandlerPrivate(AbstractDvbPsiHandler::MessageLevel messageLevel, dvbpsi_t *handle = 0);

    dvbpsi_t *dvbpsi;
    bool attached;
    AbstractDvbPsiHandler::MessageLevel msgLvl;
};

#endif // ABSTRACTDVBPSIHANDLER_P_H
