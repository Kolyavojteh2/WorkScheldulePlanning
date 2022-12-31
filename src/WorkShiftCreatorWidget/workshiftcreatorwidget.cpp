#include "workshiftcreatorwidget.h"
#include "ui_workshiftcreatorwidget.h"
#include <QLibrary>
#include <QMessageBox>
#include <QFileDialog>
#include <QSet>

#include <QMap>

WorkShiftCreatorWidget::WorkShiftCreatorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkShiftCreatorWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void WorkShiftCreatorWidget::setupWidget()
{
    validateData();
    resetData();

    // List of shifts
    connect(ui->p_button_AddShift, SIGNAL(clicked()),
            this, SLOT(slotAddShift()));
    connect(ui->p_button_DeleteShift, SIGNAL(clicked()),
            this, SLOT(slotDeleteShift()));
    connect(ui->p_list_Shifts, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(slotEditShift(QListWidgetItem *)));

    // Navigation buttons
    connect(ui->p_button_CreateNew, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));
}

void WorkShiftCreatorWidget::validateData()
{
    ui->p_line_ID_Shift->setValidator(new QRegExpValidator(QRegExp("^\\d+$|^$")));
}

void WorkShiftCreatorWidget::resetData()
{
    m_vacations.clear();
    m_allWorkers.clear();
    m_requirements.clear();

    ui->p_line_ID_Shift->setText("");
    ui->p_comboBox_Position->clear();
    ui->p_comboBox_ID_Workers->clear();

    ui->p_dateTimeEdit_StartDate->clear();
    ui->p_dateTimeEdit_EndDate->clear();

    ui->p_list_Shifts->clear();

    ui->p_line_ID_Shift->setEnabled(false);
    ui->p_comboBox_Position->setEnabled(false);
    ui->p_comboBox_ID_Workers->setEnabled(false);
    ui->p_dateTimeEdit_StartDate->setEnabled(false);
    ui->p_dateTimeEdit_EndDate->setEnabled(false);
}

void WorkShiftCreatorWidget::setInformationToForm(const SingleShift& info)
{
    ui->p_line_ID_Shift->setText(QString::number(info.ID_Shifts));
    ui->p_comboBox_Position->setCurrentIndex(ui->p_comboBox_Position->findText(info.Position));
    ui->p_comboBox_ID_Workers->setCurrentIndex(ui->p_comboBox_ID_Workers->findText(QString::number(info.ID_Workers)));
    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime::fromString(info.StartDate));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime::fromString(info.EndDate));
}

SingleShift WorkShiftCreatorWidget::getInformationFromForm()
{
    SingleShift info;

    info.ID_Shifts = ui->p_line_ID_Shift->text().toInt();
    info.Position = ui->p_comboBox_Position->currentText();
    info.StartDate = ui->p_dateTimeEdit_StartDate->dateTime().toString();
    info.EndDate = ui->p_dateTimeEdit_EndDate->dateTime().toString();

    if (ui->p_comboBox_ID_Workers->currentText().contains(recomendedString))
    {
        QString str = ui->p_comboBox_ID_Workers->currentText();
        str.remove(0, recomendedString.size() + 1);
        info.ID_Workers = str.toInt();
    }
    else if (ui->p_comboBox_ID_Workers->currentText().contains(unwantedString))
    {
        QString str = ui->p_comboBox_ID_Workers->currentText();
        str.remove(0, unwantedString.size() + 1);
        info.ID_Workers = str.toInt();
    }
    else
    {
        info.ID_Workers = ui->p_comboBox_ID_Workers->currentText().toInt();
    }

    return info;
}

WorkShiftCreatorWidget::~WorkShiftCreatorWidget()
{
    delete ui;
}

void WorkShiftCreatorWidget::slotSetEnterpriseSetup(const ProjectSetup& newSetup)
{
    m_setupEnterprise = newSetup;
}

