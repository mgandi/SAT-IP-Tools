#ifndef SCANPROCEDUREDVBT_H
#define SCANPROCEDUREDVBT_H

#include "scanprocedure.h"
#include <QObject>


class ScanProcedureDvbtPrivate;
class ScanProcedureDvbt : public ScanProcedure
{
    Q_OBJECT
public:

    explicit ScanProcedureDvbt(GatewayDevice *device, QObject *parent = 0);
    explicit ScanProcedureDvbt(int startFrequency, int endFrequency, int scanStep, int bandwidth, int stepTimeout,
                               int patTimeout, GatewayDevice *device, QObject *parent = 0);
    ~ScanProcedureDvbt();

public slots:
    void setStartFrequency(int startFrequency);
    int startFrequency() const;
    void setEndFrequency(int endFrequency);
    int endFrequency() const;
    void setScanStep(int scanStep);
    int scanStep() const;
    int nbSteps() const;
    void setBandwidth(int bandwidth);
    int bandwidth() const;

protected slots:
    virtual int getStep(bool stepForward = true);

private:
    ScanProcedureDvbtPrivate *d;
};

#endif // SCANPROCEDUREDVBT_H
