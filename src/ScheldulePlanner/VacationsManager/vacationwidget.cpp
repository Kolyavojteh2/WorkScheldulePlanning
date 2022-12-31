#include "vacationwidget.h"
#include "ui_vacationwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QRegExp>

#include "projectdataloader.h"

VacationWidget::VacationWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VacationWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void VacationWidget::setupWidget()
{
    validateData();
    slotCheckIDWorker();

    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime(QDate::currentDate(), QTime(0,0,0)));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime(QDate::currentDate().addDays(1), QTime(0, 0, 0)));

    ui->p_line_ID_Vacation->setEnabled(false);
    ui->p_line_ID_Worker->setEnabled(false);
    ui->p_dateTimeEdit_StartDate->setEnabled(false);
    ui->p_dateTimeEdit_EndDate->setEnabled(false);

    ui->p_line_Surname->setEnabled(false);
    ui->p_line_FirstName->setEnabled(false);
    ui->p_line_LastName->setEnabled(false);

    // Check ID worker, if ID is empty, then block list
    connect(ui->p_line_ID_Worker, SIGNAL(editingFinished()),
            this, SLOT(slotCheckIDWorker()));
    connect(ui->p_line_ID_Worker, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotChangedIDWorker(const QString&)));

    // Navigation buttons
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));

    // List of vacations
    connect(ui->p_list_Vacations, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(slotEditVacation(QListWidgetItem *)));
    connect(ui->p_button_AddVacation, SIGNAL(clicked()),
            this, SLOT(slotAddVacation()));
    connect(ui->p_button_DeleteVacation, SIGNAL(clicked()),
            this, SLOT(slotDeleteVacation()));

    // Editing connections
    p_editingMapper = new QSignalMapper(this);
    connect(p_editingMapper, SIGNAL(mapped(const QString&)),
            this, SLOT(slotUpdateData(const QString&)));

    connect(ui->p_line_ID_Vacation, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_line_ID_Vacation, "ID vacation");

    connect(ui->p_line_ID_Worker, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_line_ID_Worker, "ID worker");

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateTimeEdit_StartDate, "StartDate");

    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateTimeEdit_EndDate, "EndDate");

}

void VacationWidget::validateData()
{
    ui->p_line_ID_Vacation->setValidator(new QRegExpValidator(QRegExp("^\\d+$")));
    ui->p_line_ID_Worker->setValidator(new QRegExpValidator(QRegExp("^\\d+$")));
}

VacationWidget::~VacationWidget()
{
    delete ui;
}

void VacationWidget::resetData()
{
    m_vacations.clear();
    ui->p_list_Vacations->clear();

    ui->p_line_ID_Vacation->setEnabled(false);
    ui->p_line_ID_Worker->setEnabled(false);
    ui->p_dateTimeEdit_StartDate->setEnabled(false);
    ui->p_dateTimeEdit_EndDate->setEnabled(false);

    ui->p_line_Surname->setEnabled(false);
    ui->p_line_FirstName->setEnabled(false);
    ui->p_line_LastName->setEnabled(false);

    ui->p_line_ID_Vacation->setText("");
    ui->p_line_ID_Worker->setText("");
    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime(QDate::currentDate(), QTime(0,0,0)));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime(QDate::currentDate().addDays(1), QTime(0,0,0)));
}

SingleVacation VacationWidget::getInformationFromForm()
{
    SingleVacation info;

    info.ID_vacation = ui->p_line_ID_Vacation->text().toInt();
    info.ID_worker = ui->p_line_ID_Worker->text().toInt();
    info.StartDate = ui->p_dateTimeEdit_StartDate->dateTime().toString();
    info.EndDate = ui->p_dateTimeEdit_EndDate->dateTime().toString();

    return info;
}

void VacationWidget::setInformationToForm(const SingleVacation& info)
{
    ui->p_line_ID_Vacation->setText(QString::number(info.ID_vacation));
    ui->p_line_ID_Worker->setText(QString::number(info.ID_worker));
    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime::fromString(info.StartDate));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime::fromString(info.EndDate));
}

int VacationWidget::findLastIDInList()
{
    int result = 0;
    for (auto it : m_vacations)
    {
        if (it.ID_vacation > result)
            result = it.ID_vacation;
    }

    return result;
}

void VacationWidget::slotAddVacation()
{
    int lastID = findLastIDInList() + 1;
    QString name = QString::number(lastID);

    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Vacations->addItem(item);

    SingleVacation single;
    single.ID_vacation = lastID;
    single.ID_worker = 0;
    single.StartDate = QDateTime(QDate::currentDate(), QTime(0, 0, 0)).toString();
    single.EndDate = QDateTime(QDate::currentDate().addDays(1), QTime(0, 0, 0)).toString();

    m_vacations.insert(item->text(), single);
}

void VacationWidget::slotDeleteVacation()
{
    if (ui->p_list_Vacations->selectedItems().isEmpty())
        return;

    auto item = ui->p_list_Vacations->selectedItems().first();

    auto iterator_pos = m_vacations.find(item->text());
    m_vacations.erase(iterator_pos);

    if (item->text() == m_currentEditVacationName)
    {
        m_currentEditVacationName.clear();

        ui->p_line_ID_Vacation->setEnabled(false);
        ui->p_line_ID_Worker->setEnabled(false);
        ui->p_dateTimeEdit_StartDate->setEnabled(false);
        ui->p_dateTimeEdit_EndDate->setEnabled(false);

        ui->p_line_Surname->setEnabled(false);
        ui->p_line_FirstName->setEnabled(false);
        ui->p_line_LastName->setEnabled(false);

        ui->p_line_ID_Vacation->setText("");
        ui->p_line_ID_Worker->setText("");
        ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0, 0)));
        ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime(QDate::currentDate().addDays(1), QTime(0, 0, 0)));
    }

    delete item;
}

