#ifndef SDTSERVICE_H
#define SDTSERVICE_H

#include <QObject>

class SdtService : public QObject
{
    Q_OBJECT
public:
    enum RunningStatus {
        Undefined         = 0x00,
        NotRunning        = 0x01,
        StartInFewSeconds = 0x02,
        Pausing           = 0x03,
        Running           = 0x04,
        ServiceOffAir     = 0x05
    };

    SdtService(QObject *parent = 0) :
        QObject(parent) {}
    quint16 serviceId;
    RunningStatus runningStatus;
    QString serviceProviderName;
    QString serviceName;
    bool scrambled;
};

#endif // SDTSERVICE_H
