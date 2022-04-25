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

    bool operator==(const Position& r) const
    {
        if (Brief == r.Brief &&
                FullName == r.FullName &&
                Description == r.Description)
            return  true;
        return false;
    }

    bool operator!=(const Position& r) const
    {
        if (*this == r)
            return false;
        return true;
    }
};

class Positions
{
public:
    QMap<QString, Position> list;

    bool operator==(const Positions& r) const
    {
        if (this->list.size() != r.list.size())
            return false;

        return true;
    }

    bool operator!=(const Positions& r) const
    {
        if (*this == r)
            return false;
        return true;
    }
};

#endif // POSITIONS_H
