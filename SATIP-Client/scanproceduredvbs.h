#ifndef SCANPROCEDUREDVBS_H
#define SCANPROCEDUREDVBS_H

#include "scanprocedure.h"
#include <QObject>

enum Polarity {
    HORIZ = 1,
    VERT = 2
};
Q_DECLARE_FLAGS(Polarities, Polarity)
Q_DECLARE_OPERATORS_FOR_FLAGS(Polarities)

class ScanProcedureDvbsPrivate;
class ScanProcedureDvbs : public ScanProcedure
{
    Q_OBJECT
public:
    explicit ScanProcedureDvbs(GatewayDevice *device, QObject *parent = 0);
    ~ScanProcedureDvbs();

public slots:
    void setStartFrequency(int startFrequency);
    int startFrequency() const;
    void setEndFrequency(int endFrequency);
    int endFrequency() const;
    void setScanStep(int scanStep);
    int scanStep() const;
    int nbSteps() const;

    void setSymbolRate(int symbolRate);
    int symbolRate() const;
    void setPolarity(QString polarity);
    QString polarity() const;
    void setSystem(QString system);
    QString system() const;
    void setModulation(QString modulation);
    QString modulation() const;
    void setSatPos(int satPos);
    int satPos() const;

protected slots:
    virtual int getStep(bool stepForward = true);

private:
    ScanProcedureDvbsPrivate *d;


};

#endif // SCANPROCEDUREDVBS_H
