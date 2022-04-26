#include "requirement.h"
#include "ui_requirement.h"
#include <QRegExp>
#include <QDate>
#include <QTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QLibrary>
#include <QCloseEvent>
#include "../../src/XML_Parsing/Positions.h"

RequirementWidget::RequirementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Requirement)
{
    ui->setupUi(this);
    setupWidget();
}

RequirementWidget::~RequirementWidget()
{
    delete ui;
}

void RequirementWidget::setupWidget()
{
    validateData();
    resetData();

    // Navigation buttons
    connect(ui->p_button_CreateNew, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));

    // Editing connections
    p_editingMapper = new QSignalMapper;
    connect(p_editingMapper, SIGNAL(mapped(const QString&)),
            this, SLOT(slotUpdateData(const QString&)));

    connect(ui->p_line_ID, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_line_ID, "ID");

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateTimeEdit_StartDate, "StartDate");

    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateTimeEdit_EndDate, "EndDate");

    // Modify file
    connect(ui->p_comboBox_Position, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotChangedPosition(int)));
    connect(ui->p_line_ID, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_spinbox_NumberWorkers, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_spinbox_Severity, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_checkbox_BiggerAmountWorkers, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_checkbox_Neglect, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_button_AddRequirement, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_button_DeleteRequirement, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));

    // List of requirements
    connect(ui->p_button_AddRequirement, SIGNAL(clicked()),
            this, SLOT(slotAddRequirement()));
    connect(ui->p_button_DeleteRequirement, SIGNAL(clicked()),
            this, SLOT(slotDeleteRequirement()));
    connect(ui->p_list_Requirements, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(slotEditRequirement(QListWidgetItem *)));
}

void RequirementWidget::slotChangedPosition(int)
{
    m_requirements[m_currentEditRequirement] = getInformationFromForm();
    slotModifyFile();
}

void RequirementWidget::validateData()
{
    ui->p_line_ID->setValidator(new QRegExpValidator(QRegExp("^\\d+$")));
}

void RequirementWidget::resetData()
{
    ui->p_line_ID->setText("");

    QTime temp_time(8,0,0);
    ui->p_dateTimeEdit_StartDate->setDate(QDate::currentDate());
    ui->p_dateTimeEdit_StartDate->setTime(temp_time);

    temp_time.setHMS(22,0,0);
    ui->p_dateTimeEdit_EndDate->setDate(QDate::currentDate());
    ui->p_dateTimeEdit_EndDate->setTime(temp_time);

    ui->p_spinbox_NumberWorkers->setValue(1);
    ui->p_spinbox_Severity->setValue(0);

    ui->p_checkbox_BiggerAmountWorkers->setCheckState(Qt::CheckState::Unchecked);
    ui->p_checkbox_BiggerAmountWorkers->setTristate(false);
    ui->p_checkbox_Neglect->setCheckState(Qt::CheckState::Unchecked);
    ui->p_checkbox_Neglect->setTristate(false);

    ui->p_line_ID->setEnabled(false);
    ui->p_dateTimeEdit_StartDate->setEnabled(false);
    ui->p_dateTimeEdit_EndDate->setEnabled(false);
    ui->p_spinbox_NumberWorkers->setEnabled(false);
    ui->p_spinbox_Severity->setEnabled(false);
    ui->p_checkbox_BiggerAmountWorkers->setEnabled(false);
    ui->p_checkbox_Neglect->setEnabled(false);
    ui->p_comboBox_Position->setEnabled(false);

    m_requirements.clear();
    ui->p_list_Requirements->clear();
    m_currentEditRequirement.clear();
    m_filename.clear();
}

SingleRequirement RequirementWidget::getInformationFromForm()
{
    SingleRequirement info;

    info.ID = ui->p_line_ID->text().toInt();
    info.StartDate = ui->p_dateTimeEdit_StartDate->dateTime().toString();
    info.EndDate = ui->p_dateTimeEdit_EndDate->dateTime().toString();
    info.Position = ui->p_comboBox_Position->currentText();
    info.NumberWorkers = ui->p_spinbox_NumberWorkers->value();
    info.Severity = ui->p_spinbox_Severity->value();
    info.BiggerAmountWorkers = ui->p_checkbox_BiggerAmountWorkers->checkState();
    info.Neglect = ui->p_checkbox_Neglect->checkState();

    return info;
}

void RequirementWidget::setInformationToForm(const SingleRequirement& info)
{
    ui->p_line_ID->setText(QString::number(info.ID));
    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime::fromString(info.StartDate));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime::fromString(info.EndDate));
    ui->p_comboBox_Position->setCurrentIndex(ui->p_comboBox_Position->findText(info.Position));
    ui->p_spinbox_NumberWorkers->setValue(info.NumberWorkers);
    ui->p_spinbox_Severity->setValue(info.Severity);
    ui->p_checkbox_BiggerAmountWorkers->setCheckState(static_cast<Qt::CheckState>(info.BiggerAmountWorkers));
    ui->p_checkbox_Neglect->setCheckState(static_cast<Qt::CheckState>(info.Neglect));
}

