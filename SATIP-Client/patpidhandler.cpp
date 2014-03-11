#include "patpidhandler.h"
#include "tableparser_p.h"
#include "section_p.h"

#include <QtDebug>
#include <QtEndian>
#include <QElapsedTimer>

class PatPidHandlerPrivate
{
public:
    PatPidHandlerPrivate() :
        originalNetworkID(0),
        versionNumber(0)
    {}

    quint16 originalNetworkID;
    quint8 versionNumber;
    QMap<quint16, PatProgram> programs;
    QElapsedTimer patTimer;
};

PatPidHandler::PatPidHandler(bool a, QObject *parent) :
    AbstractPidHandler(a, parent),
    d(new PatPidHandlerPrivate)
{
    //d->patTimer.start();
}

PatPidHandler::~PatPidHandler()
{
    delete d;
}

quint16 PatPidHandler::tsId() const
{
    return d->originalNetworkID;
}

quint8 PatPidHandler::version() const
{
    return d->versionNumber;
}

QList<PatProgram> PatPidHandler::programs() const
{
    return d->programs.values();
}

int PatPidHandler::indexOf(quint16 programNumber) const
{
    return d->programs.keys().indexOf(programNumber);
}

void PatPidHandler::parseTable(Section *section)
{
    if (section->tableID != 0x0)
        return;

    quint16 available = section->payloadSize;
    quint16 index = 0;
    bool changed = false;

    // If current next indicator is false then data has changed
    changed = !section->currentNextIndicator;

    //qDebug() << "PAT received at " << d->patTimer.elapsed() << "ms";

    // Capture and check Version
    checkChanged(d->versionNumber, section->versionNumber, changed);

    // Capture all programs mentioned in pat
    QList<quint16> programNumbers = d->programs.keys();
    while (available > 4) {
        quint16 programNumber = qToBigEndian(*((quint16 *)&section->payload[index]));
        if (!programNumber) {
            d->originalNetworkID = (((quint16)section->payload[index + 2] & 0x1F) << 8) | ((quint16)section->payload[index + 3] & 0xFF);
//            qDebug() << "Network ID:" << qPrintable(QString("%1").arg(d->originalNetworkID, 4, 10, QLatin1Char(' ')));
        } else {
            quint16 programMapPid = (((quint16)section->payload[index + 2] & 0x1F) << 8) | ((quint16)section->payload[index + 3] & 0xFF);
            if (!d->programs.contains(programNumber)) {
                PatProgram prgm;
                prgm.number = programNumber;
                prgm.pid = programMapPid;
                d->programs.insert(programNumber, prgm);
//                qDebug() << "Program:" << qPrintable(QString("%1").arg(programNumber, 4, 16, QLatin1Char('0'))) << "@" << qPrintable(QString("%1").arg(programMapPid, 4, 10, QLatin1Char(' ')));
            }
            programNumbers.removeAll(programNumber);
        }
        index += 4;
        available -= 4;
    }

//    // Capture and check Version
//    checkChanged(d->originalNetworkID, parser->, changed);

    // Check that all the programs previously discovered are still present
    if (programNumbers.count()) {
        changed = true;
        foreach (quint16 programNumber, programNumbers) {
            d->programs.remove(programNumber);
        }
    }

    // Signal that data changed
    if (changed)
        emit dataChanged();
}

quint16 PatPidHandler::sectionMaxSize()
{
    return 1024;
}

bool PatPidHandler::hasCRC()
{
    return false;
    return true;
}

void PatPidHandler::clear()
{
    d->originalNetworkID = 0;
    d->versionNumber = 0;
    d->programs.clear();
}
