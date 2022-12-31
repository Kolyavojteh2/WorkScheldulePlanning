#include "automatedalgorithmsbuildingschedules.h"

Shifts AutomatedAlgorithmsBuildingSchedules::MinStandart( const DurationsOfWorkShift& durations,
                                                          const ProjectSetup& configuration,
                                                          const Requirements& requirements,
                                                          const QList<QString>& positions,
                                                          const Shifts& existingShifts)
{
    Shifts resultShifts;
    //resultShifts = existingShifts;

    auto requirementsByPosition = convertRequirementsToMap(requirements, configuration, positions.toSet());

    // Оптимізація
    for (auto it = requirementsByPosition.begin(); it != requirementsByPosition.end(); )
    {
        if (it.value().isEmpty())
        {
            auto old_it = it;
            it++;
            requirementsByPosition.erase(old_it);
        }
        else
        {
            it++;
        }
    }

    for (auto it_position = requirementsByPosition.begin(); it_position != requirementsByPosition.end(); it_position++)
    {
        //Shifts currentPositionShifts =
        resultShifts = mergeShifts(resultShifts, createShiftsByDuration(durations.Minimum,
                                                                        durations.Standart,
                                                                        it_position.value(),
                                                                        it_position.key(),
                                                                        existingShifts));
    }

    return resultShifts;
}

Shifts AutomatedAlgorithmsBuildingSchedules::MinMax(const DurationsOfWorkShift& durations,
                                                    const ProjectSetup& configuration,
                                                    const Requirements& requirements,
                                                    const QList<QString>& positions,
                                                    const Shifts& existingShifts)
{
    Shifts resultShifts;
    //resultShifts = existingShifts;

    auto requirementsByPosition = convertRequirementsToMap(requirements, configuration, positions.toSet());

    for (auto it_position = requirementsByPosition.begin(); it_position != requirementsByPosition.end(); it_position++)
    {
        //Shifts currentPositionShifts =
        resultShifts = mergeShifts(resultShifts, createShiftsByDuration(durations.Minimum,
                                                                        durations.Maximum,
                                                                        it_position.value(),
                                                                        it_position.key(),
                                                                        existingShifts));
    }

    return resultShifts;
}

QMap<QString, QMap<QDateTime, int>> AutomatedAlgorithmsBuildingSchedules::convertRequirementsToMap(const Requirements& requirements,
                                                                                                    const ProjectSetup& configuration,
                                                                                                    const QSet<QString>& positions)
{
    QMap<QString, QMap<QDateTime, int>> result;

    for (auto it_posiiton : positions)
    {
        QMap<QDateTime, int> singlePosition;

        for (auto it_requrement : requirements)
        {
            if (it_requrement.Position != it_posiiton)
                continue;

            if (!inRangeDateTime(QDateTime::fromString(it_requrement.StartDate),
                                 QDateTime::fromString(configuration.StartPeriod),
                                 QDateTime::fromString(configuration.EndPeriod)) &&
                    !inRangeDateTime(QDateTime::fromString(it_requrement.EndDate),
                                     QDateTime::fromString(configuration.StartPeriod),
                                     QDateTime::fromString(configuration.EndPeriod)))
                continue;

            for (QDateTime i = QDateTime::fromString(it_requrement.StartDate);
                 i <= QDateTime::fromString(it_requrement.EndDate);
                 i = i.addSecs(StepSeconds))
            {
                if (singlePosition.find(i) != singlePosition.end())
                    singlePosition[i] += it_requrement.NumberWorkers;
                else
                    singlePosition.insert(i, it_requrement.NumberWorkers);
            }
        }

        result.insert(it_posiiton, singlePosition);
    }

    return result;
}

