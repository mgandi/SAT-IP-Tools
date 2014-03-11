#include "sdtpidhandler.h"
#include "tableparser_p.h"
#include "section_p.h"

#include <QMap>
#include <QtDebug>

class SdtPidHandlerPrivate
{
public:
    SdtPidHandlerPrivate(const SdtPidHandler::WhichTS &wts) :
        whichTS(wts)
    {}

    SdtPidHandler::WhichTS whichTS;
    quint16 originalNetworkID;
    quint16 transportStreamID;
    quint16 networkId;
    quint8 versionNumber;
    QMap<quint16, SdtService *> services;
};

SdtPidHandler::SdtPidHandler(const WhichTS &whichTS, bool a, QObject *parent) :
    AbstractPidHandler(a, parent),
    d(new SdtPidHandlerPrivate(whichTS))
{
}

SdtPidHandler::~SdtPidHandler()
{
    delete d;
}


SdtPidHandler::WhichTS SdtPidHandler::whichTS() const
{
    return d->whichTS;
}

quint16 SdtPidHandler::transportStreamID() const
{
    return d->transportStreamID;
}

quint16 SdtPidHandler::networkID() const
{
    return d->networkId;
}

quint8 SdtPidHandler::version() const
{
    return d->versionNumber;
}

QMap<quint16, SdtService *> *SdtPidHandler::services() const
{
    return &d->services;
}

void SdtPidHandler::dumpDescriptor(SdtService *service, const quint8 payload[], const quint16 payloadSize)
{
    quint16 index = 0;

    while (index < payloadSize) {
        quint8 tag = payload[index++];
        quint8 length = payload[index++];
        switch (tag) {
        case 0x48:
            quint8 type = payload[index++];
            quint8 serviceProviderNameLength = payload[index++];
            const quint8 *serviceProviderName = &payload[index];
            index += serviceProviderNameLength;
            service->serviceProviderName = QString(QByteArray((const char *)serviceProviderName, (int)serviceProviderNameLength));
            quint8 serviceNamelength = payload[index++];
            const quint8 *serviceName = &payload[index];
            index += serviceNamelength;
            service->serviceName = QString(QByteArray((const char *)serviceName, (int)serviceNamelength));
            break;
        }
    }
}

void SdtPidHandler::parseTable(Section *section)
{
    if (section->tableID != d->whichTS)
        return;

    quint16 available = section->payloadSize;
    quint16 index = 0;
    bool changed = false;

    // If current next indicator is false then data has changed
    changed = !section->currentNextIndicator;

//    // Check PMT PID
//    Q_ASSERT(d->programNumber == parser->serviceID);

    // Capture and check Version
    checkChanged(d->versionNumber, section->versionNumber, changed);

    // Capture original network ID
    d->originalNetworkID = (((quint16)section->payload[0] & 0xFF) << 8) | ((quint16)section->payload[1] & 0xFF);
    index += 3;
    available -= 3;

//    qDebug() << "=========================================";
//    qDebug() << "Parsing SDT of network ID" << d->originalNetworkID;

    // Capture data about all services mentioned in SDT
    QList<quint16> serviceIDs = d->services.keys();
    while (available > 4) {
//        qDebug() << "SDT: Bytes left" << available;
        quint16 serviceID = (((quint16)section->payload[index] & 0xFF) << 8) |
                ((quint16)section->payload[index + 1] & 0xFF);
        quint8 eitScheduleFlag = (section->payload[index + 2] & 0x02) >> 1;
        quint8 eitPresentFollowingFlag = (section->payload[index + 2] & 0x01);
        quint8 runningStatus = (section->payload[index + 3] & 0xE0) >> 5;
        quint8 freeCAMode = (section->payload[index + 3] & 0x10) >> 4;
        quint16 descriptorLoopLength = (((quint16)section->payload[index + 3] & 0x0F) << 8) |
                ((quint16)section->payload[index + 4] & 0xFF);

        // Increment bytes left counter and index
        index += 5;
        available -= 5;

        // Check that descriptor loop length is not greater than the number of bytes left
        if (descriptorLoopLength > available) {
            qFatal("This is not yet implemented!");
        }

        // Create corresponding SDT Service
        if (!d->services.contains(serviceID)) {
            SdtService *service = new SdtService(this);
            service->serviceId = serviceID;
            service->runningStatus = (SdtService::RunningStatus)runningStatus;
            service->scrambled = (freeCAMode != 0);
            if (descriptorLoopLength)
                dumpDescriptor(service, (const quint8 *)&section->payload[index], descriptorLoopLength);
            d->services.insert(serviceID, service);
            changed = true;
        }
        serviceIDs.removeAll(serviceID);

        // Incerment bytes left counter and index
        index += descriptorLoopLength;
        available -= descriptorLoopLength;
    }

    // Signal that data changed
    if (changed)
        emit dataChanged();
}

quint16 SdtPidHandler::sectionMaxSize()
{
    return 1024;
}

bool SdtPidHandler::hasCRC()
{
    return false;
    return true;
}
