#ifndef SECTION_P_H
#define SECTION_P_H

#include <QObject>

class Section : public QObject
{
    Q_OBJECT
public:
    explicit Section(QObject *parent = 0);

    QByteArray data;
    quint16 need;
    bool headerComplete;
    quint16 index;
    bool validCRC;
    const quint8 *payload;
    quint16 payloadSize;

    quint8 tableID;
    quint8  sectionSyntaxIndicator;
    quint16 sectionLength;
    quint16 serviceID;
    quint8  versionNumber;
    quint8  currentNextIndicator;
    quint8  sectionNumber;
    quint8  lastSectionNumber;
};

#endif // SECTION_P_H
