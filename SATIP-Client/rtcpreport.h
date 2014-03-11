#ifndef RTCPREPORT_H
#define RTCPREPORT_H

#include <QByteArray>

class RTCPReportPrivate;
class RTCPReport
{
public:
    explicit RTCPReport();
    explicit RTCPReport(const QByteArray &report);
    explicit RTCPReport(const RTCPReport &other);
    ~RTCPReport();

    quint32 packetCount() const;

    quint32 ssrc() const;
    quint32 feID() const;
    quint32 signalLevel() const;
    bool locked() const;
    quint32 signalQuality() const;
    double frequency() const;

    bool operator ==(const RTCPReport &other) const;
    RTCPReport &operator =(const RTCPReport &other);

protected:
    void parse(const QByteArray &report);
    bool next();
    bool isSR();
    bool isAPP();

private:
    RTCPReportPrivate *d;
};

#endif // RTCPREPORT_H