int RequirementWidget::findLastIDInList()
{
    int result = 0;
    for (auto it : m_requirements)
    {
        if (it.ID > result)
            result = it.ID;
    }

    return result;
}

void RequirementWidget::slotAddRequirement()
{
    // Пошук останнього ID в списку і створення нового імені для нового ID
    int lastID = findLastIDInList() + 1;
    QString name = QString::number(lastID);

    // Створення нового елемента списку
    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Requirements->addItem(item);

    // Створення нового елемента словника та заповнення його даними
    SingleRequirement single;
    single.ID = lastID;

    QDateTime temp_dateTime;
    temp_dateTime.setDate(QDate::currentDate());
    temp_dateTime.setTime(QTime(8,0,0));

    single.StartDate = temp_dateTime.toString();
    temp_dateTime.setTime(QTime(22,0,0));
    single.EndDate = temp_dateTime.toString();

    single.NumberWorkers = 1;
    single.Severity = 0;

    single.BiggerAmountWorkers = Qt::CheckState::Unchecked;
    single.Neglect = Qt::CheckState::Unchecked;

    m_requirements.insert(item->text(), single);
}

void RequirementWidget::slotDeleteRequirement()
{
    // Якщо список пустий, то виходимо
    if (ui->p_list_Requirements->selectedItems().isEmpty())
        return;

    // Отримуємо вказівник на елемент
    auto item = ui->p_list_Requirements->selectedItems().first();

    // Видаляємо із словника елемент
    auto iterator_pos = m_requirements.find(item->text());
    m_requirements.erase(iterator_pos);

    // Якщо цей елемент що ми видаляємо зараз редагується, то треба очистити форму і заблокувати її
    if (item->text() == m_currentEditRequirement)
    {
        m_currentEditRequirement.clear();

        ui->p_line_ID->setText("");

        QTime temp_time(8,0,0);
        ui->p_dateTimeEdit_StartDate->setDate(QDate::currentDate());
        ui->p_dateTimeEdit_StartDate->setTime(temp_time);

        temp_time.setHMS(22,0,0);
        ui->p_dateTimeEdit_EndDate->setDate(QDate::currentDate());
        ui->p_dateTimeEdit_EndDate->setTime(temp_time);

        ui->p_spinbox_NumberWorkers->setValue(1);
        ui->p_spinbox_Severity->setValue(0);

        ui->p_checkbox_BiggerAmountWorkers->setCheckState(Qt::CheckState::Unchecked);
        ui->p_checkbox_BiggerAmountWorkers->setTristate(false);
        ui->p_checkbox_Neglect->setCheckState(Qt::CheckState::Unchecked);
        ui->p_checkbox_Neglect->setTristate(false);

        ui->p_line_ID->setEnabled(false);
        ui->p_dateTimeEdit_StartDate->setEnabled(false);
        ui->p_dateTimeEdit_EndDate->setEnabled(false);
        ui->p_spinbox_NumberWorkers->setEnabled(false);
        ui->p_spinbox_Severity->setEnabled(false);
        ui->p_checkbox_BiggerAmountWorkers->setEnabled(false);
        ui->p_checkbox_Neglect->setEnabled(false);
        ui->p_comboBox_Position->setEnabled(false);
    }

    // Вказуємо що файл модифіковано
    slotModifyFile();

    // Видаляємо елемент із списку форми
    delete item;
}

void RequirementWidget::slotModifyFile()
{
    isModifiedFile = true;
}

void RequirementWidget::slotEditRequirement(QListWidgetItem *item)
{
    // save data from form to memory
    if (m_currentEditRequirement != "")
    {
        m_requirements[m_currentEditRequirement] = getInformationFromForm();
    }

    // Load new data
    m_currentEditRequirement = item->text();

    auto foundItem = m_requirements.end();
    for (auto it = m_requirements.begin(); it != m_requirements.end(); it++)
    {
        if (QString::number(it->ID) == item->text())
        {
            foundItem = it;
            break;
        }
    }

    if (foundItem == m_requirements.end())
        return;

    setInformationToForm(*foundItem);

    ui->p_line_ID->setEnabled(true);
    ui->p_dateTimeEdit_StartDate->setEnabled(true);
    ui->p_dateTimeEdit_EndDate->setEnabled(true);
    ui->p_comboBox_Position->setEnabled(true);
    ui->p_spinbox_NumberWorkers->setEnabled(true);
    ui->p_spinbox_Severity->setEnabled(true);
    ui->p_checkbox_Neglect->setEnabled(true);
    ui->p_checkbox_BiggerAmountWorkers->setEnabled(true);
}

