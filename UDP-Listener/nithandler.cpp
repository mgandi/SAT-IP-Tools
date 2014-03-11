#include "nithandler.h"
#include "abstractdvbpsihandler_p.h"
#include "demuxhandler.h"
#include "descriptor.h"
#include "tables/nit.h"

#include <QtDebug>

class NitHandlerPrivate
{
public:
    NitHandlerPrivate(NitHandler *parent):
        p(parent) {}

    NitHandler *p;

    NitHandler::WhichTS whichTS;
    quint16 serviceID;
};



static void dumpDescriptors(dvbpsi_descriptor_t* descriptor)
{
    while(descriptor) {
        qDebug() << "Descriptor " << qPrintable(QString("(0x%1)").arg(descriptor->i_tag, 2, 16));
//        switch (descriptor->i_tag) {
//        }
        descriptor = descriptor->p_next;
    }
}

static void callback(void* data, dvbpsi_nit_t* nit)
{
    NitHandlerPrivate *nhp = (NitHandlerPrivate *)data;
    bool dataChanged = false;

//    // If current next indicator is false then data has changed
//    dataChanged = !nit->b_current_next;

    qDebug() << "=========================================";
    qDebug() << "NIT: Network Information Table";
    qDebug() << "Version number : " << nit->i_version;
    qDebug() << "Network id     : " << nit->i_network_id;
    qDebug() << "Current next   : " << (nit->b_current_next ? "yes" : "no");
    dumpDescriptors(nit->p_first_descriptor);

    // Signal that data changed
    if (dataChanged)
        emit nhp->p->dataChanged();

    // Delete nit
    dvbpsi_nit_delete(nit);
}

NitHandler::NitHandler(const WhichTS &whichTS, const quint16 &serviceID, const AbstractDvbPsiHandler &other, DemuxHandler *demuxHandler) :
    AbstractDvbPsiHandler(other),
    d(new NitHandlerPrivate(this))
{
    connect(demuxHandler, SIGNAL(dettached()),
            this, SLOT(demuxDettached()));

    d->whichTS = whichTS;
    d->serviceID = serviceID;
}

NitHandler::~NitHandler()
{
    delete d;
}


void NitHandler::attach()
{
//    qDebug() << "=========================================";
//    qDebug() << "NIT handler attached";
    Q_ASSERT(dvbpsi_nit_attach(abstractDvbPsiHandlerPrivate()->dvbpsi, (quint8)d->whichTS, (quint16)d->serviceID, callback, (void *)d));
    AbstractDvbPsiHandler::d->attached = true;
}

void NitHandler::dettach()
{
//    qDebug() << "=========================================";
//    qDebug() << "NIT handler detached";
    dvbpsi_nit_detach(AbstractDvbPsiHandler::d->dvbpsi, (quint8)d->whichTS, (quint16)d->serviceID);
    AbstractDvbPsiHandler::d->attached = false;
}


void NitHandler::demuxDettached()
{
    AbstractDvbPsiHandler::d->attached = false;
}
