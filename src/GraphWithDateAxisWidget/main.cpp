#include "graphwithdateaxiswidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphWithDateAxisWidget w;
    w.slotAddRequirementsFromFile("./project/requirements/requirements.xml");
    QDateTime temp(QDate::currentDate().addDays(-1), QTime(10, 0, 0));

    w.slotSetMinDate(temp);
    w.slotSetMaxDate(temp.addDays(1));

    w.slotUpdateChart();
    w.show();
    return a.exec();
}
