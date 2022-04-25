#include "xml_parsing.h"

Vacations readVacations(QDomDocument& doc)
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

Vacations read_VacationsFromFile(const QString& filename)
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

            info = readVacations(doc);
        }
        file.close();
    }

    return info;
}

void write_VacationsToFile(const QString& filename, const Vacations& info)
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
