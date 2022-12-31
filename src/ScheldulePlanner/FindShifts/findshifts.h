#ifndef FINDSHIFTS_H
#define FINDSHIFTS_H

#include <QDialog>
#include <QRegExp>
#include <QIntValidator>

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/AssignedShift.h"

namespace Ui {
class FindShifts;
}

class FindShifts : public QDialog
{
    Q_OBJECT

public:
    explicit FindShifts(QWidget *parent = nullptr);
    ~FindShifts();

private:
    Ui::FindShifts *ui;

    Positions m_positions;
    Shifts m_shifts;
    AssignedShifts m_assigned;

    bool inRangeDateTime(const QDateTime& value,
                         const QDateTime& start,
                         const QDateTime& end);

public slots:
    void setPositions(Positions);
    void setShifts(Shifts);
    void setAssignedShifts(AssignedShifts);
    void setStartDate(const QDateTime&);
    void setEndDate(const QDateTime&);
    void updateResults();
};

#endif // FINDSHIFTS_H
