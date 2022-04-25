#include "wishesworkerswidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WishesWorkersWidget w;
    w.slotSetPathWorkersInformation("./project/workers/");
    w.slotSetPathPositionsInformation("./project/positions/");
    w.show();
    return a.exec();
}