void RequirementWidget::slotUpdateData(const QString& attributeName)
{
    if (attributeName == "ID")
    {
        auto iter_currentRequirement = m_requirements.find(m_currentEditRequirement);
        if (iter_currentRequirement == m_requirements.end())
            return;

        QString newID_Requirement = ui->p_line_ID->text();

        // Пошук одинакових ID і якщо знайдено, то присвоїти дефолтний
        if (m_currentEditRequirement.toInt() != newID_Requirement.toInt())
        {
            for (auto it = m_requirements.begin(); it != m_requirements.end(); it++)
            {
                if (newID_Requirement.toInt() == it.value().ID)
                {
                    newID_Requirement = QString::number(findLastIDInList() + 1);
                    ui->p_line_ID->setText(newID_Requirement);
                    break;
                }
            }
        }
        //

        iter_currentRequirement.value().ID = newID_Requirement.toInt();

        if (QString::number(iter_currentRequirement.value().ID) == iter_currentRequirement.key())
        {
            return;
        }
        else
        {
            QListWidgetItem *newItem = new QListWidgetItem(newID_Requirement);
            QListWidgetItem *oldItem = ui->p_list_Requirements->currentItem();
            ui->p_list_Requirements->addItem(newItem);
            m_currentEditRequirement = newItem->text();
            m_requirements.insert(newItem->text(), iter_currentRequirement.value());

            delete oldItem;
            m_requirements.erase(iter_currentRequirement);

            ui->p_list_Requirements->setCurrentItem(newItem);
        }

        return;
    }

    //if (attributeName == "ID worker") {}

    if (attributeName == "StartDate" || attributeName == "EndDate")
    {
        if (ui->p_dateTimeEdit_StartDate->date() > ui->p_dateTimeEdit_EndDate->date())
        {
            ui->p_dateTimeEdit_EndDate->setDate(ui->p_dateTimeEdit_StartDate->date());
        }
    }

    // save data from form to memory
    if (m_currentEditRequirement != "")
    {
        m_requirements[m_currentEditRequirement] = getInformationFromForm();
    }
}

void RequirementWidget::slotCreateNew()
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

void RequirementWidget::slotOpen()
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
                                                       "",
                                                       "*.xml");
            openFile(str);
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();

            QString str = QFileDialog::getOpenFileName(this,
                                                       "Open...",
                                                       "",
                                                       "*.xml");
            openFile(str);
        }
    }
    else
    {
        QString str = QFileDialog::getOpenFileName(this,
                                                   "Open...",
                                                   "",
                                                   "*.xml");
        openFile(str);
    }
}

void RequirementWidget::openFile(const QString& filename)
{
    resetData();

    if (filename.isEmpty())
        return;

    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef Requirements (*func_readRequirements)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readRequirements readRequirements = (func_readRequirements)xmlParser.resolve("read_RequirementsFromFile");

    m_requirements = readRequirements(filename);
    ui->p_list_Requirements->clear();

    for (auto it : m_requirements)
    {
        ui->p_list_Requirements->addItem(QString::number(it.ID));
    }

    isModifiedFile = false;
}

void RequirementWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writeRequirementsToFile)(const QString&, const Requirements&);
    QLibrary xmlParser("XML_Parsing");
    func_writeRequirementsToFile writeRequirements = (func_writeRequirementsToFile)xmlParser.resolve("write_RequirementsToFile");

    writeRequirements(m_filename, m_requirements);
    isModifiedFile = false;
}

void RequirementWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", "");
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

void RequirementWidget::closeEvent(QCloseEvent *event)
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

void RequirementWidget::slotSetPathPositionsInformation(const QString& path)
{
    m_pathPositionsInformation = path;
    ui->p_comboBox_Position->clear();
    loadPositions();
}

void RequirementWidget::loadPositions()
{
    if (!m_pathPositionsInformation.isEmpty())
    {
        QString str = m_pathPositionsInformation + "positions.xml";
        if (QFile::exists(str))
        {
            typedef Positions (*func_readPositionsInfo)(const QString&);
            QLibrary xml_parser("XML_Parsing");
            func_readPositionsInfo readPositions = (func_readPositionsInfo)xml_parser.resolve("read_PositionsFromFile");

            m_positions = readPositions(str);

            for (auto it : m_positions)
            {
                ui->p_comboBox_Position->addItem(it.Brief);
            }
        }
    }
}

void RequirementWidget::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_layout_navigationButtons->count(); i++)
    {
        ui->p_layout_navigationButtons->itemAt(i)->widget()->setVisible(showState);
    }
}
