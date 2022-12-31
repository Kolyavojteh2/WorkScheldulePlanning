#ifndef FINDVACATION_H
#define FINDVACATION_H

#include <QDialog>
#include <QRegExp>
#include <QIntValidator>

#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/SingleVacation.h"

namespace Ui {
class FindVacation;
}

class FindVacation : public QDialog
{
    Q_OBJECT

public:
    explicit FindVacation(QWidget *parent = nullptr);
    ~FindVacation();

private:
    Ui::FindVacation *ui;

    Workers m_workers;
    Vacations m_vacations;

    bool inRangeDateTime(const QDateTime& value,
                         const QDateTime& start,
                         const QDateTime& end);

public slots:
    void setVacations(Vacations);
    void setWorkers(Workers);
    void setStartDate(const QDateTime&);
    void setEndDate(const QDateTime&);
    void updateResults();
};

#endif // FINDVACATION_H
