#ifndef EPGVIEW_H
#define EPGVIEW_H

#include <QGraphicsView>

class EpgView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit EpgView(QWidget *parent = 0);
};

#endif // EPGVIEW_H
