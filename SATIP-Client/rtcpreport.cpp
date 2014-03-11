#include "rtcpreport.h"

#include <QString>
#include <QStringList>
#include <QtEndian>

#include <QDebug>

enum RtcpType {
    RTCP_SR   = 200,
    RTCP_RR   = 201,
    RTCP_SDES = 202,
    RTCP_BYE  = 203,
    RTCP_APP  = 204
};

enum RtcpSdesType {
    RTCP_SDES_END   = 0,
    RTCP_SDES_CNAME = 1,
    RTCP_SDES_NAME  = 2,
    RTCP_SDES_EMAIL = 3,
    RTCP_SDES_PHONE = 4,
    RTCP_SDES_LOC   = 5,
    RTCP_SDES_TOOL  = 6,
    RTCP_SDES_NOTE  = 7,
    RTCP_SDES_PRIV  = 8,
    RTCP_SDES_MAX   = 9
};

struct RtcpCommonHeader {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    quint8 type;
    quint8 subtype:5;
    quint8 padbit:1;
    quint8 version:2;
#else
    quint8 version:2;
    quint8 padbit:1;
    quint8 subtype:5;
    quint8 type;
#endif
    quint16 length;

    quint16 getLength() {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        return length;
#else
        return qFromBigEndian(length);
#endif
    }
};

struct SenderInfo
{
    quint32 ntp_timestamp_msw;
    quint32 ntp_timestamp_lsw;
    quint32 rtp_timestamp;
    quint32 senders_packet_count;
    quint32 senders_octet_count;

    quint16 getPacketCount() {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        return senders_packet_count;
#else
        return qFromBigEndian(senders_packet_count);
#endif
    }
};

struct RtcpSR {
    RtcpCommonHeader header;
    quint32 ssrc;
    SenderInfo si;
};

struct RtcpChunkItem {
    quint8 type;
    quint8 length;
};

struct RtcpChunk {
    quint32 ssrc;
};

struct RtcpSDES {
    RtcpChunk chunk;
    RtcpChunkItem item;
};

struct RtcpAPP {
    RtcpCommonHeader header;
    quint32 ssrc;
    char name[4];
    quint16 identifier;
    quint16 string_length;
    char string[];

    quint16 getStringLength() {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        return string_length;
#else
        return qFromBigEndian(string_length);
#endif
    }

    quint16 getIdentifier() {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        return identifier;
#else
        return qFromBigEndian(identifier);
#endif
    }
};

struct SatipElement
{
    QString name;
    QStringList values;
};



class RTCPReportPrivate
{
public:
    quint32 packetCount;

    quint32 ssrc;
    quint32 feID;
    quint32 signalLevel;
    bool locked;
    quint32 signalQuality;
    double frequency;

    // Parsing variables
    char *ptr;
    quint32 rb;
    QByteArray rtcpPacket;
};

RTCPReport::RTCPReport() :
    d(new RTCPReportPrivate)
{
    d->packetCount = 0;

    d->ssrc = 0;
    d->feID = 0;
    d->signalLevel = 0;
    d->locked = false;
    d->signalQuality = 0;
    d->frequency = 0;
}

RTCPReport::RTCPReport(const QByteArray &report) :
    d(new RTCPReportPrivate)
{
    parse(report);
}

RTCPReport::RTCPReport(const RTCPReport &other) :
    d(new RTCPReportPrivate)
{
    d->packetCount = other.packetCount();
    d->ssrc = other.ssrc();
    d->feID = other.feID();
    d->signalLevel = other.signalLevel();
    d->locked = other.locked();
    d->signalQuality = other.signalQuality();
    d->frequency = other.frequency();
}

RTCPReport::~RTCPReport()
{
    delete d;
}

quint32 RTCPReport::packetCount() const
{
    return d->packetCount;
}

quint32 RTCPReport::ssrc() const
{
    return d->ssrc;
}

quint32 RTCPReport::feID() const
{
    return d->feID;
}