Requirements WorkShiftCreatorWidget::mergeRequirements(Requirements first, Requirements second)
{
    for (auto it = second.begin(); it != second.end();)
    {
        if (first.find(QString::number(it.value().ID)) != first.end())
        {
            int newID = findLastRequirementID(first);
            first.insert(QString::number(newID), it.value());
            first[QString::number(newID)].ID = newID;

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

void WorkShiftCreatorWidget::slotAddRequirements(const Requirements& newRequirements)
{
    m_requirements = mergeRequirements(m_requirements, newRequirements);
    /*
    for (auto it = newRequirements.begin(); it != newRequirements.end();)
    {
        if (m_requirements.find(QString::number(it.value().ID)) != m_requirements.end())
        {
            int newID = findLastRequirementID();
            m_requirements.insert(QString::number(newID), it.value());
            m_requirements[QString::number(newID)].ID = newID;

            auto delete_iterator = it;
            it++;
            fileRequirement.erase(delete_iterator);
        }
        else
        {
            m_requirements.insert(it.key(), it.value());

            auto delete_iterator = it;
            it++;
            fileRequirement.erase(delete_iterator);
        }
    }
    */
}

void WorkShiftCreatorWidget::slotAddRequirementsFromFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    // Завантаження даних про робітника із файлу
    typedef Requirements (*func_readRequirements)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readRequirements readRequirements = (func_readRequirements)xmlParser.resolve("read_RequirementsFromFile");

    Requirements fileRequirement = readRequirements(filename);

    m_requirements = mergeRequirements(m_requirements, fileRequirement);
    /*
    for (auto it = fileRequirement.begin(); it != fileRequirement.end();)
    {
        if (m_requirements.find(QString::number(it.value().ID)) != m_requirements.end())
        {
            int newID = findLastRequirementID();
            m_requirements.insert(QString::number(newID), it.value());
            m_requirements[QString::number(newID)].ID = newID;

            auto delete_iterator = it;
            it++;
            fileRequirement.erase(delete_iterator);
        }
        else
        {
            m_requirements.insert(it.key(), it.value());

            auto delete_iterator = it;
            it++;
            fileRequirement.erase(delete_iterator);
        }
    }
    */
}

void WorkShiftCreatorWidget::slotClearRequirements()
{
    m_requirements.clear();
}

int WorkShiftCreatorWidget::findLastRequirementID(const Requirements& list)
{
    int result = 0;
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (result < it.value().ID)
            result = it.value().ID;
    }

    return result;
}

int WorkShiftCreatorWidget::findLastVacationID(const Vacations& list)
{
    int result = 0;
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (result < it.value().ID_vacation)
            result = it.value().ID_vacation;
    }

    return result;
}

