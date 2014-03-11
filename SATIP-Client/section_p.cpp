#include "section_p.h"

Section::Section(QObject *parent) :
    QObject(parent),
    need(3),
    headerComplete(false),
    index(0),
    validCRC(false),
    sectionLength(0)
{
}
