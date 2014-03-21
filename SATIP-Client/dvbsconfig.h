#ifndef DVBSCONFIG_H
#define DVBSCONFIG_H

#include "gatewaydevice.h"

#include <QWidget>

namespace Ui {
class DvbSConfig;
}

class DvbSConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit DvbSConfig(QWidget *parent = 0);
    ~DvbSConfig();

    quint8 src() const;
    quint8 fe() const;
    quint32 freq() const;
    bool vpol() const;
    Mtype mtype() const;
    quint32 sr() const;
    Fec fec() const;
    QString pids() const;
    void hideFe();

private:
    Ui::DvbSConfig *ui;
};

#endif // DVBSCONFIG_H
