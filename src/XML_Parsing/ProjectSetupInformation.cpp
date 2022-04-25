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
        if (elem_attribute.tagName() == "RoundTheClockMode")
        {
            setup.RoundTheClockMode = elem_attribute.text().toInt();
        }
        if (elem_attribute.tagName() == "OpeningCorporation")
        {
            setup.OpeningCorporation = elem_attribute.text();
            if (setup.OpeningCorporation.isEmpty())
            {
                setup.OpeningCorporation = "00:00:00";
            }
        }
        if (elem_attribute.tagName() == "ClosingCorporation")
        {
            setup.ClosingCorporation = elem_attribute.text();
            if (setup.ClosingCorporation.isEmpty())
            {
                setup.ClosingCorporation = "00:00:00";
            }
        }
        if (elem_attribute.tagName() == "EveryMounthGraphic")
        {
            setup.EveryMounthGraphic = elem_attribute.text().toInt();
        }
        if (elem_attribute.tagName() == "GraphicForeverDay")
        {
            if (setup.EveryMounthGraphic != 0)
            {
                setup.GraphicForeverDay = QDate::currentDate().daysInMonth();
            }
            else
            {
                setup.GraphicForeverDay = elem_attribute.text().toInt();
            }
        }
        if (elem_attribute.tagName() == "MinRestTime")
        {
            setup.MinRestTime = elem_attribute.text();
        }
        if (elem_attribute.tagName() == "StandartRestTime")
        {
            setup.StandartRestTime = elem_attribute.text();
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

    rootElem.appendChild(makeElement(doc, "RoundTheClockMode", QString::number(info.RoundTheClockMode)));
    if (info.RoundTheClockMode != 0)
    {
        rootElem.appendChild(makeElement(doc, "OpeningCorporation", ""));
        rootElem.appendChild(makeElement(doc, "ClosingCorporation", ""));
    }
    else
    {
        rootElem.appendChild(makeElement(doc, "OpeningCorporation", info.OpeningCorporation));
        rootElem.appendChild(makeElement(doc, "ClosingCorporation", info.ClosingCorporation));
    }


    rootElem.appendChild(makeElement(doc, "EveryMounthGraphic", QString::number(info.EveryMounthGraphic)));
    if (info.EveryMounthGraphic != 0)
    {
        rootElem.appendChild(makeElement(doc, "GraphicForeverDay", ""));
    }
    else
    {
        rootElem.appendChild(makeElement(doc, "GraphicForeverDay", QString::number(info.GraphicForeverDay)));
    }

    rootElem.appendChild(makeElement(doc, "MinRestTime", info.MinRestTime));
    rootElem.appendChild(makeElement(doc, "StandartRestTime", info.StandartRestTime));

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}
