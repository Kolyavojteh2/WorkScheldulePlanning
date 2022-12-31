#ifndef SHIFTS_H
#define SHIFTS_H

#include <QString>
#include <QMap>

class SingleShift
{
public:
    int ID_Shifts;
    QString Position;
    QString StartDate;
    QString EndDate;
    //int ID_Workers;
};

typedef QMap<QString, SingleShift> Shifts;

#endif // SHIFTS_H