Shifts AutomatedAlgorithmsBuildingSchedules::createShiftsByDuration(const QTime& minDurationOfShift,
                                                                    const QTime& maxDurationOfShift,
                                                                    QMap<QDateTime, int> requirements,
                                                                    const QString& position,
                                                                    const Shifts& existingShifts)
{
    Shifts result = existingShifts;

    // Видалення існуючих змін, що не відповідають поточній позиції
    for (auto it = result.begin(); it != result.end();)
    {
        if (it.value().Position != position)
        {
            auto old_it = it;
            it++;
            result.erase(old_it);
        }
        else
        {
            it++;
        }
    }

    // Віднімання від вимог вже існуючих змін
    for (auto it : result)
    {
        QDateTime startRequirementTime = QDateTime::fromString(it.StartDate);
        QDateTime endRequirementTime = QDateTime::fromString(it.EndDate);

        // Subtract worker from requirements at this time
        for (QDateTime i = startRequirementTime; i < endRequirementTime; i = i.addSecs(StepSeconds))
        {
            requirements[i] -= 1;
        }
    }

    // Clean times where workers number less or equal 0
    for (auto it = requirements.begin(); it != requirements.end();)
    {
        auto old_it = it;
        it++;
        if (old_it.value() <= 0)
        {
            requirements.erase(old_it);
        }
    }

    int counterID = 0;
    for (auto it : result)
    {
        if (it.ID_Shifts > counterID)
        {
            counterID = it.ID_Shifts;
        }
    }
    counterID++;

    //int counterID = 1;

    while (!requirements.isEmpty())
    {
        QDateTime startRequirementTime = requirements.begin().key();
        QDateTime endRequirementTime = requirements.begin().key();

        while (requirements.contains(endRequirementTime.addSecs(StepSeconds)))
        {
            endRequirementTime = endRequirementTime.addSecs(StepSeconds);
        }

        qint64 durationInSecs = startRequirementTime.secsTo(endRequirementTime);

        // Функція для віднімання призначеної зміни та очистки дат із нульовою кількістю робітників
        auto cleaningFunc = [&]()
        {
            // Subtract worker from requirements at this time
            for (QDateTime i = startRequirementTime; i < endRequirementTime; i = i.addSecs(StepSeconds))
            {
                requirements[i] -= 1;
            }

            // Clean times where workers number less or equal 0
            for (auto it = requirements.begin(); it != requirements.end();)
            {
                auto old_it = it;
                it++;
                if (old_it.value() <= 0)
                {
                    requirements.erase(old_it);
                }
            }
        };

        // (duration) < (min)
        if (durationInSecs < timeToSeconds(minDurationOfShift))
        {
            // Subtract worker from requirements at this time
            for (QDateTime i = startRequirementTime; i <= endRequirementTime; i = i.addSecs(StepSeconds))
            {
                requirements[i] -= 1;
            }

            // Clean times where workers number equal 0
            for (auto it = requirements.begin(); it != requirements.end();)
            {
                auto old_it = it;
                it++;
                if (old_it.value() == 0)
                {
                    requirements.erase(old_it);
                }
            }
        }

        // (min) <= (duration) <= (max)
        if (timeToSeconds(minDurationOfShift) <= durationInSecs && durationInSecs <= timeToSeconds(maxDurationOfShift))
        {
            SingleShift shift;
            shift.ID_Shifts = counterID;
            shift.StartDate = startRequirementTime.toString();
            shift.EndDate = endRequirementTime.toString();
            shift.Position = position;

            result.insert(QString::number(shift.ID_Shifts), shift);
            counterID++;

            cleaningFunc();
            continue;
        }

        // (max) < (duration) < (max + min)
        if (timeToSeconds(maxDurationOfShift) < durationInSecs && durationInSecs < (timeToSeconds(maxDurationOfShift) + timeToSeconds(minDurationOfShift)))
        {
            ///////////
            durationInSecs /= 2;
            int endShiftHours = durationInSecs / (60 * 60);
            endRequirementTime = startRequirementTime.addSecs(endShiftHours * 60 * 60);

            SingleShift shift;
            shift.ID_Shifts = counterID;
            shift.StartDate = startRequirementTime.toString();
            shift.EndDate = endRequirementTime.toString();
            shift.Position = position;

            result.insert(QString::number(shift.ID_Shifts), shift);
            counterID++;

            cleaningFunc();
            continue;
        }

        // (max + min) <= (duration)
        if ((timeToSeconds(maxDurationOfShift) + timeToSeconds(minDurationOfShift)) <= durationInSecs)
        {
            endRequirementTime = startRequirementTime.addSecs(timeToSeconds(maxDurationOfShift));

            SingleShift shift;
            shift.ID_Shifts = counterID;
            shift.StartDate = startRequirementTime.toString();
            shift.EndDate = endRequirementTime.toString();
            shift.Position = position;

            result.insert(QString::number(shift.ID_Shifts), shift);
            counterID++;

            cleaningFunc();
            continue;
        }
    }

    return result;
}

