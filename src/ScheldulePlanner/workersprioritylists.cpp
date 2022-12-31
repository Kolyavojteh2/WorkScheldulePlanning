#include "workersprioritylists.h"
#include <QtMath>

WorkersPriorityLists::WorkersPriorityLists(const Vacations& vacations,
                                           const Shifts& shifts,
                                           const AssignedShifts& assigned,
                                           const Wishes& wishes,
                                           const Workers& workers,
                                           const ProjectSetup& setup)
    : m_vacations(vacations), m_shifts(shifts), m_assigned(assigned), m_wishes(wishes), m_workers(workers), m_setupEnterprise(setup) {}

bool WorkersPriorityLists::inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max)
{
    if (value >= min && value <= max)
        return true;
    return false;
}

void WorkersPriorityLists::RedList(const SingleShift& shift)
{
    QDateTime startDate = QDateTime::fromString(shift.StartDate);
    QDateTime endDate = QDateTime::fromString(shift.EndDate);

// Заповнення червоного списку(множини не працюючих людей на даній зміні)
    // Відпустка/відгул
    for (auto it_vacation : m_vacations)
    {
        // Якщо початок зміни чи її кінець є під час відпустки робітника, то він на роботу не вийде
        if (inRangeDateTime(startDate, QDateTime::fromString(it_vacation.StartDate), QDateTime::fromString(it_vacation.EndDate)) ||
                inRangeDateTime(endDate, QDateTime::fromString(it_vacation.StartDate), QDateTime::fromString(it_vacation.EndDate)))
        {
            m_redList.insert(it_vacation.ID_worker);
        }
    }

    // Тепер шукаємо працівників, що були на роботі менше ніж мінімальний час відпочинку(10 годин тому)
    // або мають зміну після закінчення цієї зміни раніше ніж через мінімальний час відпочинку.
    // Виконуємо обхід по всіх існуючих змінах
    // Мінімальна та стандартна тривалість відпочинку в секундах
    QTime minimumRest = QTime::fromString(m_setupEnterprise.MinRestTime);
    int minimumRestSeconds = minimumRest.hour() * 60 * 60 + minimumRest.minute() * 60 + minimumRest.second();

    // Обхід по призначених змінах
    for (auto it_assigned = m_assigned.begin(); it_assigned != m_assigned.end(); it_assigned++)
    {
        auto iteratorOnShift = m_shifts.find(QString::number(it_assigned.value().ID_Shift));
        if (iteratorOnShift == m_shifts.end())
            continue;

        QDateTime startIterShift = QDateTime::fromString(iteratorOnShift.value().StartDate);
        QDateTime endIterShift = QDateTime::fromString(iteratorOnShift.value().EndDate);

        if (/*endDate.secsTo(startIterShift) >= 0 &&*/
                qAbs(endDate.secsTo(startIterShift)) < minimumRestSeconds)
        {
            m_redList.insert(it_assigned.value().ID_Worker);
        }

        if (/*endIterShift.secsTo(startDate) >= 0 &&*/
                qAbs(endIterShift.secsTo(startDate)) < minimumRestSeconds)
        {
            m_redList.insert(it_assigned.value().ID_Worker);
        }
    }


    /*
    for (auto it_shift = m_shifts.begin(); it_shift != m_shifts.end(); it_shift++)
    {
        // Допоміжні змінні
        // Переведення мінімального часу для відпочинку в секунди
        QTime minRest = QTime::fromString(m_setupEnterprise.MinRestTime);
        int minRestSeconds = minRest.hour() * 60 * 60 + minRest.minute() * 60 + minRest.second();

        // Визначення точок часу, які стосуються цієї зміни
        QDateTime StartAreaShift = startDate.addSecs(-minRestSeconds);
        QDateTime EndAreaShift = endDate.addSecs(minRestSeconds);

        // Якщо поточна зміна(та що в формі) знаходиться між цими точками часу,
        // то шукаємо дані про робітника
        // і додаємо його до червоного списку
        if (inRangeDateTime(QDateTime::fromString(it_shift.value().StartDate), StartAreaShift, EndAreaShift) ||
                inRangeDateTime(QDateTime::fromString(it_shift.value().EndDate), StartAreaShift, EndAreaShift))
        {
            m_redList.insert(it_shift.value().ID_Workers);
        }
    }
    */

    // Перевищення кількості годин на період
    // Проход по всіх робітниках
    for (auto it_workers = m_workers.begin(); it_workers != m_workers.end(); it_workers++)
    {
        // Константа секунд в годині та лічильник секунд
        const int SecondsInHour = 60*60;
        int summaryDurationInSeconds = 0;

        // Додаємо тривалість поточної зміни до лічильника
        summaryDurationInSeconds += startDate.secsTo(endDate);

        // Прохід по всіх призначених змінах
        for (auto it_assigned = m_assigned.begin(); it_assigned != m_assigned.end(); it_assigned++)
        {
            // Якщо призначена зміна для поточнного робітника, то робимо перевірки
            // Якщо це не зміна робітника, то йдемо далі
            if (it_assigned.value().ID_Worker == it_workers.value().ID_worker)
            {
                // Шукаємо зміну, якщо такої немає, то проходимо далі
                auto iteratorOnShift = m_shifts.find(QString::number(it_assigned.value().ID_Shift));
                if (iteratorOnShift == m_shifts.end())
                    continue;
                else
                {
                    // Визначаємо початок та кінець призначеної зміни, для подальшого отримання її тривалості
                    QDateTime start = QDateTime::fromString(iteratorOnShift.value().StartDate);
                    QDateTime end = QDateTime::fromString(iteratorOnShift.value().EndDate);

                    // Визначаємо тривалість зміни
                    summaryDurationInSeconds += start.secsTo(end);

                    // Якщо тривалість зміни більша чи рівна максимуму, то робітник не буде працювати на цій зміні
                    if ((summaryDurationInSeconds / SecondsInHour) >= m_setupEnterprise.MaxHoursPerPeriod)
                    {
                        m_redList.insert(it_workers.value().ID_worker);
                        break;
                    }
                }
            }
        }
    }
}

