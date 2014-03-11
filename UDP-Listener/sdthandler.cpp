#include "sdthandler.h"
#include "abstractdvbpsihandler_p.h"
#include "demuxhandler.h"
#include "descriptor.h"
#include "tables/sdt.h"
#include "descriptors/dr_48.h"

#include <QMap>
#include <QtDebug>



class SdtHandlerPrivate
{
public:
    SdtHandlerPrivate(SdtHandler *parent):
        p(parent) {}

    SdtHandler *p;

    SdtHandler::WhichTS whichTS;
    quint16 transportStreamID;
    quint16 networkId;
    quint8 version;
    QMap<quint16, SdtService *> services;
};



static void dumpDescriptors(SdtService *service, dvbpsi_descriptor_t* descriptor)
{
    while(descriptor) {
        switch (descriptor->i_tag) {

        case 0x48: {
            dvbpsi_service_dr_t * serviceDescritor = dvbpsi_DecodeServiceDr(descriptor);
            Q_ASSERT(serviceDescritor);
            service->serviceProviderName = QString(QByteArray((const char *)serviceDescritor->i_service_provider_name, (int)serviceDescritor->i_service_provider_name_length));
            service->serviceName = QString(QByteArray((const char *)serviceDescritor->i_service_name, (int)serviceDescritor->i_service_name_length));
            break;
        }

        }
        descriptor = descriptor->p_next;
    }
}

static void callback(void* data, dvbpsi_sdt_t* sdt)
{
    SdtHandlerPrivate *php = (SdtHandlerPrivate *)data;
    dvbpsi_sdt_service_t* service = sdt->p_first_service;
    bool dataChanged = false;

    // If current next indicator is false then data has changed
    dataChanged = !sdt->b_current_next;

    // Capture Version, Table ID, Transport stream ID and Network ID
    Q_ASSERT(php->transportStreamID == sdt->i_extension);
    Q_ASSERT(php->whichTS == (SdtHandler::WhichTS)sdt->i_table_id);
    checkChanged(php->version, sdt->i_version, dataChanged);
    checkChanged(php->networkId, sdt->i_network_id, dataChanged);

    // Capture data about all services mentioned in SDT
    QList<quint16> serviceIDs = php->services.keys();
    while (service) {
        if (!php->services.contains(service->i_service_id)) {
            SdtService *s = new SdtService(php->p);
            s->serviceId = service->i_service_id;
            s->runningStatus = (SdtService::RunningStatus)service->i_running_status;
            dumpDescriptors(s, service->p_first_descriptor);
            php->services.insert(service->i_service_id, s);
            dataChanged = true;
        }
        serviceIDs.removeAll(service->i_service_id);
        service = service->p_next;
    }

//    // Check that all the serivces previously mentioned are still present
//    if (elementaryStreamsPIDs.count()) {
//        dataChanged = true;
//        foreach (quint16 elementaryStreamsPID, elementaryStreamsPIDs) {
//            PmtElementaryStream *elementaryStream = php->elementaryStreams.value(elementaryStreamsPID);
//            delete elementaryStream;
//            php->elementaryStreams.remove(elementaryStreamsPID);
//        }
//    }

    // Signal that data changed
    if (dataChanged)
        emit php->p->dataChanged();

    // Delete pmt
    dvbpsi_sdt_delete(sdt);
}



SdtHandler::SdtHandler(const SdtHandler::WhichTS &whichTS, const quint16 &transportStreamID, const AbstractDvbPsiHandler &other, DemuxHandler *demuxHandler) :
    AbstractDvbPsiHandler(other),
    d(new SdtHandlerPrivate(this))
{
    connect(demuxHandler, SIGNAL(dettached()),
            this, SLOT(demuxDettached()));
    d->whichTS = whichTS;
    d->transportStreamID = transportStreamID;
}

SdtHandler::~SdtHandler()
{
    delete d;
}

SdtHandler::WhichTS SdtHandler::whichTS() const
{
    return d->whichTS;
}

quint16 SdtHandler::transportStreamID() const
{
    return d->transportStreamID;
}

quint16 SdtHandler::networkID() const
{
    return d->networkId;
}

quint8 SdtHandler::version() const
{
    return d->version;
}

QMap<quint16, SdtService *> *SdtHandler::services() const
{
    return &d->services;
}


void SdtHandler::attach()
{
    Q_ASSERT(dvbpsi_sdt_attach(abstractDvbPsiHandlerPrivate()->dvbpsi, (quint8)d->whichTS, d->transportStreamID, callback, (void *)d));
    AbstractDvbPsiHandler::d->attached = true;
}

void SdtHandler::dettach()
{
    dvbpsi_sdt_detach(AbstractDvbPsiHandler::d->dvbpsi, (quint8)d->whichTS, d->transportStreamID);
    AbstractDvbPsiHandler::d->attached = false;
}

void SdtHandler::demuxDettached()
{
    AbstractDvbPsiHandler::d->attached = false;
}
