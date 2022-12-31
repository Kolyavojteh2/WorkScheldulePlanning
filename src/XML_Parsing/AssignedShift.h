#ifndef ASSIGNEDSHIFT_H
#define ASSIGNEDSHIFT_H

#include <QMap>
#include <QString>

class SingleAssignedShift
{
public:
    int ID_Shift;
    int ID_Worker;
};

typedef QMap<QString, SingleAssignedShift> AssignedShifts;

#endif // ASSIGNEDSHIFT_H
