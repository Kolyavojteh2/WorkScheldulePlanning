#include "projectsetupwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProjectSetupWidget w;
    w.show();
    return a.exec();
}
