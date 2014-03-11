#ifndef DVBTCONFIG_H
#define DVBTCONFIG_H

#include "gatewaydevice.h"

#include <QWidget>

namespace Ui {
class DvbTConfig;
}

class DvbTConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit DvbTConfig(QWidget *parent = 0);
    ~DvbTConfig();

    quint8 fe() const;
    quint32 freq() const;
    quint8 bw() const;
    QString pids() const;

private:
    Ui::DvbTConfig *ui;
};

#endif // DVBTCONFIG_H
