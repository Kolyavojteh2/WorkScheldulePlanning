#ifndef CONFIGURATIONCREATIONPROJECTBASEDEXISTING_H
#define CONFIGURATIONCREATIONPROJECTBASEDEXISTING_H

#include <QDateTime>
#include <QString>

class ConfigurationCreationProjectBasedExisting
{
public:
    QString projectName;

    QDateTime startPeriod;
    QDateTime endPeriod;

    int positions;
    int workers;
    int requirements;
    int wishes;
    int shifts;
    int assigned;
    int vacations;

    int adaptingRequirements;
    int adaptingWishes;
    int adaptingShifts;
    int adaptingVacations;
};

#endif // CONFIGURATIONCREATIONPROJECTBASEDEXISTING_H
