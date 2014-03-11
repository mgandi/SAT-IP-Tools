#include "eventitem.h"
#include "event.h"
#include "epgconstants.h"
#include "epgwidget.h"

#include <QPainter>
#include <QTime>
#include <QtDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QRect>

#define EXTENDED_FIXED_WIDTH
#define EXTENDED_SHOW_EXTENDED_DESCRIPTION

class EventItemPrivate
{
public:
    EventItemPrivate() :
        detailedView(false)
    {}

    EpgWidget *epgWidget;
    EventItem *p;
    Event *event;
    bool detailedView;
};

EventItem::EventItem(int row, Event *event, EpgWidget *epgWidget) :
    d(new EventItemPrivate)
{
    d->epgWidget = epgWidget;
    d->event = event;
    setPos(((d->event->startDateTime().time().hour() * 60) + d->event->startDateTime().time().minute()) * 3, row * BOX_HEIGHT);
    setAcceptHoverEvents(true);
}

EventItem::~EventItem()
{
    delete d;
}


QRectF EventItem::boundingRect() const
{
    if (d->detailedView) {
        return extendedBoundingRect();
    } else {
        return standardBoundingRect();
    }
}

QPainterPath EventItem::shape() const
{
    QPainterPath path;
    path.addRoundedRect(BOX_MARGIN, BOX_MARGIN, length() - (2 * BOX_MARGIN), BOX_HEIGHT - (2 * BOX_MARGIN), BOX_RADIUS, BOX_RADIUS);
    return path;
}

void EventItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Compute max text length
    qreal maxTextLength;
    if (d->detailedView) {
        maxTextLength = extendedWidth() - (2 * INNER_BOX_MARGIN) - (2 * BOX_MARGIN);
    } else {
        maxTextLength = length() - (2 * INNER_BOX_MARGIN) - (2 * BOX_MARGIN);
    }
    if (maxTextLength < 0)
        maxTextLength = 0;

    // Compute different pens and brushes
    QColor background;
    QColor border;
    QColor text;
    switch (d->event->type()) {
    case Event::Undefined:
    case Event::Special:
    case Event::User:
    default:
        background = QColor(255, 255, 255);
        border = QColor(204, 204, 204);
        text = QColor(0, 0, 0);
        break;
    case Event::Movie:
    case Event::Show:
        background = QColor(91, 192, 222);
        border = QColor(70, 184, 218);
        text = QColor(255, 255, 255);
        break;
    case Event::News:
        background = QColor(66, 139, 202);
        border = QColor(53, 126, 189);
        text = QColor(255, 255, 255);
        break;
    case Event::Sports:
        background = QColor(217, 83, 79);
        border = QColor(212, 63, 58);
        text = QColor(255, 255, 255);
        break;
    case Event::Children:
    case Event::Education:
        background = QColor(92, 184, 92);
        border = QColor(74, 176, 74);
        text = QColor(255, 255, 255);
        break;
    case Event::Music:
    case Event::Culture:
        background = QColor(240, 173, 78);
        border = QColor(238, 162, 54);
        text = QColor(255, 255, 255);
        break;
    case Event::Social:
    case Event::Leisure:
        background = QColor(86, 61, 124);
        border = QColor(70, 50, 101);
        text = QColor(255, 255, 255);
        break;
    }

    // Compute fonts
    QFont regFont = painter->font();
    regFont.setPointSize(12);
    regFont.setBold(false);
    QFont boldFont = regFont;
    boldFont.setBold(true);

    // Draw box
    painter->setBrush(background);
    painter->setPen(border);
    if (d->detailedView) {
        // Draw extended box
        QPainterPath path;
        path.moveTo(BOX_RADIUS + BOX_MARGIN, BOX_MARGIN);
        path.lineTo(extendedWidth() - BOX_RADIUS - BOX_MARGIN, BOX_MARGIN);
        path.arcTo(extendedWidth() - (BOX_RADIUS * 2) - BOX_MARGIN, BOX_MARGIN, BOX_RADIUS * 2, BOX_RADIUS * 2, 90, -90);
        path.lineTo(extendedWidth() - BOX_MARGIN, extendedHeight(painter) - BOX_RADIUS - BOX_MARGIN);
        path.arcTo(extendedWidth() - (BOX_RADIUS * 2) - BOX_MARGIN, extendedHeight(painter) - (BOX_RADIUS * 2) - BOX_MARGIN, BOX_RADIUS * 2, BOX_RADIUS * 2, 0, -90);
        path.lineTo(BOX_RADIUS + BOX_MARGIN, extendedHeight(painter) - BOX_MARGIN);
        path.arcTo(BOX_MARGIN, extendedHeight(painter) - (BOX_RADIUS * 2) - BOX_MARGIN, BOX_RADIUS * 2, BOX_RADIUS * 2, 270, -90);
        path.lineTo(BOX_MARGIN, BOX_RADIUS + BOX_MARGIN);
        path.arcTo(BOX_MARGIN, BOX_MARGIN, BOX_RADIUS * 2, BOX_RADIUS * 2, 180, -90);
        painter->drawPath(path);
        painter->fillPath(path, background);

        // Draw original box
        painter->setBrush(border);
        painter->drawRoundedRect(BOX_MARGIN, BOX_MARGIN, length() - (2 * BOX_MARGIN), BOX_HEIGHT - (2 * BOX_MARGIN), BOX_RADIUS, BOX_RADIUS);
    } else {
        painter->drawRoundedRect(BOX_MARGIN, BOX_MARGIN, length() - (2 * BOX_MARGIN), BOX_HEIGHT - (2 * BOX_MARGIN), BOX_RADIUS, BOX_RADIUS);
    }

    // Draw event name
    QFontMetrics fontMetrics(boldFont);
    QString nt = QString("%1 %2").arg(d->event->startDateTime().toString("h:mm")).arg(d->event->name());
    painter->setFont(boldFont);
    painter->setPen(text);
    QRect enr;
    if (d->detailedView) {
        painter->drawText(BOX_MARGIN + INNER_BOX_MARGIN,
                          BOX_MARGIN + INNER_BOX_MARGIN,
                          extendedWidth(),
                          extendedHeight(painter),
                          Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                          nt,
                          &enr);
    } else {
        while (fontMetrics.width(nt) > maxTextLength) nt.chop(1);
        painter->drawText(BOX_MARGIN + INNER_BOX_MARGIN, BOX_MARGIN + INNER_BOX_MARGIN + fontMetrics.ascent(), nt);
    }

    // Draw short description
    fontMetrics = QFontMetrics(regFont);
    QString sd = d->event->shortDescription();
    painter->setFont(regFont);
    painter->setPen(text);
    QRect sdr;
    if (d->detailedView) {
        painter->drawText(BOX_MARGIN + INNER_BOX_MARGIN,
                          BOX_MARGIN + INNER_BOX_MARGIN + fontMetrics.lineSpacing(),
                          extendedWidth() - (INNER_BOX_MARGIN * 2) - (BOX_MARGIN * 2),
                          extendedHeight(painter) - (INNER_BOX_MARGIN * 2) - (BOX_MARGIN * 2) - enr.height(),
                          Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                          sd,
                          &sdr);
    } else {
        while (fontMetrics.width(sd) > maxTextLength) sd.chop(1);
        painter->drawText(BOX_MARGIN + INNER_BOX_MARGIN, BOX_MARGIN + INNER_BOX_MARGIN + fontMetrics.ascent() + fontMetrics.lineSpacing(), sd);
    }

#ifdef EXTENDED_SHOW_EXTENDED_DESCRIPTION
    // Draw extended Description;
    QString ed = d->event->extendedDescription();
    if (d->detailedView) {
        int x = BOX_MARGIN + INNER_BOX_MARGIN;
        int y = BOX_MARGIN + INNER_BOX_MARGIN + (2 * fontMetrics.lineSpacing()) + sdr.height();
        int w = maxTextLength;
        int h = (BOX_HEIGHT * BOX_HEIGHT_FACTOR) - BOX_HEIGHT - (2 * INNER_BOX_MARGIN) - (2 * BOX_MARGIN);
        painter->drawText(x, y, w, h, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, ed);
    }
