#ifndef SINGLEREQUIREMENT_H
#define SINGLEREQUIREMENT_H

#include <QString>
#include <QMap>

class SingleRequirement
{
public:
    int ID;
    QString StartDate;
    QString EndDate;
    QString Position;
    int NumberWorkers;
    //int Severity;
    //int BiggerAmountWorkers;
    //int Neglect;
};

typedef QMap<QString, SingleRequirement> Requirements;

#endif // SINGLEREQUIREMENT_H
