#include "workerinfo.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WorkerInfo w;
    w.show();
    return a.exec();
}