Vacations WorkShiftCreatorWidget::mergeVacations(Vacations first, Vacations second)
{
    for (auto it = second.begin(); it != second.end();)
    {
        if (first.find(QString::number(it.value().ID_vacation)) != first.end())
        {
            int newID = findLastVacationID(first);
            first.insert(QString::number(newID), it.value());
            first[QString::number(newID)].ID_vacation = newID;

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

void WorkShiftCreatorWidget::slotAddVacations(const Vacations& newVacations)
{
    m_vacations = mergeVacations(m_vacations, newVacations);
}

void WorkShiftCreatorWidget::slotAddVacationsFromFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    // Завантаження даних про робітника із файлу
    typedef Vacations (*func_readVacations)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readVacations readVacations = (func_readVacations)xmlParser.resolve("read_VacationsFromFile");

    Vacations fileVacations = readVacations(filename);

    m_vacations = mergeVacations(m_vacations, fileVacations);
}

void WorkShiftCreatorWidget::slotClearVacations()
{
    m_vacations.clear();
}

int WorkShiftCreatorWidget::findLastWishID(const Wishes& list)
{
    int result = 0;
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (result < it.value().ID_Wishes)
            result = it.value().ID_Wishes;
    }

    return result;
}

Wishes WorkShiftCreatorWidget::mergeWishes(Wishes first, Wishes second)
{
    for (auto it = second.begin(); it != second.end();)
    {
        if (first.find(QString::number(it.value().ID_Wishes)) != first.end())
        {
            int newID = findLastWishID(first);
            first.insert(QString::number(newID), it.value());
            first[QString::number(newID)].ID_Wishes = newID;

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

void WorkShiftCreatorWidget::slotAddWishes(const Wishes& newWishes)
{
    m_wishes = mergeWishes(m_wishes, newWishes);
}

void WorkShiftCreatorWidget::slotAddWishesFromFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    // Завантаження даних про робітника із файлу
    typedef Wishes (*func_readWishes)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readWishes readWishes = (func_readWishes)xmlParser.resolve("read_WishesFromFile");

    Wishes fileWishes = readWishes(filename);

    m_wishes = mergeWishes(m_wishes, fileWishes);
}

void WorkShiftCreatorWidget::slotClearWishes()
{
    m_wishes.clear();
}

bool WorkShiftCreatorWidget::inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max)
{
    if (value >= min && value < max)
        return true;
    return false;
}

int WorkShiftCreatorWidget::findLastShiftID(const Shifts& list)
{
    int result = 0;
    for (auto it : list)
    {
        if (it.ID_Shifts > result)
            result = it.ID_Shifts;
    }

    return result;
}

void WorkShiftCreatorWidget::slotAddShift()
{
    // Пошук останнього ID в списку і створення нового імені для нового ID
    int lastID = findLastShiftID(m_shifts) + 1;
    QString name = QString::number(lastID);

    // Створення нового елемента списку
    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Shifts->addItem(item);

    // Створення нового елемента словника та заповнення його даними
    SingleShift single;
    single.ID_Shifts = lastID;
    single.ID_Workers = 0;

    single.StartDate = QDateTime(QDate::currentDate(), QTime(0,0,0)).toString();
    single.EndDate = QDateTime(QDate::currentDate(), QTime(0,0,0)).toString();

    m_shifts.insert(item->text(), single);
    slotModifyFile();
    //ui->p_line_ID_worker->setEnabled(false);
}

void WorkShiftCreatorWidget::slotDeleteShift()
{
    // Якщо список пустий, то виходимо
    if (ui->p_list_Shifts->selectedItems().isEmpty())
        return;

    // Отримуємо вказівник на елемент
    auto item = ui->p_list_Shifts->selectedItems().first();

    // Видаляємо із словника елемент
    auto iterator_pos = m_shifts.find(item->text());
    m_shifts.erase(iterator_pos);

    // Якщо цей елемент що ми видаляємо зараз редагується, то треба очистити форму і заблокувати її
    if (item->text() == m_currentEditShift)
    {
        m_currentEditShift.clear();

        //ui->p_line_ID_worker->setText("");
        ui->p_line_ID_Shift->setText("");

        QTime temp_time(0,0,0);
        ui->p_dateTimeEdit_StartDate->setDate(QDate::currentDate());
        ui->p_dateTimeEdit_StartDate->setTime(temp_time);

        ui->p_dateTimeEdit_EndDate->setDate(QDate::currentDate());
        ui->p_dateTimeEdit_EndDate->setTime(temp_time);

        ui->p_comboBox_Position->clear();
        ui->p_comboBox_ID_Workers->clear();

        //ui->p_line_ID_worker->setEnabled(false);
        ui->p_line_ID_Shift->setEnabled(false);
        ui->p_dateTimeEdit_StartDate->setEnabled(false);
        ui->p_dateTimeEdit_EndDate->setEnabled(false);
        ui->p_comboBox_Position->setEnabled(false);
        ui->p_comboBox_ID_Workers->setEnabled(false);
    }

    // Вказуємо що файл модифіковано
    slotModifyFile();

    // Видаляємо елемент із списку форми
    delete item;
}

void WorkShiftCreatorWidget::slotModifyFile()
{
    isModifiedFile = true;
}

void WorkShiftCreatorWidget::slotCreateNew()
{
    if (isModifiedFile)
    {
        int result = QMessageBox::warning(this,
                                          tr("No changes have been saved to the file"),
                                          tr("Do you want to save the changes to the file"),
                                          QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);

        if (result == QMessageBox::No)
        {
            resetData();
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();
            if (!isModifiedFile)
                resetData();
            else
                return;
        }
        if (result == QMessageBox::Cancel)
        {
            return;
        }
    }
    else
    {
        resetData();
    }
}

void WorkShiftCreatorWidget::slotOpen()
{
    if (isModifiedFile)
    {
        int result = QMessageBox::warning(this,
                                          tr("No changes have been saved to the file"),
                                          tr("Do you want to save the changes to the file"),
                                          QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);

        if (result == QMessageBox::No)
        {
            QString str = QFileDialog::getOpenFileName(this,
                                                       "Open...",
                                                       m_defaultPath,
                                                       "*.xml");
            openFile(str);
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();

            QString str = QFileDialog::getOpenFileName(this,
                                                       "Open...",
                                                       m_defaultPath,
                                                       "*.xml");
            openFile(str);
        }
    }
    else
    {
        QString str = QFileDialog::getOpenFileName(this,
                                                   "Open...",
                                                   m_defaultPath,
                                                   "*.xml");
        openFile(str);
    }
}

void WorkShiftCreatorWidget::openFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    resetData();
    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef Shifts (*func_readShifts)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readShifts readShifts = (func_readShifts)xmlParser.resolve("read_ShiftsFromFile");

    m_shifts = readShifts(filename);

    ui->p_list_Shifts->clear();

    for (auto it : m_shifts)
    {
        ui->p_list_Shifts->addItem(QString::number(it.ID_Shifts));
    }

    isModifiedFile = false;
}

void WorkShiftCreatorWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writeShiftsToFile)(const QString&, const Shifts&);
    QLibrary xmlParser("XML_Parsing");
    func_writeShiftsToFile writeShifts = (func_writeShiftsToFile)xmlParser.resolve("write_ShiftsToFile");

    writeShifts(m_filename, m_shifts);
    isModifiedFile = false;
}

void WorkShiftCreatorWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", m_defaultPath, "*.xml");
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

QString WorkShiftCreatorWidget::defaultPath()
{
    return m_defaultPath;
}

void WorkShiftCreatorWidget::slotSetDefaultPath(const QString& path)
{
    m_defaultPath = path;
}

void WorkShiftCreatorWidget::closeEvent(QCloseEvent *event)
{
    if (isModifiedFile)
    {
        int result = QMessageBox::warning(this,
                                          tr("No changes have been saved to the file"),
                                          tr("Do you want to save the changes to the file"),
                                          QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);

        if (result == QMessageBox::No)
        {
            event->accept();
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();
            if (!isModifiedFile)
                event->accept();
            else
                event->ignore();
        }
        if (result == QMessageBox::Cancel)
        {
            event->ignore();
        }
    }
    else
    {
        event->accept();
    }
}

void WorkShiftCreatorWidget::slotEditShift(QListWidgetItem *item)
{
    // save data from form to memory
    if (m_currentEditShift != "")
    {
        m_shifts[m_currentEditShift] = getInformationFromForm();
    }

    // Load new data
    m_currentEditShift = item->text();

    auto foundItem = m_shifts.end();
    for (auto it = m_shifts.begin(); it != m_shifts.end(); it++)
    {
        if (QString::number(it->ID_Shifts) == item->text())
        {
            foundItem = it;
            break;
        }
    }

    if (foundItem == m_shifts.end())
        return;

    setInformationToForm(*foundItem);

    ui->p_line_ID_Shift->setEnabled(true);
    ui->p_dateTimeEdit_StartDate->setEnabled(true);
    ui->p_dateTimeEdit_EndDate->setEnabled(true);
    ui->p_comboBox_ID_Workers->setEnabled(true);
    ui->p_comboBox_Position->setEnabled(true);
}

void WorkShiftCreatorWidget::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_buttons_layout->count(); i++)
    {
        ui->p_buttons_layout->itemAt(i)->widget()->setVisible(showState);
    }
}