#endif
}

qreal EventItem::length() const
{
    return ((d->event->duration().hour() * 60) + d->event->duration().minute()) * 3;
}

qreal EventItem::extendedWidth() const
{
#ifdef EXTENDED_FIXED_WIDTH
    return length() > BOX_EXT_WIDTH ? length() : BOX_EXT_WIDTH;
#else
    QFontMetrics bolFontMetrics(boldFont);
    QFontMetrics regFontMetrics(regFont);
    int widerText = qMax(bolFontMetrics.width(d->event->shortDescription()), regFontMetrics.width(d->event->extendedDescription()));
    return length() > widerText ? length() : widerText;
#endif
}

qreal EventItem::extendedHeight(QPainter *painter) const
{
    // Compute fonts
    QFont regFont = painter->font();
    regFont.setPointSize(12);
    regFont.setBold(false);
    QFont boldFont = regFont;
    boldFont.setBold(true);
    QFontMetrics regFontMetrics(regFont);
    QFontMetrics boldFontMetrics(boldFont);

    // Compute height of event name for fixed width
    int enh = boldFontMetrics.height();

    // Compute height of short description for fixed width
    int sdh = regFontMetrics.height();
    if (regFontMetrics.elidedText(d->event->shortDescription(), Qt::ElideRight, BOX_EXT_WIDTH, Qt::AlignLeft | Qt::AlignTop) != d->event->shortDescription()) {
        int f = 3;
        while ((f * sdh) == regFontMetrics.boundingRect(0,0,BOX_EXT_WIDTH,f * sdh,Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,d->event->shortDescription()).height()) {
            ++f;
        }
        sdh = sdh * --f;
    }

#ifdef EXTENDED_SHOW_EXTENDED_DESCRIPTION
    // Compute height of extended description for fixed width
    int edh = regFontMetrics.height();
    if (regFontMetrics.elidedText(d->event->extendedDescription(), Qt::ElideRight, BOX_EXT_WIDTH, Qt::AlignLeft | Qt::AlignTop) != d->event->extendedDescription()) {
        int f = 3;
        while ((f * edh) == regFontMetrics.boundingRect(0,0,BOX_EXT_WIDTH,f * edh,Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,d->event->extendedDescription()).height()) {
            ++f;
        }
        edh = edh * --f;
    }
    sdh += regFontMetrics.lineSpacing() + edh;
#endif

    int h = (2 * INNER_BOX_MARGIN) + (2 * BOX_MARGIN) + enh + sdh;

    return BOX_HEIGHT > h ? BOX_HEIGHT : h;
}

QRectF EventItem::standardBoundingRect() const
{
    return QRectF(BOX_MARGIN - PEN_WIDTH / 2, BOX_MARGIN - PEN_WIDTH / 2,
                  length() + PEN_WIDTH, BOX_HEIGHT + PEN_WIDTH);
}

QRectF EventItem::extendedBoundingRect() const
{
    return QRectF(BOX_MARGIN - PEN_WIDTH / 2, BOX_MARGIN - PEN_WIDTH / 2,
                  extendedWidth() + PEN_WIDTH, (BOX_HEIGHT * BOX_HEIGHT_FACTOR) + PEN_WIDTH);
}

void EventItem::toggleDetailedView()
{
    d->detailedView = !d->detailedView;
    if (d->detailedView) {
        setZValue(1);
    } else {
        setZValue(0);
    }
    prepareGeometryChange();
    update();
}

void EventItem::showDetailedView()
{
    if (d->detailedView)
      return;

    d->detailedView = !d->detailedView;
    setZValue(1);

    prepareGeometryChange();
    update();
}

void EventItem::hideDetailedView()
{
    if (!d->detailedView)
      return;

    d->detailedView = !d->detailedView;
    setZValue(0);

    prepareGeometryChange();
    update();
}

void EventItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    showDetailedView();
    event->accept();
}

void EventItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hideDetailedView();
    event->accept();
}

void EventItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void EventItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void EventItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void EventItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    d->epgWidget->recordEvent(d->event);
}
