#ifndef SINGLEWISH_H
#define SINGLEWISH_H

#include <QString>
#include <QMap>

class SingleWish
{
public:
    int ID_Wishes;
    int ID_Workers;
    QString StartDate;
    QString EndDate;
    QString Position;
    QString Addition;
    int MinWorkerDays;
    int MaxWorkerDay;
    QString GetStartedLaterThan;
    QString GetEndEarlierThan;
};

typedef QMap<QString, SingleWish> Wishes;

#endif // SINGLEWISH_H
