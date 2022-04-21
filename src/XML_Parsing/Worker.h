#ifndef WORKER_H
#define WORKER_H

#include <QString>
#include <QList>

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

    bool operator==(Worker r)
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

    bool operator!=(Worker r)
    {
        if(*this == r)
            return false;
        return true;
    }
};

#endif // WORKER_H