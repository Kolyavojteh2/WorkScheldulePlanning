#include "xml_parsing.h"

Shifts readShifts(QDomDocument& doc)
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

Shifts read_ShiftsFromFile(const QString& filename)
{
    QDomDocument doc;
    QFile file(filename);
    Shifts info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            if (doc.doctype().name() != "WishesWorkers")
                return info;

            info = readShifts(doc);
        }
        file.close();
    }

    return info;
}

void write_ShiftsToFile(const QString& filename, const Shifts& info)
{
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
}
