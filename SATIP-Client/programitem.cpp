#include "programitem.h"
#include "program.h"
#include "epgconstants.h"
#include "epgcollector.h"

#include <QPainter>
#include <QtDebug>

class ProgramItemPrivate
{
public:
    Program *program;
    int row;
    bool collecting;
    EpgCollector *epgCollector;
};

ProgramItem::ProgramItem(int row, Program *program, EpgCollector *epgCollector) :
    d(new ProgramItemPrivate)
{
    d->program = program;
    d->row = row;
    d->epgCollector = epgCollector;
    int x = 0, y = row * BOX_HEIGHT;
    setPos(x, y);
}

ProgramItem::~ProgramItem()
{
    delete d;
}


QRectF ProgramItem::boundingRect() const
{
    return QRectF(BOX_MARGIN - PEN_WIDTH / 2, BOX_MARGIN - PEN_WIDTH / 2,
                  length() + PEN_WIDTH, BOX_HEIGHT + PEN_WIDTH);
}

void ProgramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    // Compute different pens and brushes
    QColor background;
    QColor text;
    if (d->collecting) {
        background = QColor(51,51,51);
        text = QColor(153,153,153);
    } else {
        background = Qt::white;
        text = Qt::black;
    }

    // Draw box
    painter->setBrush(background);
    painter->setPen(Qt::NoPen);
    painter->drawRect(0, 0, length(), BOX_HEIGHT);

    // Draw program index
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(12);
    QFontMetrics fontMetrics(font);
    int h = fontMetrics.ascent();
    painter->setFont(font);
    painter->setPen(text);
    painter->drawText(INNER_BOX_MARGIN, INNER_BOX_MARGIN + h, QString("%1").arg(d->row + 1));

    // Draw program name
    font.setBold(false);
    fontMetrics = QFontMetrics(font);
    h = fontMetrics.ascent() + fontMetrics.lineSpacing();
    painter->setFont(font);
    painter->drawText(INNER_BOX_MARGIN, INNER_BOX_MARGIN + h, d->program->serviceName());

//    // Draw wait GIF
//    painter->drawImage();

//    // Draw right border
//    painter->drawLine(length() - PEN_WIDTH, 0, length() - PEN_WIDTH, BOX_HEIGHT);
}

void ProgramItem::setColleting(bool collecting)
{
    d->collecting = collecting;
    update();
}

qreal ProgramItem::length() const
{
    return PROGRAM_ITEM_WIDTH;
}

void ProgramItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    d->epgCollector->stop();
    d->epgCollector->start(d->program);
}
