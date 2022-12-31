#ifndef XML_PARSING_H
#define XML_PARSING_H

#include "XML_Parsing_global.h"
#include <QString>
#include <QList>

#include <QtXml/QtXml>
#include <QFile>
#include <QTextStream>

#include "Worker.h"
#include "Positions.h"
#include "ProjectLocations.h"
#include "ProjectSetup.h"
#include "SingleVacation.h"
#include "SingleRequirement.h"
#include "SingleWish.h"
#include "Shifts.h"

QDomElement makeElement(QDomDocument& doc, const QString &tagName, const QString &tagValue);

/*******************************************************************************/
extern "C"
Worker read_WorkerInfoFromFile(const QString& filename);

extern "C"
void write_WorkerInfoToFile(const QString& filename, const Worker& info);

/*******************************************************************************/
extern "C"
Positions read_PositionsFromFile(const QString& filename);

extern "C"
void write_PositionsToFile(const QString& filename, const Positions& info);

/*******************************************************************************/
extern "C"
ProjectLocations read_ProjectLocationsFromFile(const QString& filename);

extern "C"
void write_ProjectLocationsToFile(const QString& filename, const ProjectLocations& info);

/*******************************************************************************/
extern "C"
ProjectSetup read_ProjectSetupFromFile(const QString& filename);

extern "C"
void write_ProjectSetupToFile(const QString& filename, const ProjectSetup& info);

/*******************************************************************************/
extern "C"
Vacations read_VacationsFromFile(const QString& filename);

extern "C"
void write_VacationsToFile(const QString& filename, const Vacations& info);

/*******************************************************************************/
extern "C"
Requirements read_RequirementsFromFile(const QString& filename);

extern "C"
void write_RequirementsToFile(const QString& filename, const Requirements& info);

/*******************************************************************************/
extern "C"
Wishes read_WishesFromFile(const QString& filename);

extern "C"
void write_WishesToFile(const QString& filename, const Wishes& info);

/*******************************************************************************/
extern "C"
Shifts read_ShiftsFromFile(const QString& filename);

extern "C"
void write_ShiftsToFile(const QString& filename, const Shifts& info);


#endif // XML_PARSING_H
