#ifndef PROJECTDATASAVER_H
#define PROJECTDATASAVER_H

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/ProjectSetup.h"
#include "../../src/XML_Parsing/SingleRequirement.h"
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/AssignedShift.h"

#include <QtXml/QtXml>

class ProjectDataSaver
{
public:
    static void savePositions(const QString&, const Positions&);
    static void saveProjectSetup(const QString&, const ProjectSetup&);
    static void saveRequirements(const QString&, const Requirements&);
    static void saveVacations(const QString&, const Vacations&);
    static void saveWishes(const QString&, const Wishes&);
    static void saveWorker(const QString&, const Worker&);
    static void saveWorkShifts(const QString&, const Shifts&);
    static void saveAssignedShifts(const QString&, const AssignedShifts&);

private:
    static QDomElement makeElement(QDomDocument &doc,
                                   const QString &tagName,
                                   const QString &tagValue);
};

#endif // PROJECTDATASAVER_H
