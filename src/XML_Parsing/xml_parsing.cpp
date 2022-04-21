#include "xml_parsing.h"
#include <QtXml/QtXml>
#include <QFile>
#include <QTextStream>

Worker traverseNodeWorker(const QDomNode &node, Worker info)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "ID_worker")
                {
                    info.ID_worker = domElement.text().toInt();
                }
                if (domElement.tagName() == "Surname")
                {
                    info.Surname = domElement.text();
                }
                if (domElement.tagName() == "FirstName")
                {
                    info.FirstName = domElement.text();
                }
                if (domElement.tagName() == "LastName")
                {
                    info.LastName = domElement.text();
                }
                if (domElement.tagName() == "PhoneNumber")
                {
                    info.PhoneNumber = domElement.text();
                }
                if (domElement.tagName() == "Email")
                {
                    info.Email = domElement.text();
                }
                if (domElement.tagName() == "Position")
                {
                    info.ListPositions.push_back(domElement.text());
                }
            }
        }

        info = traverseNodeWorker(domNode, info);
        domNode = domNode.nextSibling();
    }

    return info;
}

QDomElement makeElement(QDomDocument& doc, const QString &tagName, const QString &tagValue)
{
    QDomElement elem = doc.createElement(tagName);

    if (!tagValue.isEmpty())
    {
        QDomText text = doc.createTextNode(tagValue);
        elem.appendChild(text);
    }

    return elem;
}

// EXPORT
Worker read_WorkerInfoFromFile(const QString filename)
{
    QDomDocument doc;
    QFile file(filename);
    Worker info;

    if (file.open(QIODevice::ReadOnly))
    {
        if (doc.setContent(&file))
        {
            QDomElement domElement = doc.documentElement();
            info = traverseNodeWorker(domElement, info);
        }
        file.close();
    }

    return info;
}

// EXPORT
void write_WorkerInfoToFile(const QString filename, const Worker info)
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

