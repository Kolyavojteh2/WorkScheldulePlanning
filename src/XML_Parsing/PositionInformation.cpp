#include "xml_parsing.h"

Positions readPositions(QDomDocument& doc)
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
        allPositions.list.insert(info_singlePosition.Brief, info_singlePosition);

        node_singlePosition = node_singlePosition.nextSibling();
    }
    return allPositions;
}

// EXPORT
Positions read_PositionsFromFile(const QString& filename)
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

            info = readPositions(doc);
        }
        file.close();
    }

    return info;
}

// EXPORT
void write_PositionsToFile(const QString& filename, const Positions& info)
{
    QDomDocument doc("Positions");
    QDomElement rootElem = doc.createElement("Positions");
    doc.appendChild(rootElem);

    for (auto it = info.list.begin(); it != info.list.end(); it++)
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
