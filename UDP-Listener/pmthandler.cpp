#include "pmthandler.h"
#include "abstractdvbpsihandler_p.h"
#include "descriptor.h"
#include "tables/pmt.h"

#include <QMap>

class PmtHandlerPrivate
{
public:
    PmtHandlerPrivate(PmtHandler *pmtHandler):
        p(pmtHandler), triggered(false) {}

    PmtHandler *p;

    quint16 programNumber;
    quint8 version;
    quint16 pcrPid;
    QMap<quint16, PmtElementaryStream *> elementaryStreams;
    bool triggered;
};



static void callback(void* data, dvbpsi_pmt_t* pmt)
{
    PmtHandlerPrivate *php = (PmtHandlerPrivate *)data;
    dvbpsi_pmt_es_t *es = pmt->p_first_es;
    bool dataChanged = false;

    // The PMT handler has been triggered
    php->triggered = true;

    // If current next indicator is false then data has changed
    dataChanged = !pmt->b_current_next;

    // Capture Version and PCR PID
    Q_ASSERT(php->programNumber == pmt->i_program_number);
    checkChanged(php->version, pmt->i_version, dataChanged);
    checkChanged(php->pcrPid, pmt->i_pcr_pid, dataChanged);

    // Capture all elementary stream mentioned in pmt
    QList<quint16> elementaryStreamsPIDs = php->elementaryStreams.keys();
    while (es) {
        if (!php->elementaryStreams.contains(es->i_pid)) {
            PmtElementaryStream *elementaryStream = new PmtElementaryStream(php->p);
            elementaryStream->pid = es->i_pid;
            elementaryStream->type = es->i_type;
            php->elementaryStreams.insert(es->i_pid, elementaryStream);
            dataChanged = true;
        }
        elementaryStreamsPIDs.removeAll(es->i_pid);
        es = es->p_next;
    }

    // Check that all the elementary streams previously discovered are still present
    if (elementaryStreamsPIDs.count()) {
        dataChanged = true;
        foreach (quint16 elementaryStreamsPID, elementaryStreamsPIDs) {
            PmtElementaryStream *elementaryStream = php->elementaryStreams.value(elementaryStreamsPID);
            delete elementaryStream;
            php->elementaryStreams.remove(elementaryStreamsPID);
        }
    }

    // Signal that data changed
    if (dataChanged)
        emit php->p->dataChanged();

    // Delete pmt
    dvbpsi_pmt_delete(pmt);
}



PmtHandler::PmtHandler(quint16 programNumber, QObject *parent) :
    AbstractDvbPsiHandler(parent),
    d(new PmtHandlerPrivate(this))
{
    d->programNumber = programNumber;
}

PmtHandler::~PmtHandler()
{
    delete d;
}

quint16 PmtHandler::programNumber() const
{
    return d->programNumber;
}

quint8 PmtHandler::version() const
{
    return d->version;
}

quint16 PmtHandler::pcrPid() const
{
    return d->pcrPid;
}

QList<PmtElementaryStream *> PmtHandler::elementaryStreams() const
{
    return d->elementaryStreams.values();
}

bool PmtHandler::contains(PmtElementaryStream *elementaryStream) const
{
    return d->elementaryStreams.values().contains(elementaryStream);
}

bool PmtHandler::triggered() const
{
    return d->triggered;
}

void PmtHandler::attach()
{
    Q_ASSERT(dvbpsi_pmt_attach(abstractDvbPsiHandlerPrivate()->dvbpsi, d->programNumber, callback, (void *)d));
    AbstractDvbPsiHandler::d->attached = true;
}

void PmtHandler::dettach()
{
    dvbpsi_pmt_detach(AbstractDvbPsiHandler::d->dvbpsi);
    AbstractDvbPsiHandler::d->attached = false;
}
