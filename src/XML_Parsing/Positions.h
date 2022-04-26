#ifndef POSITIONS_H
#define POSITIONS_H

#include <QString>
#include <QList>
#include <QMap>

class Position
{
public:
    QString Brief;
    QString FullName;
    QString Description;

};

typedef QMap<QString, Position> Positions;

#endif // POSITIONS_H
