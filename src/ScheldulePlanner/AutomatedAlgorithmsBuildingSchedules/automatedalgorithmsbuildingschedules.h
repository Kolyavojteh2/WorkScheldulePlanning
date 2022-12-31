#ifndef AUTOMATEDALGORITHMSBUILDINGSCHEDULES_H
#define AUTOMATEDALGORITHMSBUILDINGSCHEDULES_H

#include <QList>
#include <QMap>
#include <QSet>
#include <QRegExp>
#include <QQueue>

#include "../../src/XML_Parsing/DurationsOfWorkShift.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/SingleRequirement.h"
#include "../../src/XML_Parsing/ProjectSetup.h"
#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/AssignedShift.h"
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../workersprioritylists.h"

class AutomatedAlgorithmsBuildingSchedules
{
public:
    static Shifts MinStandart(const DurationsOfWorkShift&,
                              const ProjectSetup&,
                              const Requirements&,
                              const QList<QString>&,
                              const Shifts&);

    static Shifts MinMax(const DurationsOfWorkShift&,
                         const ProjectSetup&,
                         const Requirements&,
                         const QList<QString>&,
                         const Shifts&);

    static AssignedShifts AssigningShiftsToTheMostVulnerableShift(Shifts shifts,
                                                                  const Wishes& wishes,
                                                                  const Workers& workers,
                                                                  const Vacations& vacations,
                                                                  const AssignedShifts& assigned,
                                                                  const ProjectSetup& setup);

    static AssignedShifts AssigningShiftsToTheMostVulnerableShiftWithoutWishes(Shifts shifts,
                                                                               const Wishes& wishes,
                                                                  const Workers& workers,
                                                                  const Vacations& vacations,
                                                                  const AssignedShifts& assigned,
                                                                  const ProjectSetup& setup);


private:
    static const int StepSeconds = 5 * 60;

    static QMap<QString, QMap<QDateTime, int>> convertRequirementsToMap(const Requirements&,
                                                                         const ProjectSetup&,
                                                                         const QSet<QString>&);
    static Shifts createShiftsByDuration(const QTime& minDurationOfShift,
                                         const QTime& maxDurationOfShift,
                                         QMap<QDateTime, int> requirements,
                                         const QString& position,
                                         const Shifts& existingShifts);

    static qint64 timeToSeconds(const QTime& time);

    static Shifts mergeShifts(Shifts first, Shifts second);
    static int findLastShiftID(const Shifts&);

    static bool inRangeDateTime(const QDateTime& value,
                                const QDateTime& start,
                                const QDateTime& end);

    //static

    //static QMap<int /*ID_Worker*/, int /*number*/> getMapPossibilityOfWorkersByShift(const Shifts&,
    //                                                                                 const Workers&,
    //                                                                                const Vacations&,
    //                                                                                 const AssignedShifts&,
    //                                                                                 const ProjectSetup&);

};

#endif // AUTOMATEDALGORITHMSBUILDINGSCHEDULES_H
