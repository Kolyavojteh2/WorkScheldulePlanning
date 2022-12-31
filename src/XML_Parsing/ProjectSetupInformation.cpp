#include "xml_parsing.h"

ProjectSetup readProjectSetup(QDomDocument& doc)
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

ProjectSetup read_ProjectSetupFromFile(const QString& filename)
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

            info = readProjectSetup(doc);
        }
        file.close();
    }

    return info;
}

void write_ProjectSetupToFile(const QString& filename, const ProjectSetup& info)
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
