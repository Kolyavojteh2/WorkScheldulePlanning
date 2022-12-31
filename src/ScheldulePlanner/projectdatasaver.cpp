#include "projectdatasaver.h"

void ProjectDataSaver::savePositions(const QString& filename, const Positions& info)
{
    QDomDocument doc("Positions");
    QDomElement rootElem = doc.createElement("Positions");
    doc.appendChild(rootElem);

    for (auto it = info.begin(); it != info.end(); it++)
    {
        QDomElement singlePosition = doc.createElement("SinglePosition");

        singlePosition.appendChild(makeElement(doc, "Brief", it->Brief));
        singlePosition.appendChild(makeElement(doc, "FullName", it->FullName));
        singlePosition.appendChild(makeElement(doc, "Description", it->Description));

        rootElem.appendChild(singlePosition);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveProjectSetup(const QString& filename, const ProjectSetup& info)
{
    QDomDocument doc("ProjectSetup");
    QDomElement rootElem = doc.createElement("ProjectSetup");
    doc.appendChild(rootElem);

    rootElem.appendChild(makeElement(doc, "StartPeriod", info.StartPeriod));
    rootElem.appendChild(makeElement(doc, "EndPeriod", info.EndPeriod));

    rootElem.appendChild(makeElement(doc, "MinRestTime", info.MinRestTime));
    rootElem.appendChild(makeElement(doc, "StandartRestTime", info.StandartRestTime));
    rootElem.appendChild(makeElement(doc, "MaxHoursPerPeriod", QString::number(info.MaxHoursPerPeriod)));

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveRequirements(const QString& filename, const Requirements& info)
{
    QDomDocument doc("Requirements");
    QDomElement rootElem = doc.createElement("Requirements");
    doc.appendChild(rootElem);

    for (auto it = info.begin(); it != info.end(); it++)
    {
        QDomElement singleRequirement = doc.createElement("singleRequirement");

        singleRequirement.appendChild(makeElement(doc, "ID", QString::number(it->ID)));
        singleRequirement.appendChild(makeElement(doc, "StartDate", it->StartDate));
        singleRequirement.appendChild(makeElement(doc, "EndDate", it->EndDate));
        singleRequirement.appendChild(makeElement(doc, "Position", it->Position));
        singleRequirement.appendChild(makeElement(doc, "NumberWorkers", QString::number(it->NumberWorkers)));
        //singleRequirement.appendChild(makeElement(doc, "Severity", QString::number(it->Severity)));
        //singleRequirement.appendChild(makeElement(doc, "BiggerAmountWorkers", QString::number(it->BiggerAmountWorkers)));
        //singleRequirement.appendChild(makeElement(doc, "Neglect", QString::number(it->Neglect)));

        rootElem.appendChild(singleRequirement);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveVacations(const QString& filename, const Vacations& info)
{
    QDomDocument doc("Vacations");
    QDomElement rootElem = doc.createElement("Vacations");
    doc.appendChild(rootElem);

    for (auto it = info.begin(); it != info.end(); it++)
    {
        QDomElement singleVacation = doc.createElement("SingleVacation");

        singleVacation.appendChild(makeElement(doc, "ID_Vacation", QString::number(it->ID_vacation)));
        singleVacation.appendChild(makeElement(doc, "ID_Worker", QString::number(it->ID_worker)));
        singleVacation.appendChild(makeElement(doc, "StartDate", it->StartDate));
        singleVacation.appendChild(makeElement(doc, "EndDate", it->EndDate));

        rootElem.appendChild(singleVacation);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveWishes(const QString& filename, const Wishes& info)
{
    QDomDocument doc("WishesWorkers");
    QDomElement rootElem = doc.createElement("Wishes");
    doc.appendChild(rootElem);

    for (auto it = info.begin(); it != info.end(); it++)
    {
        QDomElement singleWish = doc.createElement("SingleWish");

        singleWish.appendChild(makeElement(doc, "ID_Wishes", QString::number(it->ID_Wishes)));
        singleWish.appendChild(makeElement(doc, "ID_Workers", QString::number(it->ID_Workers)));
        singleWish.appendChild(makeElement(doc, "StartDate", it->StartDate));
        singleWish.appendChild(makeElement(doc, "EndDate", it->EndDate));
        singleWish.appendChild(makeElement(doc, "Position", it->Position));
        singleWish.appendChild(makeElement(doc, "Addition", it->Addition));

        rootElem.appendChild(singleWish);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveWorker(const QString& filename, const Worker& info)
{
    QDomDocument doc("WorkerInformation");
    QDomElement rootElem = doc.createElement("Worker");
    doc.appendChild(rootElem);

    rootElem.appendChild(makeElement(doc, "ID_worker", QString().number(info.ID_worker)));
    rootElem.appendChild(makeElement(doc, "Surname", info.Surname));
    rootElem.appendChild(makeElement(doc, "FirstName", info.FirstName));
    rootElem.appendChild(makeElement(doc, "LastName", info.LastName));
    rootElem.appendChild(makeElement(doc, "PhoneNumber", info.PhoneNumber));
    rootElem.appendChild(makeElement(doc, "Email", info.Email));

    for (auto it : info.ListPositions)
    {
        rootElem.appendChild(makeElement(doc, "Position", it));
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveWorkShifts(const QString& filename, const Shifts& info)
{
    QDomDocument doc("WorkShifts");
    QDomElement rootElem = doc.createElement("Shifts");
    doc.appendChild(rootElem);

    for (auto it = info.begin(); it != info.end(); it++)
    {
        QDomElement singleShift = doc.createElement("SingleShift");

        singleShift.appendChild(makeElement(doc, "ID_Shift", QString::number(it->ID_Shifts)));
        //singleShift.appendChild(makeElement(doc, "ID_Workers", QString::number(it->ID_Workers)));
        singleShift.appendChild(makeElement(doc, "StartDate", it->StartDate));
        singleShift.appendChild(makeElement(doc, "EndDate", it->EndDate));
        singleShift.appendChild(makeElement(doc, "Position", it->Position));

        rootElem.appendChild(singleShift);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

void ProjectDataSaver::saveAssignedShifts(const QString& filename, const AssignedShifts& info)
{
    QDomDocument doc("AssignedShifts");
    QDomElement rootElem = doc.createElement("AssignedShifts");
    doc.appendChild(rootElem);

    for (auto it = info.begin(); it != info.end(); it++)
    {
        QDomElement singleShift = doc.createElement("SingleAssignedShift");

        singleShift.appendChild(makeElement(doc, "ID_Shift", QString::number(it->ID_Shift)));
        singleShift.appendChild(makeElement(doc, "ID_Worker", QString::number(it->ID_Worker)));
        rootElem.appendChild(singleShift);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}


QDomElement ProjectDataSaver::makeElement(QDomDocument& doc, const QString &tagName, const QString &tagValue)
{
    QDomElement elem = doc.createElement(tagName);

    if (!tagValue.isEmpty())
    {
        QDomText text = doc.createTextNode(tagValue);
        elem.appendChild(text);
    }

    return elem;
}
