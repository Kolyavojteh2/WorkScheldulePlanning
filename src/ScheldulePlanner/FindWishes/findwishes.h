#ifndef FINDWISHES_H
#define FINDWISHES_H

#include <QDialog>
#include <QRegExp>
#include <QIntValidator>

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/SingleWish.h"

namespace Ui {
class FindWishes;
}

class FindWishes : public QDialog
{
    Q_OBJECT

public:
    explicit FindWishes(QWidget *parent = nullptr);
    ~FindWishes();

private:
    Ui::FindWishes *ui;

    Positions m_positions;
    Workers m_workers;
    Wishes m_wishes;

    bool inRangeDateTime(const QDateTime& value,
                         const QDateTime& start,
                         const QDateTime& end);

public slots:
    void setPositions(Positions);
    void setWishes(Wishes);
    void setWorkers(Workers);
    void setStartDate(const QDateTime&);
    void setEndDate(const QDateTime&);
    void updateResults();
};

#endif // FINDWISHES_H
