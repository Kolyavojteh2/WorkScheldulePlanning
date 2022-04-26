#include "vacationwidget.h"
#include "ui_vacationwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QRegExp>

VacationWidget::VacationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VacationWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void VacationWidget::setupWidget()
{
    validateData();
    slotCheckIDWorker();

    ui->p_dateEdit_StartDate->setDate(QDate::currentDate());
    ui->p_dateEdit_EndDate->setDate(QDate::currentDate());

    ui->p_line_ID_Vacation->setEnabled(false);
    ui->p_line_ID_Worker->setEnabled(false);
    ui->p_dateEdit_StartDate->setEnabled(false);
    ui->p_dateEdit_EndDate->setEnabled(false);

    ui->p_line_Surname->setEnabled(false);
    ui->p_line_FirstName->setEnabled(false);
    ui->p_line_LastName->setEnabled(false);

    // Check ID worker, if ID is empty, then block list
    connect(ui->p_line_ID_Worker, SIGNAL(editingFinished()),
            this, SLOT(slotCheckIDWorker()));
    connect(ui->p_line_ID_Worker, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotChangedIDWorker(const QString&)));

    // Navigation buttons
    connect(ui->p_button_CreateNew, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));

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

    connect(ui->p_dateEdit_StartDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateEdit_StartDate, "StartDate");

    connect(ui->p_dateEdit_EndDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateEdit_EndDate, "EndDate");

    // Modify file
    connect(ui->p_button_AddVacation, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_line_ID_Vacation, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_line_ID_Worker, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_dateEdit_StartDate, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_dateEdit_EndDate, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
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
    m_filename.clear();
    ui->p_list_Vacations->clear();

    ui->p_line_ID_Vacation->setEnabled(false);
    ui->p_line_ID_Worker->setEnabled(false);
    ui->p_dateEdit_StartDate->setEnabled(false);
    ui->p_dateEdit_EndDate->setEnabled(false);

    ui->p_line_Surname->setEnabled(false);
    ui->p_line_FirstName->setEnabled(false);
    ui->p_line_LastName->setEnabled(false);

    ui->p_line_ID_Vacation->setText("");
    ui->p_line_ID_Worker->setText("");
    ui->p_dateEdit_StartDate->setDate(QDate::currentDate());
    ui->p_dateEdit_EndDate->setDate(QDate::currentDate());
}

SingleVacation VacationWidget::getInformationFromForm()
{
    SingleVacation info;

    info.ID_vacation = ui->p_line_ID_Vacation->text().toInt();
    info.ID_worker = ui->p_line_ID_Worker->text().toInt();
    info.StartDate = ui->p_dateEdit_StartDate->date().toString();
    info.EndDate = ui->p_dateEdit_EndDate->date().toString();

    return info;
}

void VacationWidget::setInformationToForm(const SingleVacation& info)
{
    ui->p_line_ID_Vacation->setText(QString::number(info.ID_vacation));
    ui->p_line_ID_Worker->setText(QString::number(info.ID_worker));
    ui->p_dateEdit_StartDate->setDate(QDate::fromString(info.StartDate));
    ui->p_dateEdit_EndDate->setDate(QDate::fromString(info.EndDate));
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
    single.StartDate = QDate::currentDate().toString();
    single.EndDate = QDate::currentDate().toString();

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
        ui->p_dateEdit_StartDate->setEnabled(false);
        ui->p_dateEdit_EndDate->setEnabled(false);

        ui->p_line_Surname->setEnabled(false);
        ui->p_line_FirstName->setEnabled(false);
        ui->p_line_LastName->setEnabled(false);

        ui->p_line_ID_Vacation->setText("");
        ui->p_line_ID_Worker->setText("");
        ui->p_dateEdit_StartDate->setDate(QDate::currentDate());
        ui->p_dateEdit_EndDate->setDate(QDate::currentDate());
    }

    // Вказуємо що файл модифіковано
    slotModifyFile();

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
    ui->p_dateEdit_StartDate->setEnabled(true);
    ui->p_dateEdit_EndDate->setEnabled(true);

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
        if (ui->p_dateEdit_StartDate->date() > ui->p_dateEdit_EndDate->date())
        {
            ui->p_dateEdit_EndDate->setDate(ui->p_dateEdit_StartDate->date());
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
            typedef Worker (*func_readWorkerInfo)(const QString&);
            QLibrary xml_parser("XML_Parsing");
            func_readWorkerInfo readWorker = (func_readWorkerInfo)xml_parser.resolve("read_WorkerInfoFromFile");

            Worker currentWorker = readWorker(str);

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

    if (ui->p_line_ID_Worker->text().isEmpty() && ui->p_list_Vacations->count() == 0)
        isModifiedFile = false;
}

void VacationWidget::slotCreateNew()
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

void VacationWidget::slotOpen()
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

void VacationWidget::openFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef Vacations (*func_readVacations)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readVacations readVacations = (func_readVacations)xmlParser.resolve("read_VacationsFromFile");

    m_vacations = readVacations(filename);
    ui->p_list_Vacations->clear();

    for (auto it : m_vacations)
    {
        ui->p_list_Vacations->addItem(QString::number(it.ID_vacation));
    }

    isModifiedFile = false;
}

void VacationWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writeVacationsToFile)(const QString&, const Vacations&);
    QLibrary xmlParser("XML_Parsing");
    func_writeVacationsToFile writeVacations = (func_writeVacationsToFile)xmlParser.resolve("write_VacationsToFile");

    writeVacations(m_filename, m_vacations);
    isModifiedFile = false;
}

void VacationWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", m_defaultPath, "*.xml");
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

void VacationWidget::closeEvent(QCloseEvent *event)
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

void VacationWidget::slotModifyFile()
{
    isModifiedFile = true;
}

void VacationWidget::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_layout_navigationButtons->count(); i++)
    {
        ui->p_layout_navigationButtons->itemAt(i)->widget()->setVisible(showState);
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

void VacationWidget::slotSetDefaulPath(const QString& path)
{
    m_defaultPath = path;
}
