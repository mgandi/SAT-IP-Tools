#ifndef TABLEPARSER_P_H
#define TABLEPARSER_P_H

#include <QObject>

class TableParser : public QObject
{
    Q_OBJECT

public:
    explicit TableParser(QObject *parent = 0);

    void push(const quint8 p[], const quint8 s, bool newSection = false);

    void parse(const quint8 p[], const quint8 s);
    void append(const quint8 p[], const quint8 s);
//    quint64 uid() const;

    QByteArray section;
    quint8  tableID;
    quint8  sectionSyntaxIndicator;
    quint16 sectionLength;
    quint16 serviceID;
    quint8  versionNumber;
    quint8  currentNextIndicator;
    quint8  sectionNumber;
    quint8  lastSectionNumber;
    QByteArray payload;
    quint16 remaining;
    bool headerParsed;
    quint32 crc;
//    quint8  payloadSize;
//    const quint8 *payload;

    bool valid;

public slots:
    void reinit();

protected:
    void calculateCRC32();
    void setComplete();

signals:
    void complete();
};

#endif // TABLEPARSER_P_H
