#include "workersinformation.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WorkersInformation w;
    w.slotSetPathPositionsInformation("./project/positions/");
    w.slotSetDefaulPath("./project/workers/");
    w.show();
    return a.exec();
}