qint64 AutomatedAlgorithmsBuildingSchedules::timeToSeconds(const QTime& time)
{
    int result = time.second();
    result += time.minute() * 60;
    result += time.hour() * 60 * 60;

    return result;
}

Shifts AutomatedAlgorithmsBuildingSchedules::mergeShifts(Shifts first, Shifts second)
{
    for (auto it = second.begin(); it != second.end();)
    {
        if (first.find(QString::number(it.value().ID_Shifts)) != first.end())
        {
            int newID = findLastShiftID(first) + 1;
            first.insert(QString::number(newID), it.value());
            first[QString::number(newID)].ID_Shifts = newID;

            auto delete_iterator = it;
            it++;
            second.erase(delete_iterator);
        }
        else
        {
            first.insert(it.key(), it.value());

            auto delete_iterator = it;
            it++;
            second.erase(delete_iterator);
        }
    }
    return first;
}

int AutomatedAlgorithmsBuildingSchedules::findLastShiftID(const Shifts& list)
{
    int result = 0;
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (result < it.value().ID_Shifts)
            result = it.value().ID_Shifts;
    }

    return result;
}

bool AutomatedAlgorithmsBuildingSchedules::inRangeDateTime(const QDateTime& value,
                                                           const QDateTime& start,
                                                           const QDateTime& end)
{
    if (start <= value && value < end)
        return true;
    else
        return false;
}

