#include "requirement.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RequirementWidget w;
    w.slotSetPathPositionsInformation("./project/positions/");
    w.show();
    return a.exec();
}