void WorkShiftCreatorWidget::slotUpdateData(const QString& attributeName)
{
    if (attributeName == "ID_Shifts")
    {
        auto iter_currentShift = m_shifts.find(m_currentEditShift);
        if (iter_currentShift == m_shifts.end())
            return;

        QString newID_Wish = ui->p_line_ID_Shift->text();

        // Пошук одинакових ID і якщо знайдено, то присвоїти дефолтний
        if (m_currentEditShift.toInt() != newID_Wish.toInt())
        {
            for (auto it = m_shifts.begin(); it != m_shifts.end(); it++)
            {
                if (newID_Wish.toInt() == it.value().ID_Shifts)
                {
                    newID_Wish = QString::number(findLastShiftID(m_shifts) + 1);
                    ui->p_line_ID_Shift->setText(newID_Wish);
                    break;
                }
            }
        }
        //

        iter_currentShift.value().ID_Shifts = newID_Wish.toInt();

        if (QString::number(iter_currentShift.value().ID_Shifts) == iter_currentShift.key())
        {
            return;
        }
        else
        {
            QListWidgetItem *newItem = new QListWidgetItem(newID_Wish);
            QListWidgetItem *oldItem = ui->p_list_Shifts->currentItem();
            ui->p_list_Shifts->addItem(newItem);
            m_currentEditShift = newItem->text();
            m_shifts.insert(newItem->text(), iter_currentShift.value());

            delete oldItem;
            m_shifts.erase(iter_currentShift);

            ui->p_list_Shifts->setCurrentItem(newItem);
        }

        return;
    }

    /*
    if (attributeName == "ID worker") {}
    */

    if (attributeName == "StartDate" || attributeName == "EndDate")
    {
        if (ui->p_dateTimeEdit_StartDate->date() > ui->p_dateTimeEdit_EndDate->date())
        {
            ui->p_dateTimeEdit_EndDate->setDate(ui->p_dateTimeEdit_StartDate->date());
        }
    }

    if (attributeName == "Position")
    {
        updateWorkersComboBox();
    }

    /*
    if (attributeName == "ID_Worker")
    {
        updateWorkersComboBox();
    }
    */

    // save data from form to memory
    if (m_currentEditShift != "")
    {
        m_shifts[m_currentEditShift] = getInformationFromForm();
    }
}

void WorkShiftCreatorWidget::updatePositionsComboBox()
{
    QSet<QString> setOfPositions;
    for (auto it = m_requirements.begin(); it != m_requirements.end(); it++)
    {
        setOfPositions.insert(it.value().Position);
    }

    QString currentItem = ui->p_comboBox_Position->currentText();
    ui->p_comboBox_Position->clear();
    ui->p_comboBox_Position->addItems(setOfPositions.toList());
    ui->p_comboBox_Position->setCurrentIndex(ui->p_comboBox_Position->findText(currentItem));
}

void WorkShiftCreatorWidget::updateWorkersComboBox()
{
    QList<Worker> listWorkers;
    listWorkers = m_allWorkers;

    QSet<int> notWork;
    QSet<int> greenList;
    QSet<int> yellowList;
    QSet<int> commonList;

    QDateTime startDate = ui->p_dateTimeEdit_StartDate->dateTime();
    QDateTime endDate = ui->p_dateTimeEdit_EndDate->dateTime();

// Заповнення червоного списку(множини не працюючих людей на даній зміні)
    for (auto it_vacation : m_vacations)
    {
        // Якщо початок зміни чи її кінець є під час відпустки робітника, то він на роботу не вийде
        if (inRangeDateTime(startDate, QDateTime::fromString(it_vacation.StartDate), QDateTime::fromString(it_vacation.EndDate)) ||
                inRangeDateTime(endDate, QDateTime::fromString(it_vacation.StartDate), QDateTime::fromString(it_vacation.EndDate)))
        {
            notWork.insert(it_vacation.ID_worker);
            /*
            // Знайти цього працівника ID, який зараз у відпустці
            for (auto it_worker = m_allWorkers.begin(); it_worker != m_allWorkers.end(); it_worker++)
            {
                // Якщо це той працівник у відпустці, то додаємо його до списку не працюючих
                if (it_worker->ID_worker == it_vacation.ID_worker)
                {
                    notWork.insert(*it_worker);
                    break;
                }
            }
            */
        }
    }

    // Тепер шукаємо працівників, що були на роботі менше ніж мінімальний час відпочинку(10 годин тому)
    // або мають зміну після закінчення цієї зміни раніше ніж через мінімальний час відпочинку.
    // Виконуємо обхід по всіх існуючих змінах
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
            notWork.insert(it_shift.value().ID_Workers);
            /*
            // Знайти працівника ID
            for (auto it_worker = m_allWorkers.begin(); it_worker != m_allWorkers.end(); it_worker++)
            {
                // Якщо це той працівник у відпустці, то додаємо його до списку не працюючих
                if (it_worker->ID_worker == it_shift->ID_Workers)
                {
                    notWork.insert(*it_worker);
                    break;
                }
            }
            */
        }
    }

