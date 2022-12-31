#include "projectdataloader.h"

Positions ProjectDataLoader::loadPositions(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Positions info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "Positions")
                return info;

            info = parsePositions(doc);
        }
        file.close();
    }

    return info;
}

Positions ProjectDataLoader::parsePositions(QDomDocument& doc)
{
    // Positions
    QDomElement rootElement = doc.documentElement();
    Positions allPositions;
    // Single position
    QDomNode node_singlePosition = rootElement.firstChild();
    while(!node_singlePosition.isNull() && node_singlePosition.isElement())
    {
        QDomElement elem_singlePosition = node_singlePosition.toElement();
        // Attributes
        Position info_singlePosition;
        QDomNode node_attribute = elem_singlePosition.firstChild();
        while(!node_attribute.isNull() && node_attribute.isElement())
        {
            QDomElement elem_attribute = node_attribute.toElement();
            if (elem_attribute.tagName() == "Brief")
            {
                info_singlePosition.Brief = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "FullName")
            {
                info_singlePosition.FullName = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "Description")
            {
                info_singlePosition.Description = elem_attribute.text();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allPositions.insert(info_singlePosition.Brief, info_singlePosition);

        node_singlePosition = node_singlePosition.nextSibling();
    }
    return allPositions;
}

ProjectSetup ProjectDataLoader::loadProjectSetup(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    ProjectSetup info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "ProjectSetup")
                return info;

            info = parseProjectSetup(doc);
        }
        file.close();
    }

    return info;
}

ProjectSetup ProjectDataLoader::parseProjectSetup(QDomDocument& doc)
{
    // ProjectSetup
    QDomElement rootElement = doc.documentElement();
    ProjectSetup setup;
    // Attributes
    QDomNode node_attribute = rootElement.firstChild();
    while(!node_attribute.isNull() && node_attribute.isElement())
    {
        QDomElement elem_attribute = node_attribute.toElement();
        if (elem_attribute.tagName() == "StartPeriod")
        {
            setup.StartPeriod = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "EndPeriod")
        {
            setup.EndPeriod = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "MinRestTime")
        {
            setup.MinRestTime = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "StandartRestTime")
        {
            setup.StandartRestTime = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "MaxHoursPerPeriod")
        {
            setup.MaxHoursPerPeriod = elem_attribute.text().toInt();
        }

        node_attribute = node_attribute.nextSibling();
    }

    return setup;
}

Requirements ProjectDataLoader::loadRequirements(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Requirements info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "Requirements")
                return info;

            info = parseRequirements(doc);
        }
        file.close();
    }

    return info;
}

Requirements ProjectDataLoader::parseRequirements(QDomDocument& doc)
{
    // Requirements
    QDomElement rootElement = doc.documentElement();
    Requirements allRequirements;

    // Single requirement
    QDomNode node_singleRequirement = rootElement.firstChild();
    while(!node_singleRequirement.isNull() && node_singleRequirement.isElement())
    {
        QDomElement elem_singleRequirement = node_singleRequirement.toElement();
        // Attributes
        SingleRequirement info_singleRequirement;
        QDomNode node_attribute = elem_singleRequirement.firstChild();
        while(!node_attribute.isNull() && node_attribute.isElement())
        {
            QDomElement elem_attribute = node_attribute.toElement();
            if (elem_attribute.tagName() == "ID")
            {
                info_singleRequirement.ID = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "StartDate")
            {
                info_singleRequirement.StartDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "EndDate")
            {
                info_singleRequirement.EndDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "Position")
            {
                info_singleRequirement.Position = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "NumberWorkers")
            {
                info_singleRequirement.NumberWorkers = elem_attribute.text().toInt();
            }
            /*
            if (elem_attribute.tagName() == "Severity")
            {
                info_singleRequirement.Severity = elem_attribute.text().toInt();
            }
            */
            /*
            if (elem_attribute.tagName() == "BiggerAmountWorkers")
            {
                info_singleRequirement.BiggerAmountWorkers = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "Neglect")
            {
                info_singleRequirement.Neglect = elem_attribute.text().toInt();
            }
            */

            node_attribute = node_attribute.nextSibling();
        }
        allRequirements.insert(QString::number(info_singleRequirement.ID), info_singleRequirement);

        node_singleRequirement = node_singleRequirement.nextSibling();
    }
    return allRequirements;
}

Vacations ProjectDataLoader::loadVacations(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Vacations info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "Vacations")
                return info;

            info = parseVacations(doc);
        }
        file.close();
    }

    return info;
}

Vacations ProjectDataLoader::parseVacations(QDomDocument& doc)
{
    // Vacations
    QDomElement rootElement = doc.documentElement();
    Vacations allVacations;
    // Single Vacation
    QDomNode node_singleVacation = rootElement.firstChild();
    while(!node_singleVacation.isNull() && node_singleVacation.isElement())
    {
        QDomElement elem_singleVacation = node_singleVacation.toElement();
        // Attributes
        SingleVacation info_singleVacation;
        QDomNode node_attribute = elem_singleVacation.firstChild();
        while(!node_attribute.isNull() && node_attribute.isElement())
        {
            QDomElement elem_attribute = node_attribute.toElement();
            if (elem_attribute.tagName() == "ID_Vacation")
            {
                info_singleVacation.ID_vacation = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "ID_Worker")
            {
                info_singleVacation.ID_worker = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "StartDate")
            {
                info_singleVacation.StartDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "EndDate")
            {
                info_singleVacation.EndDate = elem_attribute.text();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allVacations.insert(QString::number(info_singleVacation.ID_vacation), info_singleVacation);

        node_singleVacation = node_singleVacation.nextSibling();
    }
    return allVacations;
}

Wishes ProjectDataLoader::loadWishes(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Wishes info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "WishesWorkers")
                return info;

            info = parseWishes(doc);
        }
        file.close();
    }

    return info;
}

