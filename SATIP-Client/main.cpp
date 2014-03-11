#include "mainwindow.h"
#include "settings.h"
#include "objectpool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Settings settings;
    qDebug() << qPrintable(settings.fileName());

    ObjectPool objectPool;

    MainWindow w;
    w.show();
    
    return a.exec();
}
