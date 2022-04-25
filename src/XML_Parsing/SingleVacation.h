#ifndef SINGLEVACATION_H
#define SINGLEVACATION_H

#include <QString>

class SingleVacation
{
public:
    int ID_vacation;
    int ID_worker;
    QString StartDate;
    QString EndDate;

};

typedef QMap<QString, SingleVacation> Vacations;

#endif // SINGLEVACATION_H