AssignedShifts
AutomatedAlgorithmsBuildingSchedules::AssigningShiftsToTheMostVulnerableShift(Shifts shifts,
                                                                              const Wishes& wishes,
                                                                              const Workers& workers,
                                                                              const Vacations& vacations,
                                                                              const AssignedShifts& assigned,
                                                                              const ProjectSetup& setup)
{
    AssignedShifts assignedResult = assigned;

    // Запуск основного циклу, він триває до тих пір, доки ще можна присвоїти робочу зміну
    while (1)
    {
        QMap<int, int> workersPosibility;

        // Проходимо по всім змінах і отримуємо кількість людей що можуть взяти цю робочу зміну
        // Також заповнюємо словник можливостей робітника
        QMap<int /*ID_shift*/, int/*number*/> numberWorkersPerShift;
        for (auto it_shift : shifts)
        {
            if (assignedResult.find(QString::number(it_shift.ID_Shifts)) != assignedResult.end())
                continue;

            WorkersPriorityLists priorityList(vacations, shifts, assignedResult, wishes, workers, setup);
            auto list = priorityList.getWorkersListPriority(it_shift);
            int number = list.size();

            for (auto it : list)
            {
                workersPosibility[WorkersPriorityLists::stringToID(it)] += 1;
            }

            numberWorkersPerShift.insert(it_shift.ID_Shifts, number);
        }

        // Якщо немає робочих змін, то повертаємо значення, роботу виконано
        if (numberWorkersPerShift.isEmpty())
            return assignedResult;

        // Видаляємо із цього списку робочі зміни, яким неможливо присвоїти робітника
        for (auto it = numberWorkersPerShift.begin(); it != numberWorkersPerShift.end();)
        {
            auto old_it = it;
            it++;
            if (old_it.value() == 0)
                numberWorkersPerShift.erase(old_it);
        }

        // Якщо після очистки не залишилось робочих змін, то повертаємо значення, роботу виконано частково
        if (numberWorkersPerShift.isEmpty())
            return assignedResult;

        // Пошук вразливого моменту, результатом є ID робочої зміни із найменшою кількістю людей
        QPair<int, int> vulnerableValue;
        vulnerableValue.first = numberWorkersPerShift.begin().key();
        vulnerableValue.second = numberWorkersPerShift.begin().value();
        for (auto it = ++numberWorkersPerShift.begin(); it != numberWorkersPerShift.end(); it++)
        {
            if (it.value() < vulnerableValue.second)
            {
                vulnerableValue.first = it.key();
                vulnerableValue.second = it.value();
            }
        }

        // Формуємо чергу із робітників в залежності від їхньої можливості отримати зміну.
        // Робітники із найменшою кількістю можливостей, отримують зміну першими
        QMap<int/*posibility*/, QList<int>/*list with ID workers*/> queueWorkers;
        for (auto iter_worker = workersPosibility.begin(); iter_worker != workersPosibility.end(); iter_worker++)
        {
            queueWorkers[iter_worker.value()].push_back(iter_worker.key());
        }

        // Отримуємо список робітників для вразливої зміни
        WorkersPriorityLists priorityList(vacations, shifts, assignedResult, wishes, workers, setup);
        auto listOfWorkersForShift = priorityList.getWorkersListPriority(shifts.find(QString::number(vulnerableValue.first)).value());

        // Робимо спробу поставити на робочу зміну робітника із бажанням
        if (!listOfWorkersForShift.filter(priorityList.getGreenText(), Qt::CaseSensitivity::CaseInsensitive).isEmpty())
        {
            QList<QString> workersWithWish = listOfWorkersForShift.filter(priorityList.getGreenText(), Qt::CaseSensitivity::CaseInsensitive);

            int ID_candidate = -1;
            for (auto it = queueWorkers.begin(); it != queueWorkers.end(); it++)
            {
                for (auto it_ID : it.value())
                {
                    for (auto it_list : workersWithWish)
                    {
                        if (it_ID == WorkersPriorityLists::stringToID(it_list))
                        {
                            ID_candidate = it_ID;
                            break;
                        }
                    }

                    if (ID_candidate != -1)
                        break;
                }

                if (ID_candidate != -1)
                    break;
            }

            /*
            QString workerTextFromList = listOfWorkersForShift.filter(priorityList.getGreenText(), Qt::CaseSensitivity::CaseInsensitive).first();
            workerTextFromList.remove(0, priorityList.getGreenText().size() + QString(" ID:").size());
            */

            SingleAssignedShift singleAssignment;
            singleAssignment.ID_Shift = vulnerableValue.first;
            //singleAssignment.ID_Worker = workerTextFromList.toInt();
            singleAssignment.ID_Worker = ID_candidate;

            assignedResult.insert(QString::number(singleAssignment.ID_Shift), singleAssignment);

            continue;
        }

        // Cтавимо на робочу зміну звичайного робітника або небажаного
        {
            QList<QString> commonWorkers = listOfWorkersForShift;

            int ID_candidate = -1;
            for (auto it = queueWorkers.begin(); it != queueWorkers.end(); it++)
            {
                for (auto it_ID : it.value())
                {
                    for (auto it_list : commonWorkers)
                    {
                        if (it_ID == WorkersPriorityLists::stringToID(it_list))
                        {
                            ID_candidate = it_ID;
                            break;
                        }
                    }

                    if (ID_candidate != -1)
                        break;
                }

                if (ID_candidate != -1)
                    break;
            }

            /*
            QString workerTextFromList = listOfWorkersForShift.first();
            QString strStartID = "ID:";
            QRegExp regexp(strStartID);
            workerTextFromList.remove(0, regexp.indexIn(strStartID) + strStartID.size());
            //workerTextFromList.remove(0, priorityList.getGreenText().size() + QString(" ID:").size());
*/
            SingleAssignedShift singleAssignment;
            singleAssignment.ID_Shift = vulnerableValue.first;
            //singleAssignment.ID_Worker = workerTextFromList.toInt();
            singleAssignment.ID_Worker = ID_candidate;

            assignedResult.insert(QString::number(singleAssignment.ID_Shift), singleAssignment);

            continue;
        }
    }
}


