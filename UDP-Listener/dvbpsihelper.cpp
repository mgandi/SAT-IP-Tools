#include "dvbpsihelper.h"

#include <QtEndian>

DvbPsiHelper::DvbPsiHelper(QObject *parent) :
    QObject(parent)
{
}

QDateTime DvbPsiHelper::dateTimeFromMJD_BCD(const quint64 mjd_bcd)
{
    quint16 mjd = (mjd_bcd >> 24);
    int yp = (int)((((double)mjd) - 15078.2)/365.25);
    int mp = (int)(((((double)mjd) - 14956.1) - ((int)((double)yp) * 365.25)) / 30.6001);
    int d = (mjd - 14956 - ((int)(yp * 365.25)) - ((int)(mp * 30.6001)));
    int k = 0;
    if ((mp == 14) || (mp == 15)) {
        k = 1;
    }
    int y = (yp + k) + 1900;
    int m = mp - 1 - k * 12;

    quint32 bcd = ((quint32)(mjd_bcd & 0x00FFFFFF));

    return QDateTime::fromString(QString("%1/%2/%3 %4:%5:%6")
                                 .arg(d).arg(m).arg(y)
                                 .arg((bcd & 0xFF0000) >> 16, 0, 16)
                                 .arg((bcd & 0x00FF00) >> 8, 0, 16)
                                 .arg((bcd & 0x0000FF) >> 0, 0, 16), QString("d/M/yyyy h:m:s"));
}

QTime DvbPsiHelper::timeFromBCD(const quint32 bcd)
{
    return QTime::fromString(QString("%1:%2:%3")
                             .arg((bcd & 0xFF0000) >> 16, 0, 16)
                             .arg((bcd & 0x00FF00) >> 8, 0, 16)
                             .arg((bcd & 0x0000FF) >> 0, 0, 16), QString("h:m:s"));
}

QString DvbPsiHelper::stringFromAnnexA(const char *string, int stringLength)
{
    if ((string[0] >= 0x01) && (string[0] <= 0x05)) {
        return QString::fromLatin1(QByteArray(&string[1], stringLength - 1));
    } else if (string[0] >= 0x10) {
//        return "Do not know how to decode text!";
        return QString();
    } else if (string[0] >= 0x11) {
//        return "Do not know how to decode text!";
        return QString();
    } else if (string[0] >= 0x20) {
        return QString::fromLatin1(QByteArray(&string[1], stringLength - 1));
    } else {
        return "Language coding does not exist!";
    }
}
