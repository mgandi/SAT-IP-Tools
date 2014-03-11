#ifndef DVBPSIHELPER_H
#define DVBPSIHELPER_H

#include <QObject>
#include <QDateTime>

class DvbPsiHelper : public QObject
{
    Q_OBJECT
public:
    explicit DvbPsiHelper(QObject *parent = 0);

    static QDateTime dateTimeFromMJD_BCD(const quint64 mjd_bcd);
    static QTime timeFromBCD(const quint32 bcd);
    static QString stringFromAnnexA(const char *string, int stringLength);

};

#endif // DVBPSIHELPER_H
