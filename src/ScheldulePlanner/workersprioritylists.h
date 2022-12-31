#ifndef WORKERSPRIORITYLISTS_H
#define WORKERSPRIORITYLISTS_H

#include <QString>
#include <QList>
#include <QSet>
#include <QTranslator>
#include <QRegExp>

#include "../../src/XML_Parsing/AssignedShift.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/ProjectSetup.h"

#include <QDateTime>

static QString greenText = QObject::tr("(Recommend)");
static QString yellowText = QObject::tr("(Not recommended)");

class WorkersPriorityLists
{
private:
    Vacations m_vacations;
    Shifts m_shifts;
    AssignedShifts m_assigned;
    Wishes m_wishes;
    Workers m_workers;
    ProjectSetup m_setupEnterprise;

    QSet<int> m_redList;
    QSet<int> m_yellowList;
    QSet<int> m_commonList;
    QSet<int> m_greenList;

private:
    void RedList(const SingleShift&);
    void YellowList(const SingleShift&);
    void GreenList(const SingleShift&);
    void CommonList();

    bool inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max);
    void selectWorkers(const SingleShift&);

public:
    WorkersPriorityLists(const Vacations&,
                         const Shifts&,
                         const AssignedShifts&,
                         const Wishes&,
                         const Workers&,
                         const ProjectSetup&);

    static int stringToID(const QString&);

    QList<QString> getWorkersListPriority(const SingleShift&);

    const QString getGreenText() const;
    const QString getYellowText() const;
};

#endif // WORKERSPRIORITYLISTS_H