AssignedShifts
AutomatedAlgorithmsBuildingSchedules::AssigningShiftsToTheMostVulnerableShiftWithoutWishes(Shifts shifts,
                                                                                           const Wishes& wishes,
                                                              const Workers& workers,
                                                              const Vacations& vacations,
                                                              const AssignedShifts& assigned,
                                                              const ProjectSetup& setup)
{
    AssignedShifts assignedResult = assigned;

    // Запуск основного циклу, він триває до тих пір, доки ще можна присвоїти робочу зміну
    while (1)
    {
        QMap<int, int> workersPosibility;

        // Проходимо по всім змінах і отримуємо кількість людей що можуть взяти цю робочу зміну
        // Також заповнюємо словник можливостей робітника
        QMap<int /*ID_shift*/, int/*number*/> numberWorkersPerShift;
        for (auto it_shift : shifts)
        {
            if (assignedResult.find(QString::number(it_shift.ID_Shifts)) != assignedResult.end())
                continue;

            WorkersPriorityLists priorityList(vacations, shifts, assignedResult, wishes, workers, setup);
            auto list = priorityList.getWorkersListPriority(it_shift);
            int number = list.size();

            for (auto it : list)
            {
                workersPosibility[WorkersPriorityLists::stringToID(it)] += 1;
            }

            numberWorkersPerShift.insert(it_shift.ID_Shifts, number);
        }

        // Якщо немає робочих змін, то повертаємо значення, роботу виконано
        if (numberWorkersPerShift.isEmpty())
            return assignedResult;

        // Видаляємо із цього списку робочі зміни, яким неможливо присвоїти робітника
        for (auto it = numberWorkersPerShift.begin(); it != numberWorkersPerShift.end();)
        {
            auto old_it = it;
            it++;
            if (old_it.value() == 0)
                numberWorkersPerShift.erase(old_it);
        }

        // Якщо після очистки не залишилось робочих змін, то повертаємо значення, роботу виконано частково
        if (numberWorkersPerShift.isEmpty())
            return assignedResult;

        // Пошук вразливого моменту, результатом є ID робочої зміни із найменшою кількістю людей
        QPair<int, int> vulnerableValue;
        vulnerableValue.first = numberWorkersPerShift.begin().key();
        vulnerableValue.second = numberWorkersPerShift.begin().value();
        for (auto it = ++numberWorkersPerShift.begin(); it != numberWorkersPerShift.end(); it++)
        {
            if (it.value() < vulnerableValue.second)
            {
                vulnerableValue.first = it.key();
                vulnerableValue.second = it.value();
            }
        }

        // Формуємо чергу із робітників в залежності від їхньої можливості отримати зміну.
        // Робітники із найменшою кількістю можливостей, отримують зміну першими
        QMap<int/*posibility*/, QList<int>/*list with ID workers*/> queueWorkers;
        for (auto iter_worker = workersPosibility.begin(); iter_worker != workersPosibility.end(); iter_worker++)
        {
            queueWorkers[iter_worker.value()].push_back(iter_worker.key());
        }

        // Отримуємо список робітників для вразливої зміни
        WorkersPriorityLists priorityList(vacations, shifts, assignedResult, wishes, workers, setup);
        auto listOfWorkersForShift = priorityList.getWorkersListPriority(shifts.find(QString::number(vulnerableValue.first)).value());

        /*// Робимо спробу поставити на робочу зміну робітника із бажанням
        if (!listOfWorkersForShift.filter(priorityList.getGreenText(), Qt::CaseSensitivity::CaseInsensitive).isEmpty())
        {
            QList<QString> workersWithWish = listOfWorkersForShift.filter(priorityList.getGreenText(), Qt::CaseSensitivity::CaseInsensitive);

            int ID_candidate = -1;
            for (auto it = queueWorkers.begin(); it != queueWorkers.end(); it++)
            {
                for (auto it_ID : it.value())
                {
                    for (auto it_list : workersWithWish)
                    {
                        if (it_ID == WorkersPriorityLists::stringToID(it_list))
                        {
                            ID_candidate = it_ID;
                            break;
                        }
                    }

                    if (ID_candidate != -1)
                        break;
                }

                if (ID_candidate != -1)
                    break;
            }

            SingleAssignedShift singleAssignment;
            singleAssignment.ID_Shift = vulnerableValue.first;
            //singleAssignment.ID_Worker = workerTextFromList.toInt();
            singleAssignment.ID_Worker = ID_candidate;

            assignedResult.insert(QString::number(singleAssignment.ID_Shift), singleAssignment);

            continue;
        }
        */

        // Cтавимо на робочу зміну робітника
        {
            QList<QString> commonWorkers = listOfWorkersForShift;

            int ID_candidate = -1;
            for (auto it = queueWorkers.begin(); it != queueWorkers.end(); it++)
            {
                for (auto it_ID : it.value())
                {
                    for (auto it_list : commonWorkers)
                    {
                        if (it_ID == WorkersPriorityLists::stringToID(it_list))
                        {
                            ID_candidate = it_ID;
                            break;
                        }
                    }

                    if (ID_candidate != -1)
                        break;
                }

                if (ID_candidate != -1)
                    break;
            }


            SingleAssignedShift singleAssignment;
            singleAssignment.ID_Shift = vulnerableValue.first;
            singleAssignment.ID_Worker = ID_candidate;

            assignedResult.insert(QString::number(singleAssignment.ID_Shift), singleAssignment);

            continue;
        }
    }
}


