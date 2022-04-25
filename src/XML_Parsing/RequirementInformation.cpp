#include "xml_parsing.h"

Requirements readRequirements(QDomDocument& doc)
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
            if (elem_attribute.tagName() == "Severity")
            {
                info_singleRequirement.Severity = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "BiggerAmountWorkers")
            {
                info_singleRequirement.BiggerAmountWorkers = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "Neglect")
            {
                info_singleRequirement.Neglect = elem_attribute.text().toInt();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allRequirements.insert(QString::number(info_singleRequirement.ID), info_singleRequirement);

        node_singleRequirement = node_singleRequirement.nextSibling();
    }
    return allRequirements;
}

Requirements read_RequirementsFromFile(const QString& filename)
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

            info = readRequirements(doc);
        }
        file.close();
    }

    return info;
}

void write_RequirementsToFile(const QString& filename, const Requirements& info)
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
        singleRequirement.appendChild(makeElement(doc, "Severity", QString::number(it->Severity)));
        singleRequirement.appendChild(makeElement(doc, "BiggerAmountWorkers", QString::number(it->BiggerAmountWorkers)));
        singleRequirement.appendChild(makeElement(doc, "Neglect", QString::number(it->Neglect)));

        rootElem.appendChild(singleRequirement);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}

