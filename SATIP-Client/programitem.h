#ifndef PROGRAMITEM_H
#define PROGRAMITEM_H

#include <QGraphicsItem>

class EpgCollector;
class Program;
class ProgramItemPrivate;
class ProgramItem : public QGraphicsItem
{
public:
    ProgramItem(int row, Program *program, EpgCollector *epgCollector);
    ~ProgramItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setColleting(bool collecting);

protected:
    qreal length() const;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    ProgramItemPrivate *d;
};

#endif // PROGRAMITEM_H