/*
QMap<int
//ID_Worker
, int
//number
>
AutomatedAlgorithmsBuildingSchedules::getMapPossibilityOfWorkersByShift(const Shifts& shifts,
                                                                        const Workers& workers,
                                                                        const Vacations& vacations,
                                                                        const AssignedShifts& assigned,
                                                                        const ProjectSetup& setup)
{
    QMap<int, int> result;

    for (auto it_worker = workers.begin(); it_worker != workers.end(); it_worker++)
    {
        for (auto it_shift = shifts.begin(); it_shift != shifts.end(); it_shift++)
        {
            QDateTime startDate = QDateTime::fromString(it_shift->StartDate);
            QDateTime endDate = QDateTime::fromString(it_shift->EndDate);

            // Якщо робітник має відпустку, то ця зміна не буде враховуватись для нього
            bool haveVacation = false;
            for (auto it_vacation : vacations)
            {
                if (it_vacation.ID_worker != it_worker.value().ID_worker)
                    continue;

                // Якщо початок зміни чи її кінець є під час відпустки робітника, то він на роботу не вийде
                if (inRangeDateTime(startDate, QDateTime::fromString(it_vacation.StartDate), QDateTime::fromString(it_vacation.EndDate)) ||
                        inRangeDateTime(endDate, QDateTime::fromString(it_vacation.StartDate), QDateTime::fromString(it_vacation.EndDate)))
                {
                    haveVacation = true;
                    break;
                }
            }
            if (haveVacation)
                continue;

            // Якщо робітник працюватиме разом із цією зміною більше або рівне максимальній
            // кількості годин за період, то ця зміна не буде враховуватись для нього



        }
    }

    return result;
}
*/
