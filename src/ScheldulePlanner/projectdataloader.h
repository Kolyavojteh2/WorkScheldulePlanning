#ifndef PROJECTDATALOADER_H
#define PROJECTDATALOADER_H

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/ProjectSetup.h"
#include "../../src/XML_Parsing/SingleRequirement.h"
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/AssignedShift.h"

#include <QtXml/QtXml>

class ProjectDataLoader
{
public:
    static Positions        loadPositions       (const QString&);
    static ProjectSetup     loadProjectSetup    (const QString&);
    static Requirements     loadRequirements    (const QString&);
    static Vacations        loadVacations       (const QString&);
    static Wishes           loadWishes          (const QString&);
    static Worker           loadWorker          (const QString&);
    static Shifts           loadShifts          (const QString&);
    static AssignedShifts   loadAssignedShifts  (const QString&);

private:
    static Positions        parsePositions      (QDomDocument&);
    static ProjectSetup     parseProjectSetup   (QDomDocument&);
    static Requirements     parseRequirements   (QDomDocument&);
    static Vacations        parseVacations      (QDomDocument&);
    static Wishes           parseWishes         (QDomDocument&);
    static Worker           parseWorker         (QDomDocument&);
    static Shifts           parseShifts         (QDomDocument&);
    static AssignedShifts   parseAssignedShifts (QDomDocument&);
};

#endif // PROJECTDATALOADER_H