quint32 RTCPReport::signalLevel() const
{
    return d->signalLevel;
}

bool RTCPReport::locked() const
{
    return d->locked;
}

quint32 RTCPReport::signalQuality() const
{
    return d->signalQuality;
}

double RTCPReport::frequency() const
{
    return d->frequency;
}

bool RTCPReport::operator ==(const RTCPReport &other) const
{
    if (ssrc() != other.ssrc())
        return false;
    if (feID() != other.feID())
        return false;
    if (signalLevel() != other.signalLevel())
        return false;
    if (locked() != other.locked())
        return false;
    if (signalQuality() != other.signalQuality())
        return false;
    if (frequency() != other.frequency())
        return false;
    return true;
}

RTCPReport &RTCPReport::operator =(const RTCPReport &other)
{
    d->packetCount = other.packetCount();
    d->ssrc = other.ssrc();
    d->feID = other.feID();
    d->signalLevel = other.signalLevel();
    d->locked = other.locked();
    d->signalQuality = other.signalQuality();
    d->frequency = other.frequency();
    return *this;
}

void RTCPReport::parse(const QByteArray &report)
{
    d->rtcpPacket = QByteArray(report);
    d->ptr = d->rtcpPacket.data();
    d->rb = d->rtcpPacket.size();

    do {
        if (isSR()) {
            RtcpSR *sr = (RtcpSR *)d->ptr;
            d->packetCount = sr->si.getPacketCount();
        }

        if (isAPP()) {
            RtcpAPP *app = (RtcpAPP *)d->ptr;
            d->ssrc = app->ssrc;

            QString str = QString(QByteArray((const char *)app->string, (int)app->getStringLength()));
            QStringList reportElements = str.split(";", QString::SkipEmptyParts);
            QList<SatipElement> satipElements;
            foreach (QString reportElement, reportElements) {
                QStringList nameValue = reportElement.split("=", QString::SkipEmptyParts);
                SatipElement satipElement;
                satipElement.name = nameValue[0];
                satipElement.values = nameValue[1].split(",", QString::SkipEmptyParts);
                satipElements += satipElement;
            }

            foreach (SatipElement satIpElement, satipElements) {
                if (satIpElement.name == "tuner") {
                    d->feID = satIpElement.values[0].toUInt();
                    d->signalLevel = satIpElement.values[1].toUInt();
                    d->locked = satIpElement.values[2].toUShort();
                    d->signalQuality = satIpElement.values[3].toUInt();
                    d->frequency = satIpElement.values[4].toDouble();
                    //qDebug() << "signal level " << d->signalLevel << "qual" << d->signalQuality;
                }
            }
        }
    } while (next());
}

bool RTCPReport::next()
{
    RtcpCommonHeader *header = (RtcpCommonHeader *)d->ptr;
    quint32 size = (header->getLength() + 1) * 4;

    if (d->rb < size)
        return false;

    d->ptr += size;
    d->rb -= size;

    switch (((RtcpCommonHeader *)d->ptr)->type) {
    case RTCP_SR:
    case RTCP_RR:
    case RTCP_SDES:
    case RTCP_BYE:
    case RTCP_APP:
        break;
    default:
        return false;
    }

    return true;
}

bool RTCPReport::isSR()
{
    RtcpCommonHeader *header = (RtcpCommonHeader *)d->ptr;
    quint32 size = (header->getLength() + 1) * 4;

    if (header->type == RTCP_SR) {
        if (d->rb < size)
            return false;

        if (d->rb < sizeof(RtcpSR))
            return false;

        return true;
    }

    return false;
}

bool RTCPReport::isAPP()
{
    RtcpCommonHeader *header = (RtcpCommonHeader *)d->ptr;
    quint32 size = (header->getLength() + 1) * 4;

    if (header->type == RTCP_APP) {
        if (d->rb < size)
            return false;

        if (d->rb < sizeof(RtcpAPP))
            return false;

        return true;
    }

    return false;
}
