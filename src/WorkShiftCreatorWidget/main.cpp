#include "workshiftcreatorwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WorkShiftCreatorWidget w;
    w.show();
    return a.exec();
}
