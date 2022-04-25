#include "xml_parsing.h"

Wishes readWishes(QDomDocument& doc)
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
            if (elem_attribute.tagName() == "MinWorkerDays")
            {
                info_singleWish.MinWorkerDays = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "MaxWorkerDay")
            {
                info_singleWish.MaxWorkerDay = elem_attribute.text().toInt();
            }
            if (elem_attribute.tagName() == "GetStartedLaterThan")
            {
                info_singleWish.GetStartedLaterThan = elem_attribute.text();
            }
            if (elem_attribute.tagName() == "GetEndEarlierThan")
            {
                info_singleWish.GetEndEarlierThan = elem_attribute.text();
            }

            node_attribute = node_attribute.nextSibling();
        }
        allWishes.insert(QString::number(info_singleWish.ID_Wishes), info_singleWish);

        node_singleWish = node_singleWish.nextSibling();
    }
    return allWishes;
}

Wishes read_WishesFromFile(const QString& filename)
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

            info = readWishes(doc);
        }
        file.close();
    }

    return info;
}

void write_WishesToFile(const QString& filename, const Wishes& info)
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
        singleWish.appendChild(makeElement(doc, "MinWorkerDays", QString::number(it->MinWorkerDays)));
        singleWish.appendChild(makeElement(doc, "MaxWorkerDay", QString::number(it->MaxWorkerDay)));
        singleWish.appendChild(makeElement(doc, "GetStartedLaterThan", it->GetStartedLaterThan));
        singleWish.appendChild(makeElement(doc, "GetEndEarlierThan", it->GetEndEarlierThan));

        rootElem.appendChild(singleWish);
    }

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream(&file) << doc.toString();
        file.close();
    }
}
