#include "scanprocedure.h"
#include "scanproceduredvbt.h"
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


class ScanProcedureDvbtPrivate
{
public:

    ScanProcedureDvbtPrivate(ScanProcedureDvbt *parent) :
        p(parent),
        startFrequency(474000),
        endFrequency(826000),
        scanStep(8000)
    {}

    ScanProcedureDvbt *p;

    // Scan setup elements
    int startFrequency; /* kHz */
    int endFrequency; /* kHz */
    int scanStep;
    int stepIndex;

    Program *param;
};

ScanProcedureDvbt::ScanProcedureDvbt(GatewayDevice *device, QObject *parent) :
    ScanProcedure(device, parent),
    d(new ScanProcedureDvbtPrivate(this))
{
    d->param = getParam();
    d->param->setSystem(DVBT);
    d->param->setBandwidth(8);
    d->param->setPid(8192);
}

ScanProcedureDvbt::ScanProcedureDvbt(int startFrequency, int endFrequency, int scanStep, int bandwidth, int stepTimeout,
                                 int patTimeout, GatewayDevice *device, QObject *parent) :
    ScanProcedure(device, parent, stepTimeout, patTimeout),
    d(new ScanProcedureDvbtPrivate(this))
{
    setStartFrequency(startFrequency);
    setEndFrequency(endFrequency);
    setScanStep(scanStep);

    d->param = getParam();
    d->param->setBandwidth(bandwidth);
    d->param->setBandwidth(8);
    d->param->setSystem(DVBT);
}

ScanProcedureDvbt::~ScanProcedureDvbt()
{
    delete d;
}

void ScanProcedureDvbt::setStartFrequency(int startFrequency)
{
    d->startFrequency = startFrequency;
}

int ScanProcedureDvbt::startFrequency() const
{
    return d->startFrequency;
}

void ScanProcedureDvbt::setEndFrequency(int endFrequency)
{
    d->endFrequency = endFrequency;
}

int ScanProcedureDvbt::endFrequency() const
{
    return d->endFrequency;
}

void ScanProcedureDvbt::setBandwidth(int bandwidth)
{
    d->param->setBandwidth(bandwidth/1000);
}

int ScanProcedureDvbt::bandwidth() const
{
    return d->param->bandwidth()*1000;
}

void ScanProcedureDvbt::setScanStep(int scanStep)
{
    d->scanStep = scanStep;
}

int ScanProcedureDvbt::scanStep() const
{
    return d->scanStep;
}

int ScanProcedureDvbt::nbSteps() const
{
    return ((d->endFrequency - d->startFrequency) / d->scanStep);
}

int ScanProcedureDvbt::getStep(bool stepForward)
{
    double freq = d->param->frequency()*1000;
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
    d->param->setFrequency(freq/1000); /* MHz */
    return (int)((100 / (double)nbSteps()) * d->stepIndex);
}
