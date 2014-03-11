#ifndef PMTELEMENTARYSTREAM_H
#define PMTELEMENTARYSTREAM_H

#include <QObject>

class PmtElementaryStream : public QObject
{
    Q_OBJECT
public:
    PmtElementaryStream(QObject *parent = 0):
        QObject(parent) {}
    quint8 type;
    quint16 pid;
};

#endif // PMTELEMENTARYSTREAM_H
