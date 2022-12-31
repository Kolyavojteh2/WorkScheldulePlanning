#ifndef PROJECTSETUP_H
#define PROJECTSETUP_H

#include <QString>

class ProjectSetup
{
public:
    //int RoundTheClockMode;
    //QString OpeningCorporation;
    //QString ClosingCorporation;
    //int EveryMounthGraphic;
    //int GraphicForeverDay;
    QString MinRestTime;
    QString StandartRestTime;
    //int MaxPercentOfWorkShiftToPeriod;
    int MaxHoursPerPeriod;

    QString StartPeriod;
    QString EndPeriod;
};

#endif // PROJECTSETUP_H