Wishes ProjectDataLoader::parseWishes(QDomDocument& doc)
{
    // Wishes
    QDomElement rootElement = doc.documentElement();
    Wishes allWishes;

    // Single Wish
    QDomNode node_singleWish = rootElement.firstChild();
    while(!node_singleWish.isNull() && node_singleWish.isElement())
    {
        QDomElement elem_singleWish = node_singleWish.toElement();
        // Attributes
        SingleWish info_singleWish;
        QDomNode node_attribute = elem_singleWish.firstChild();
        while(!node_attribute.isNull() && node_attribute.isElement())
        {
            QDomElement elem_attribute = node_attribute.toElement();
            if (elem_attribute.tagName() == "ID_Wishes")
            {
                info_singleWish.ID_Wishes = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "ID_Workers")
            {
                info_singleWish.ID_Workers = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "StartDate")
            {
                info_singleWish.StartDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "EndDate")
            {
                info_singleWish.EndDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "Position")
            {
                info_singleWish.Position = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "Addition")
            {
                info_singleWish.Addition = elem_attribute.text();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allWishes.insert(QString::number(info_singleWish.ID_Wishes), info_singleWish);

        node_singleWish = node_singleWish.nextSibling();
    }
    return allWishes;
}

Worker ProjectDataLoader::loadWorker(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Worker info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "WorkerInformation")
                return info;

            info = parseWorker(doc);
        }
        file.close();
    }

    return info;
}

Worker ProjectDataLoader::parseWorker(QDomDocument& doc)
{
    // Worker
    QDomElement rootElement = doc.documentElement();
    Worker singleWorker;

    // Single Worker
    // Attributes
    QDomNode node_attribute = rootElement.firstChild();
    while(!node_attribute.isNull() && node_attribute.isElement())
    {
        QDomElement elem_attribute = node_attribute.toElement();
        if (elem_attribute.tagName() == "ID_worker")
        {
            singleWorker.ID_worker = elem_attribute.text().toInt();
        }
        if (elem_attribute.tagName() == "Surname")
        {
            singleWorker.Surname = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "FirstName")
        {
            singleWorker.FirstName = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "LastName")
        {
            singleWorker.LastName = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "PhoneNumber")
        {
            singleWorker.PhoneNumber = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "Email")
        {
            singleWorker.Email = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "Position")
        {
            singleWorker.ListPositions.push_back(elem_attribute.text());
        }

        node_attribute = node_attribute.nextSibling();
    }
    return singleWorker;
}

Shifts ProjectDataLoader::loadShifts(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Shifts info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "WorkShifts")
                return info;

            info = parseShifts(doc);
        }
        file.close();
    }

    return info;
}

Shifts ProjectDataLoader::parseShifts(QDomDocument& doc)
{
    // Shifts
    QDomElement rootElement = doc.documentElement();
    Shifts allShifts;

    // Single Shifts
    QDomNode node_singleShift = rootElement.firstChild();
    while(!node_singleShift.isNull() && node_singleShift.isElement())
    {
        QDomElement elem_singleShift = node_singleShift.toElement();
        // Attributes
        SingleShift info_singleShift;
        QDomNode node_attribute = elem_singleShift.firstChild();
        while(!node_attribute.isNull() && node_attribute.isElement())
        {
            QDomElement elem_attribute = node_attribute.toElement();
            if (elem_attribute.tagName() == "ID_Shift")
            {
                info_singleShift.ID_Shifts = elem_attribute.text().toInt();
            }
            //if (elem_attribute.tagName() == "ID_Workers")
            //{
            //    info_singleShift.ID_Workers = elem_attribute.text().toInt();
            //}
            if (elem_attribute.tagName() == "StartDate")
            {
                info_singleShift.StartDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "EndDate")
            {
                info_singleShift.EndDate = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "Position")
            {
                info_singleShift.Position = elem_attribute.text();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allShifts.insert(QString::number(info_singleShift.ID_Shifts), info_singleShift);

        node_singleShift = node_singleShift.nextSibling();
    }
    return allShifts;
}


AssignedShifts ProjectDataLoader::loadAssignedShifts(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    AssignedShifts info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "AssignedShifts")
                return info;

            info = parseAssignedShifts(doc);
        }
        file.close();
    }

    return info;
}

AssignedShifts ProjectDataLoader::parseAssignedShifts(QDomDocument& doc)
{
    // Assigned Shifts
    QDomElement rootElement = doc.documentElement();
    AssignedShifts allShifts;

    // Single Assigned Shift
    QDomNode node_singleShift = rootElement.firstChild();
    while(!node_singleShift.isNull() && node_singleShift.isElement())
    {
        QDomElement elem_singleShift = node_singleShift.toElement();
        // Attributes
        SingleAssignedShift info_singleShift;
        QDomNode node_attribute = elem_singleShift.firstChild();
        while(!node_attribute.isNull() && node_attribute.isElement())
        {
            QDomElement elem_attribute = node_attribute.toElement();
            if (elem_attribute.tagName() == "ID_Shift")
            {
                info_singleShift.ID_Shift = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "ID_Worker")
            {
                info_singleShift.ID_Worker = elem_attribute.text().toInt();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allShifts.insert(QString::number(info_singleShift.ID_Shift), info_singleShift);

        node_singleShift = node_singleShift.nextSibling();
    }
    return allShifts;
}
