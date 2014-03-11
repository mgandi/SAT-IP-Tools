#ifndef EVENTITEM_H
#define EVENTITEM_H

#include <QGraphicsItem>

class EpgWidget;
class Event;
class EventItemPrivate;
class EventItem : public QGraphicsItem
{
public:
    EventItem(int row, Event *event, EpgWidget *epgWidget);
    ~EventItem();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    qreal length() const;
    qreal extendedWidth() const;
    qreal extendedHeight(QPainter *painter) const;
    QRectF standardBoundingRect() const;
    QRectF extendedBoundingRect() const;
    void toggleDetailedView();
    void showDetailedView();
    void hideDetailedView();
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

private:
    EventItemPrivate *d;
};

#endif // EVENTITEM_H
