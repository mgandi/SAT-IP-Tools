#include "elementarystream.h"

class ElementaryStreamPrivate
{
public:
    quint16 pid;
    quint8 type;
};

ElementaryStream::ElementaryStream(const ElementaryStream &other) :
    QObject(other.parent()),
    d(new ElementaryStreamPrivate)
{
    d->pid = other.pid();
    d->type = other.type();
}

ElementaryStream::ElementaryStream(quint16 pid, quint8 type, QObject *parent) :
    QObject(parent),
    d(new ElementaryStreamPrivate)
{
    d->pid = pid;
    d->type = type;
}

ElementaryStream::~ElementaryStream()
{
    delete d;
}


quint16 ElementaryStream::pid() const
{
    return d->pid;
}

quint8 ElementaryStream::type() const
{
    return d->type;
}

QString ElementaryStream::typeToString() const
{
    switch (d->type)
    {
    case 0x00: return "ITU-T | ISO/IEC Reserved";
    case 0x01: return "ISO/IEC 11172-2 Video";
    case 0x02: return "ITU-T Rec H.262 | ISO/IEC 13818-2 Video stream descriptor or ISO/IEC 11172-2 constrained parameter video stream";
    case 0x03: return "ISO/IEC 11172-3 Audio stream descriptor";
    case 0x04: return "ISO/IEC 13818-3 Audio MPEG Audio layer 1/2";
    case 0x05: return "ITU-T Rec H.222.0 | ISO/IEC 13818-1 Private Section: Registration descriptor";
    case 0x06: return "ITU-T Rec H.222.0 | ISO/IEC 13818-1 Private PES data packets";
    case 0x07: return "ISO/IEC 13522 MHEG";
    case 0x08: return "ITU-T Rec H.222.0 | ISO/IEC 13818-1 Annex A DSM CC";
    case 0x09: return "ITU-T Rec H222.1";
    case 0x0a: return "ISO/IEC 13818-6 type A";
    case 0x0b: return "ISO/IEC 13818-6 type B";
    case 0x0c: return "ISO/IEC 13818-6 type C";
    case 0x0d: return "ISO/IEC 13818-6 type D";
    case 0x0e: return "ITU-T Rec H.222.0 | ISO/IEC 13818-1 auxillary";

    case 0x0f: return "ISO/IEC 13818-7 MPEG2 Audio with ADTS transport syntax";
    case 0x10: return "ISO/IEC 14496-2 Visual";
    case 0x11: return "ISO/IEC 14496-3 MPEG4 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3";
    case 0x12: return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets";
    case 0x13: return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC 14496_sections";
    case 0x14: return "ISO/IEC 13818-6 Synchronized Download Protocol";
    case 0x15: return "Metadata carried in PES packets";
    case 0x16: return "Metadata carried in metadata_sections";
    case 0x17: return "Metadata carried in ISO/IEC 13818-6 Data Carousel";
    case 0x18: return "Metadata carried in ISO/IEC 13818-6 Object Carousel";
    case 0x19: return "Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol";
    case 0x1A: return "IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP)";
    case 0x1B: return "AVC video stream as defined in ITU-T Rec. H.264 | ISO/IEC 14496-10 Video";
    case 0x7F: return "IPMP stream";
    default:
        if ((d->type >= 0x1C) && (d->type <= 0x7E))
            return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved";
        else if (d->type >= 0x80) /* 0x80 - 0xFF */
            return "User Private";
        else
            return "Unknown";
        break;
    };
}
