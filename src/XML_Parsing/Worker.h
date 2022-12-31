#ifndef WORKER_H
#define WORKER_H

#include <QString>
#include <QList>
#include <QMap>

class Worker
{
public:
    int ID_worker = 0;
    QString Surname;
    QString FirstName;
    QString LastName;
    QString PhoneNumber;
    QString Email;
    QList<QString> ListPositions;

    bool operator==(const Worker& r) const
    {
        if (ID_worker == r.ID_worker &&
                Surname == r.Surname &&
                FirstName == r.FirstName &&
                LastName == r.LastName &&
                PhoneNumber == r.PhoneNumber &&
                Email == r.Email &&
                ListPositions == r.ListPositions)
            return true;
        return false;
    }

    bool operator!=(const Worker& r) const
    {
        if(*this == r)
            return false;
        return true;
    }
};

typedef QMap<QString, Worker> Workers;

#endif // WORKER_H