void VacationWidget::slotEditVacation(QListWidgetItem *item)
{
    // save data from form to memory
    if (m_currentEditVacationName != "")
    {
        m_vacations[m_currentEditVacationName] = getInformationFromForm();
    }

    // Load new data
    m_currentEditVacationName = item->text();

    auto foundItem = m_vacations.end();
    for (auto it = m_vacations.begin(); it != m_vacations.end(); it++)
    {
        if (QString::number(it->ID_vacation) == item->text())
        {
            foundItem = it;
            break;
        }
    }

    if (foundItem == m_vacations.end())
        return;

    setInformationToForm(*foundItem);

    ui->p_line_ID_Vacation->setEnabled(true);
    ui->p_line_ID_Worker->setEnabled(true);
    ui->p_dateTimeEdit_StartDate->setEnabled(true);
    ui->p_dateTimeEdit_EndDate->setEnabled(true);

    ui->p_line_Surname->setEnabled(true);
    ui->p_line_FirstName->setEnabled(true);
    ui->p_line_LastName->setEnabled(true);
}

void VacationWidget::slotUpdateData(const QString& attributeName)
{
    if (attributeName == "ID vacation")
    {
        auto iter_currentVacation = m_vacations.find(m_currentEditVacationName);
        if (iter_currentVacation == m_vacations.end())
            return;

        QString newID_vacation = ui->p_line_ID_Vacation->text();
        // Пошук одинакових ID і якщо знайдено, то присвоїти дефолтний
        if (m_currentEditVacationName.toInt() != newID_vacation.toInt())
        {
            for (auto it = m_vacations.begin(); it != m_vacations.end(); it++)
            {
                if (newID_vacation.toInt() == it.value().ID_vacation)
                {
                    newID_vacation = QString::number(findLastIDInList() + 1);
                    ui->p_line_ID_Vacation->setText(newID_vacation);
                    break;
                }
            }
        }
        //
        iter_currentVacation.value().ID_vacation = newID_vacation.toInt();

        if (QString::number(iter_currentVacation.value().ID_vacation) == iter_currentVacation.key())
        {
            return;
        }
        else
        {
            QListWidgetItem *newItem = new QListWidgetItem(newID_vacation);
            QListWidgetItem *oldItem = ui->p_list_Vacations->currentItem();
            ui->p_list_Vacations->addItem(newItem);
            m_currentEditVacationName = newItem->text();
            m_vacations.insert(newItem->text(), iter_currentVacation.value());

            delete oldItem;
            m_vacations.erase(iter_currentVacation);

            ui->p_list_Vacations->setCurrentItem(newItem);
        }

        return;
    }

    //if (attributeName == "ID worker") {}

    if (attributeName == "StartDate" || attributeName == "EndDate")
    {
        if (ui->p_dateTimeEdit_StartDate->dateTime() > ui->p_dateTimeEdit_EndDate->dateTime())
        {
            ui->p_dateTimeEdit_EndDate->setDateTime(ui->p_dateTimeEdit_StartDate->dateTime());
        }
    }

    // save data from form to memory
    if (m_currentEditVacationName != "")
    {
        m_vacations[m_currentEditVacationName] = getInformationFromForm();
    }
}

void VacationWidget::slotCheckIDWorker()
{
    //QFile worker_file;
    if (!m_pathWorkersInformation.isEmpty())
    {
        QString str = m_pathWorkersInformation + ui->p_line_ID_Worker->text() + ".xml";
        if (QFile::exists(str))
        {
            Worker currentWorker = ProjectDataLoader::loadWorker(str);

            ui->p_line_Surname->setText(currentWorker.Surname);
            ui->p_line_FirstName->setText(currentWorker.FirstName);
            ui->p_line_LastName->setText(currentWorker.LastName);
        }
        else
        {
            ui->p_line_Surname->setText("unknown");
            ui->p_line_FirstName->setText("unknown");
            ui->p_line_LastName->setText("unknown");
        }
    }

    if (ui->p_line_ID_Worker->text().isEmpty())
    {
        ui->p_line_Surname->clear();
        ui->p_line_FirstName->clear();
        ui->p_line_LastName->clear();
    }
}

void VacationWidget::slotSave()
{
    isSaved = true;
    emit signalSendVacations(m_vacations);
}

void VacationWidget::closeEvent(QCloseEvent *event)
{
    if (!isSaved)
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
            event->accept();
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

void VacationWidget::slotSetPathWorkersInformation(const QString& path)
{
    m_pathWorkersInformation = path;
}

void VacationWidget::slotChangedIDWorker(const QString &text)
{
    slotCheckIDWorker();
}

void VacationWidget::slotSetVacations(Vacations info)
{
    m_vacations = info;

    QMap<int, SingleVacation> sortedInfo;
    for (auto it = info.begin(); it != info.end(); it++)
    {
        sortedInfo.insert(it.key().toInt(), it.value());
    }

    ui->p_list_Vacations->clear();
    for (auto it : sortedInfo)
    {
        ui->p_list_Vacations->addItem(QString::number(it.ID_vacation));
    }
}