// Заповнення зеленого списку(побажання робітника)
    for (auto it_wishes : m_wishes)
    {
        // Перевіряємо чи початок побажання і кінець побажання входять в діапазон роботи цієї зміни, то ми додаємо робітника до зеленого списку
        if (inRangeDateTime(QDateTime::fromString(it_wishes.StartDate), startDate, endDate) &&
                inRangeDateTime(QDateTime::fromString(it_wishes.EndDate), startDate, endDate))
        {
            greenList.insert(it_wishes.ID_Workers);
            /*
            // Знайти працівника ID
            for (auto it_worker = m_allWorkers.begin(); it_worker != m_allWorkers.end(); it_worker++)
            {
                // Якщо це той працівник у відпустці, то додаємо його до списку не працюючих
                if (it_worker->ID_worker == it_wishes.ID_Workers)
                {
                    greenList.insert(*it_worker);
                    break;
                }
            }
            */
        }
    }

// Заповнення жовтого списку(люди що відпочили більше ніж мінімальна кількість годин, але менше ніж нормальна
// та ті хто вже перевищили свій відсоток змін на період)
    // Виконуємо обхід по всіх існуючих змінах
    for (auto it_shift = m_shifts.begin(); it_shift != m_shifts.end(); it_shift++)
    {
        // Допоміжні змінні
        // Переведення мінімального часу для відпочинку в секунди
        QTime standartRest = QTime::fromString(m_setupEnterprise.StandartRestTime);
        int standartRestSeconds = standartRest.hour() * 60 * 60 + standartRest.minute() * 60 + standartRest.second();

        // Визначення точок часу, які стосуються цієї зміни
        QDateTime StartAreaShift = startDate.addSecs(-standartRestSeconds);
        QDateTime EndAreaShift = endDate.addSecs(standartRestSeconds);

        // Якщо поточна зміна(та що в формі) знаходиться між цими точками часу,
        // то шукаємо дані про робітника
        // і додаємо його до червоного списку
        if (inRangeDateTime(QDateTime::fromString(it_shift.value().StartDate), StartAreaShift, EndAreaShift) ||
                inRangeDateTime(QDateTime::fromString(it_shift.value().EndDate), StartAreaShift, EndAreaShift))
        {
            yellowList.insert(it_shift.value().ID_Workers);
            /*
            // Знайти працівника ID
            for (auto it_worker = m_allWorkers.begin(); it_worker != m_allWorkers.end(); it_worker++)
            {
                // Якщо це той працівник у відпустці, то додаємо його до списку не працюючих
                if (it_worker->ID_worker == it_shift->ID_Workers)
                {
                    yellowList.insert(*it_worker);
                    break;
                }
            }
            */
        }
    }

    QMap<int, int> countOfShiftsPerWorker; // worker/count
    for (auto it_shift = m_shifts.begin(); it_shift != m_shifts.end(); it_shift++)
    {
        countOfShiftsPerWorker[it_shift.value().ID_Workers]++;
    }

    for (auto it = countOfShiftsPerWorker.begin(); it != countOfShiftsPerWorker.end(); it++)
    {
        if ((static_cast<float>(it.value()) / m_setupEnterprise.GraphicForeverDay) > (m_setupEnterprise.MaxPercentOfWorkShiftToPeriod / 100.0f))
        {
            yellowList.insert(it.key());
        }
    }


// Заповнення звичайного списку(всі решта хто не потрапив у якусь категорію)
    for (auto it : m_allWorkers)
    {
        commonList.insert(it.ID_worker);
    }

// Операції над множинами(прибирання людей із категорій)
    greenList.subtract(notWork);

    yellowList.subtract(greenList);
    yellowList.subtract(notWork);

    commonList.subtract(notWork);
    commonList.subtract(greenList);
    commonList.subtract(yellowList);

    for (auto it : greenList)
    {
        ui->p_comboBox_ID_Workers->addItem(recomendedString + " " + QString::number(it));
    }

    for (auto it : commonList)
    {
        ui->p_comboBox_ID_Workers->addItem(QString::number(it));
    }

    for (auto it : yellowList)
    {
        ui->p_comboBox_ID_Workers->addItem(unwantedString + " " + QString::number(it));
    }
}
