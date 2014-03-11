#include "abstractdvbpsihandler_p.h"

AbstractDvbPsiHandlerPrivate::AbstractDvbPsiHandlerPrivate(AbstractDvbPsiHandler::MessageLevel messageLevel, dvbpsi_t *handle):
    dvbpsi(handle),
    attached(false),
    msgLvl(messageLevel)
{
}