void WorkersPriorityLists::GreenList(const SingleShift& shift)
{
    QDateTime startDate = QDateTime::fromString(shift.StartDate);
    QDateTime endDate = QDateTime::fromString(shift.EndDate);

    // Заповнення зеленого списку(побажання робітника)
    for (auto it_wishes : m_wishes)
    {
        // Перевіряємо чи початок побажання і кінець побажання входять в діапазон роботи цієї зміни, то ми додаємо робітника до зеленого списку
        QDateTime startWishDate = QDateTime::fromString(it_wishes.StartDate);
        QDateTime endWishDate = QDateTime::fromString(it_wishes.EndDate);



        if (inRangeDateTime(startDate, startWishDate, endWishDate) &&
                inRangeDateTime(endDate, startWishDate, endWishDate))
        //if (inRangeDateTime(QDateTime::fromString(it_wishes.StartDate), startDate, endDate) &&
        //        inRangeDateTime(QDateTime::fromString(it_wishes.EndDate), startDate, endDate))
        {
            m_greenList.insert(it_wishes.ID_Workers);
        }
    }

    m_greenList.subtract(m_redList);
    m_greenList.subtract(m_yellowList);
}

void WorkersPriorityLists::YellowList(const SingleShift& shift)
{
    // Заповнення жовтого списку(люди що відпочили більше ніж мінімальна кількість годин, але менше ніж нормальна

    QDateTime startDate = QDateTime::fromString(shift.StartDate);
    QDateTime endDate = QDateTime::fromString(shift.EndDate);

    // Мінімальна та стандартна тривалість відпочинку в секундах
    QTime standartRest = QTime::fromString(m_setupEnterprise.StandartRestTime);
    int standartRestSeconds = standartRest.hour() * 60 * 60 + standartRest.minute() * 60 + standartRest.second();

    QTime minimumRest = QTime::fromString(m_setupEnterprise.MinRestTime);
    int minimumRestSeconds = minimumRest.hour() * 60 * 60 + minimumRest.minute() * 60 + minimumRest.second();

    // Обхід по призначених змінах
    for (auto it_assigned = m_assigned.begin(); it_assigned != m_assigned.end(); it_assigned++)
    {
        auto iteratorOnShift = m_shifts.find(QString::number(it_assigned.value().ID_Shift));
        if (iteratorOnShift == m_shifts.end())
            continue;

        QDateTime startIterShift = QDateTime::fromString(iteratorOnShift.value().StartDate);
        QDateTime endIterShift = QDateTime::fromString(iteratorOnShift.value().EndDate);

        if (endDate.secsTo(startIterShift) > 0 &&
                endDate.secsTo(startIterShift) > minimumRestSeconds &&
                endDate.secsTo(startIterShift) < standartRestSeconds)
        {
            m_yellowList.insert(it_assigned.value().ID_Worker);
        }

        if (endIterShift.secsTo(startDate) > 0 &&
                endIterShift.secsTo(startDate) > minimumRestSeconds &&
                endIterShift.secsTo(startDate) < standartRestSeconds)
        {
            m_yellowList.insert(it_assigned.value().ID_Worker);
        }
    }

    m_yellowList.subtract(m_redList);
}

void WorkersPriorityLists::CommonList()
{
    // Заповнення звичайного списку(всі решта хто не потрапив у якусь категорію)
    for (auto it : m_workers)
    {
        m_commonList.insert(it.ID_worker);
    }

    m_commonList.subtract(m_greenList);
    m_commonList.subtract(m_yellowList);
    m_commonList.subtract(m_redList);
}

QList<QString> WorkersPriorityLists::getWorkersListPriority(const SingleShift& shift)
{
    selectWorkers(shift);

    RedList(shift);
    YellowList(shift);
    GreenList(shift);
    CommonList();

    QList<QString> result;
    for (auto it : m_greenList)
    {
        result.push_back(greenText + " ID:"+ QString::number(it));
    }

    for (auto it : m_commonList)
    {
        result.push_back("ID:"+ QString::number(it));
    }

    for (auto it : m_yellowList)
    {
        result.push_back(yellowText + " ID:"+ QString::number(it));
    }

    return result;
}

void WorkersPriorityLists::selectWorkers(const SingleShift& shift)
{
    Workers newWorkersList;

    for (auto it_worker = m_workers.begin(); it_worker != m_workers.end(); it_worker++)
    {
        bool isExists = false;
        for (auto pos : it_worker.value().ListPositions)
        {
            if (shift.Position == pos)
            {
                isExists = true;
            }
        }

        if (isExists)
        {
            newWorkersList.insert(it_worker.key(), it_worker.value());
        }
    }

    m_workers = newWorkersList;
}

const QString WorkersPriorityLists::getGreenText() const
{
    return greenText;
}

const QString WorkersPriorityLists::getYellowText() const
{
    return yellowText;
}

int WorkersPriorityLists::stringToID(const QString& str)
{
    QString strID = " ID:";
    QString result = str;

    int pos = str.indexOf(strID, 0, Qt::CaseSensitivity::CaseInsensitive);
    result.remove(0, pos + strID.size());

    return result.toInt();
}
