#include "vacationwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VacationWidget w;
    w.slotSetPathWorkersInformation("./project/workers/");
    w.show();
    return a.exec();
}
