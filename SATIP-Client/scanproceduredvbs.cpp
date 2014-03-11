
#include "scanprocedure.h"
#include "scanproceduredvbs.h"
#include "rtpsocket.h"
#include "rtspsession.h"
#include "gatewaydevice.h"
#include "elementarystream.h"
#include "program.h"
#include "programmodel.h"

#include "demuxer.h"
#include "patpidhandler.h"
#include "pmtpidhandler.h"
#include "sdtpidhandler.h"

#include <QUrl>
#include <QUrlQuery>
#include <QHash>
#include <QFlags>
#include <QCoreApplication>
#include <QTimerEvent>

class ScanProcedureDvbsPrivate
{
public:

    ScanProcedureDvbsPrivate(ScanProcedureDvbs *parent) :
        p(parent),
        startFrequency(11361),
        endFrequency(12800),
        scanStep(3),
        symbolRate(22000),
        polarities(HORIZ),
        modulations(M_8PSK),
        systems(DVBS2)
    {}

    ScanProcedureDvbs *p;
    Program *param;

    int startFrequency;
    int endFrequency;
    int scanStep;
    int stepIndex;
    int symbolRate;
    Polarities polarities;
    Modulations modulations;
    Systems systems;
};

ScanProcedureDvbs::ScanProcedureDvbs(GatewayDevice *device, QObject *parent) :
    ScanProcedure(device, parent),
    d(new ScanProcedureDvbsPrivate(this))
{
    d->param = getParam();
    d->param->setSystem(DVBS2);
    d->param->setModulation(M_8PSK);
    d->param->setVPolarization(0);
    d->param->setSatPos(1);
}

ScanProcedureDvbs::~ScanProcedureDvbs()
{
    delete d;
}

void ScanProcedureDvbs::setStartFrequency(int startFrequency)
{
    d->startFrequency = startFrequency;
}

int ScanProcedureDvbs::startFrequency() const
{
    return d->startFrequency;
}

void ScanProcedureDvbs::setEndFrequency(int endFrequency)
{
    d->endFrequency = endFrequency;
}

int ScanProcedureDvbs::endFrequency() const
{
    return d->endFrequency;
}

void ScanProcedureDvbs::setSymbolRate(int symbolRate)
{
    d->symbolRate = symbolRate;
}

int ScanProcedureDvbs::symbolRate() const
{
    return d->symbolRate;
}

void ScanProcedureDvbs::setPolarity(QString polarity)
{
    if (polarity == "Auto")
        d->polarities = HORIZ | VERT;
    else if (polarity == "Horiz")
        d->polarities = HORIZ;
    else if (polarity == "Vert")
        d->polarities = VERT;
}

QString ScanProcedureDvbs::polarity() const
{
    if (d->polarities & (HORIZ | VERT))
        return "Auto";
    else if (d->polarities & (HORIZ))
        return "Horiz";
    else if (d->polarities & (VERT))
        return "Vert";
    return "";
}

void ScanProcedureDvbs::setSystem(QString system)
{
    if (system == "Auto")
        d->systems = DVBS | DVBS2;
    else if (system == "DVB-S")
        d->systems = DVBS;
    else if (system == "DVB-S2")
        d->systems = DVBS2;
}

QString ScanProcedureDvbs::system() const
{
    if (d->systems & (DVBS | DVBS2))
        return "Auto";
    else if (d->systems & (DVBS))
        return "DVB-S";
    else if (d->systems & (DVBS2))
        return "DVB-S2";
    return "";
}

void ScanProcedureDvbs::setModulation(QString modulation)
{
    if (modulation == "Auto")
        d->modulations = M_QPSK | M_8PSK;
    else if (modulation == "Q-PSK")
        d->modulations = M_QPSK;
    else if (modulation == "8-PSK")
        d->modulations = M_8PSK;
}

QString ScanProcedureDvbs::modulation() const
{
    if (d->modulations & (M_QPSK | M_8PSK))
        return "Auto";
    else if (d->modulations & (M_QPSK))
        return "Q-PSK";
    else if (d->modulations & (M_8PSK))
        return "8-PSK";
    return "";
}

void ScanProcedureDvbs::setSatPos(int satPos)
{
    d->param->setSatPos(satPos);
}

int ScanProcedureDvbs::satPos() const
{
    return d->param->satPos();
}
void ScanProcedureDvbs::setScanStep(int scanStep)
{
    d->scanStep = scanStep;
}

int ScanProcedureDvbs::scanStep() const
{
    return d->scanStep;
}

int ScanProcedureDvbs::nbSteps() const
{
    return ((d->endFrequency - d->startFrequency) / d->scanStep);
}

int ScanProcedureDvbs::getStep(bool stepForward)
{
    double freq = d->param->frequency();
    if (freq) {
        if (stepForward) {
             // Increment current frequency value
            freq += d->scanStep;
            d->stepIndex++;
        } else {
            if (freq <= d->startFrequency) {
                // Set current frequency value to start frequency
                freq = d->startFrequency;
                d->stepIndex = 0;
            } else {
                // Decrement current frequency value
                freq -= d->scanStep;
                d->stepIndex--;
            }
        }
    } else {
        // Set current frequency value to start frequency
        freq = d->startFrequency;
        d->stepIndex = 0;
    }
    d->param->setFrequency(freq); /* MHz */

    if (d->systems & DVBS)
        d->param->setSystem(DVBS);
    else if (d->systems & DVBS2)
        d->param->setSystem(DVBS2);

    if (d->modulations & M_QPSK)
        d->param->setModulation(M_QPSK);
    else if (d->modulations & M_8PSK)
        d->param->setModulation(M_8PSK);

    d->param->setSymbolRate(d->symbolRate);

    if (d->polarities & HORIZ)
        d->param->setVPolarization(0);
    else
        d->param->setVPolarization(1);

    return (int)((100 / (double)nbSteps()) * d->stepIndex);
}
