#include "pathandler.h"
#include "abstractdvbpsihandler_p.h"

//#include "demux.h"
//#include "psi.h"
//#include "descriptor.h"
#include "tables/pat.h"
//#include "tables/pmt.h"
//#include "tables/cat.h"
//#include "tables/bat.h"
//#include "tables/eit.h"
//#include "tables/nit.h"
//#include "tables/sdt.h"
//#include "tables/tot.h"
//#include "tables/rst.h"
//#include "descriptors/dr.h"

#include <QMap>

class PatHandlerPrivate
{
public:
    PatHandlerPrivate(PatHandler *patHandler):
        p(patHandler) {}

    PatHandler *p;

    quint16 tsId;
    quint8  version;
    QMap<quint16, PatProgram> programs;
};


static void callback(void* data, dvbpsi_pat_t* pat)
{
    PatHandlerPrivate *php = (PatHandlerPrivate *)data;
    dvbpsi_pat_program_t* program = pat->p_first_program;
    bool dataChanged = false;

    // If current next indicator is false then data has changed
    dataChanged = !pat->b_current_next;

    // Capture TS ID and Version
    checkChanged(php->tsId, pat->i_ts_id, dataChanged);
    checkChanged(php->version, pat->i_version, dataChanged);

    // Capture all programs mentioned in pat
    QList<quint16> programNumbers = php->programs.keys();
    while (program) {
        if (!php->programs.contains(program->i_number)) {
            PatProgram prgm;
            prgm.number = program->i_number;
            prgm.pid = program->i_pid;
            php->programs.insert(program->i_number, prgm);
            dataChanged = true;
        }
        programNumbers.removeAll(program->i_number);
        program = program->p_next;
    }

    // Check that all the programs previously discovered are still present
    if (programNumbers.count()) {
        dataChanged = true;
        foreach (quint16 programNumber, programNumbers) {
            php->programs.remove(programNumber);
        }
    }

    // Signal that data changed
    if (dataChanged)
        emit php->p->dataChanged();

    // Delete pat
    dvbpsi_pat_delete(pat);
}


PatHandler::PatHandler(QObject *parent) :
    AbstractDvbPsiHandler(parent),
    d(new PatHandlerPrivate(this))
{
}

PatHandler::~PatHandler()
{
    delete d;
}


quint16 PatHandler::tsId() const
{
    return d->tsId;
}

quint8 PatHandler::version() const
{
    return d->version;
}

QList<PatProgram> PatHandler::programs() const
{
    return d->programs.values();
}

int PatHandler::indexOf(quint16 programNumber) const
{
    return d->programs.keys().indexOf(programNumber);
}


void PatHandler::attach()
{
    Q_ASSERT(dvbpsi_pat_attach(abstractDvbPsiHandlerPrivate()->dvbpsi, callback, (void *)d));
    AbstractDvbPsiHandler::d->attached = true;
}

void PatHandler::clear()
{
    d->tsId = 0;
    d->version = 0;
    d->programs.clear();
}

void PatHandler::dettach()
{
    dvbpsi_pat_detach(AbstractDvbPsiHandler::d->dvbpsi);
    AbstractDvbPsiHandler::d->attached = false;
}
