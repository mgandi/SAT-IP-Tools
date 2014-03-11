#include "pmtpidhandler.h"
#include "tableparser_p.h"
#include "section_p.h"

#include <QMap>
#include <QtDebug>

class PmtPidHandlerPrivate
{
public:
    PmtPidHandlerPrivate(quint16 pn) :
        programNumber(pn),
        versionNumber(0),
        triggered(false)
    {}

    quint16 programNumber;
    quint8 versionNumber;
    quint16 pcrPid;
    quint16 programInfoLength;
    QMap<quint16, PmtElementaryStream *> elementaryStreams;
    bool triggered;
};

PmtPidHandler::PmtPidHandler(quint16 programNumber, bool a, QObject *parent) :
    AbstractPidHandler(a, parent),
    d(new PmtPidHandlerPrivate(programNumber))
{
}

PmtPidHandler::~PmtPidHandler()
{
    delete d;
}

quint16 PmtPidHandler::programNumber() const
{
    return d->programNumber;
}

quint8 PmtPidHandler::version() const
{
    return d->versionNumber;
}

quint16 PmtPidHandler::pcrPid() const
{
    return d->pcrPid;
}

QList<PmtElementaryStream *> PmtPidHandler::elementaryStreams() const
{
    return d->elementaryStreams.values();
}

bool PmtPidHandler::contains(PmtElementaryStream *elementaryStream) const
{
    return d->elementaryStreams.values().contains(elementaryStream);
}

bool PmtPidHandler::triggered() const
{
    return d->triggered;
}

void PmtPidHandler::parseTable(Section *section)
{
    if (section->tableID != 0x2)
        return;

    quint16 available = section->payloadSize;
    quint16 index = 0;
    bool changed = false;

    // The PMT handler has been triggered
    d->triggered = true;

    // If current next indicator is false then data has changed
    changed = !section->currentNextIndicator;

    // Check PMT PID
    Q_ASSERT(d->programNumber == section->serviceID);

    // Capture and check Version
    checkChanged(d->versionNumber, section->versionNumber, changed);

    // Capture PCR PID and program info length
    d->pcrPid = (((quint16)section->payload[0] & 0x1F) << 8) | ((quint16)section->payload[1] & 0xFF);
    d->programInfoLength = (((quint16)section->payload[2] & 0x0F) << 8) | ((quint16)section->payload[3] & 0xFF);

    // Check for descriptors
    if (d->programInfoLength) {
    }

//    qDebug() << "=========================================";
//    qDebug() << "Parsing PMT of program" << d->programNumber;

    // Increment bytes left counter and index
    index += d->programInfoLength + 4;
    available -= d->programInfoLength + 4;

    // Capture all elementary stream mentioned in pmt
    QList<quint16> elementaryStreamsPIDs = d->elementaryStreams.keys();
    while (available > 4) {
//        qDebug() << "PMT: Bytes left" << available;
        quint8 type = section->payload[index];
        quint16 pid = (((quint16)section->payload[index + 1] & 0x1F) << 8) | ((quint16)section->payload[index + 2] & 0xFF);
        quint16 esInfoLength = (((quint16)section->payload[index + 3] & 0x0F) << 8) | ((quint16)section->payload[index + 4] & 0xFF);

        // Check for descriptors
        if (esInfoLength) {
        }

        if (esInfoLength + 5 > available) {
            qFatal("This should never happen!");
        }

        // Increment bytes left counter and index
        index += esInfoLength + 5;
        available -= esInfoLength + 5;

        if (!d->elementaryStreams.contains(pid)) {
            PmtElementaryStream *elementaryStream = new PmtElementaryStream(this);
            elementaryStream->pid = pid;
            elementaryStream->type = type;
            d->elementaryStreams.insert(pid, elementaryStream);
            changed = true;
        }
        elementaryStreamsPIDs.removeAll(pid);
    }

    // Check that all the elementary streams previously discovered are still present
    if (elementaryStreamsPIDs.count()) {
        changed = true;
        foreach (quint16 elementaryStreamsPID, elementaryStreamsPIDs) {
            PmtElementaryStream *elementaryStream = d->elementaryStreams.value(elementaryStreamsPID);
            delete elementaryStream;
            d->elementaryStreams.remove(elementaryStreamsPID);
        }
    }

    // Signal that data changed
    if (changed)
        emit dataChanged();
}

quint16 PmtPidHandler::sectionMaxSize()
{
    return 1024;
}

bool PmtPidHandler::hasCRC()
{
    return false;
    return true;
}
