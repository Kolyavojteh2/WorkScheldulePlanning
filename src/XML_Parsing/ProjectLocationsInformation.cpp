#include "ProjectLocations.h"
#include "xml_parsing.h"

ProjectLocations readLocations(QDomDocument& doc)
{
    // ProjectLocations
    QDomElement rootElement = doc.documentElement();
    ProjectLocations locations;
    // Attributes
    QDomNode node_attribute = rootElement.firstChild();
    while(!node_attribute.isNull() && node_attribute.isElement())
    {
        QDomElement elem_attribute = node_attribute.toElement();
        if (elem_attribute.tagName() == "PositionsLocation")
        {
            locations.PositionsLocation = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "WorkGraphicLocation")
        {
            locations.WorkGraphicLocation = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "RequirementsLocation")
        {
            locations.RequirementsLocation = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "WishesWorkersLocation")
        {
            locations.WishesWorkersLocation = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "WorkerGraphicLocation")
        {
            locations.WorkerGraphicLocation = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "WorkersInformationLocation")
        {
            locations.WorkersInformationLocation = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "VacationInformationLocation")
        {
            locations.VacationInformationLocation = elem_attribute.text();
        }

        node_attribute = node_attribute.nextSibling();
    }

    return locations;
}

ProjectLocations read_ProjectLocationsFromFile(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    ProjectLocations info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "ProjectLocations")
                return info;

            info = readLocations(doc);
        }
        file.close();
    }

    return info;
}

void write_ProjectLocationsToFile(const QString& filename, const ProjectLocations& info)
{
    QDomDocument doc("ProjectLocations");
    QDomElement rootElem = doc.createElement("ProjectLocations");
    doc.appendChild(rootElem);

    rootElem.appendChild(makeElement(doc, "RequirementsLocation", info.RequirementsLocation));
    rootElem.appendChild(makeElement(doc, "WishesWorkersLocation", info.WishesWorkersLocation));
    rootElem.appendChild(makeElement(doc, "WorkersInformationLocation", info.WorkersInformationLocation));
    rootElem.appendChild(makeElement(doc, "VacationInformationLocation", info.VacationInformationLocation));
    rootElem.appendChild(makeElement(doc, "WorkGraphicLocation", info.WorkGraphicLocation));
    rootElem.appendChild(makeElement(doc, "WorkerGraphicLocation", info.WorkerGraphicLocation));
    rootElem.appendChild(makeElement(doc, "PositionsLocation", info.PositionsLocation));

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}
