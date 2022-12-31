#ifndef FINDASSIGNEDSHIFTS_H
#define FINDASSIGNEDSHIFTS_H

#include <QDialog>
#include <QRegExp>
#include <QIntValidator>

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/AssignedShift.h"
#include "../../src/XML_Parsing/Shifts.h"

namespace Ui {
class FindAssignedShifts;
}

class FindAssignedShifts : public QDialog
{
    Q_OBJECT

public:
    explicit FindAssignedShifts(QWidget *parent = nullptr);
    ~FindAssignedShifts();

private:
    Ui::FindAssignedShifts *ui;

    Positions m_positions;
    Workers m_workers;
    Shifts m_shifts;
    AssignedShifts m_assigned;

    bool inRangeDateTime(const QDateTime& value,
                         const QDateTime& start,
                         const QDateTime& end);

public slots:
    void setPositions(Positions);
    void setShifts(Shifts);
    void setAssigned(AssignedShifts);
    void setWorkers(Workers);
    void setStartDate(const QDateTime&);
    void setEndDate(const QDateTime&);
    void updateResults();
};

#endif // FINDASSIGNEDSHIFTS_H
