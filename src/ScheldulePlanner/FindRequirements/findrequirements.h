#ifndef FINDREQUIREMENTS_H
#define FINDREQUIREMENTS_H

#include <QDialog>
#include <QRegExp>

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/SingleRequirement.h"

namespace Ui {
class FindRequirements;
}

class FindRequirements : public QDialog
{
    Q_OBJECT

public:
    explicit FindRequirements(QWidget *parent = nullptr);
    ~FindRequirements();

private:
    Ui::FindRequirements *ui;

    Requirements m_requirements;
    Positions m_listOfPositions;

    bool inRangeDateTime(const QDateTime& value,
                         const QDateTime& start,
                         const QDateTime& end);

public slots:
    void setPositions(Positions);
    void setRequirements(Requirements);
    void setStartDate(const QDateTime&);
    void setEndDate(const QDateTime&);
    void updateResults();
};

#endif // FINDREQUIREMENTS_H
