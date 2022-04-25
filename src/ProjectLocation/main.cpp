#include "projectlocationwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProjectLocationWidget w;
    w.show();
    return a.exec();
}
