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
    : QDialog(parent)
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
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));

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
    connect(ui->p_comboBox_Position, SIGNAL(activated(int)),
            this, SLOT(slotChangedPosition(int)));

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
    if (ui->p_list_Requirements->count() == 0)
        return;

    m_requirements[m_currentEditRequirement] = getInformationFromForm();
    //slotModifyFile();
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

    ui->p_line_ID->setEnabled(false);
    ui->p_dateTimeEdit_StartDate->setEnabled(false);
    ui->p_dateTimeEdit_EndDate->setEnabled(false);
    ui->p_spinbox_NumberWorkers->setEnabled(false);
    ui->p_comboBox_Position->setEnabled(false);

    m_requirements.clear();
    ui->p_list_Requirements->clear();
    m_currentEditRequirement.clear();
}

SingleRequirement RequirementWidget::getInformationFromForm()
{
    SingleRequirement info;

    info.ID = ui->p_line_ID->text().toInt();
    info.StartDate = ui->p_dateTimeEdit_StartDate->dateTime().toString();
    info.EndDate = ui->p_dateTimeEdit_EndDate->dateTime().toString();
    info.Position = ui->p_comboBox_Position->currentText();
    info.NumberWorkers = ui->p_spinbox_NumberWorkers->value();

    return info;
}

void RequirementWidget::setInformationToForm(const SingleRequirement& info)
{
    ui->p_line_ID->setText(QString::number(info.ID));
    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime::fromString(info.StartDate));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime::fromString(info.EndDate));
    ui->p_comboBox_Position->setCurrentIndex(ui->p_comboBox_Position->findText(info.Position));
    ui->p_spinbox_NumberWorkers->setValue(info.NumberWorkers);
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
    // BugFix: ???????? ???????????????????? ????????????, ???? ???????????????? ?????? ???????????????????????? ???????????????????????? ???? ???????? ????????????
    ui->p_list_Requirements->setFocus();

    // ?????????? ???????????????????? ID ?? ???????????? ?? ?????????????????? ???????????? ?????????? ?????? ???????????? ID
    int lastID = findLastIDInList() + 1;
    QString name = QString::number(lastID);

    // ?????????????????? ???????????? ???????????????? ????????????
    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Requirements->addItem(item);

    // ?????????????????? ???????????? ???????????????? ???????????????? ???? ???????????????????? ???????? ????????????
    SingleRequirement single;
    single.ID = lastID;

    QDateTime temp_dateTime;
    temp_dateTime.setDate(QDate::currentDate());
    temp_dateTime.setTime(QTime(8,0,0));

    single.StartDate = temp_dateTime.toString();
    temp_dateTime.setTime(QTime(22,0,0));
    single.EndDate = temp_dateTime.toString();

    single.NumberWorkers = 1;
    //single.Severity = 0;

    //single.BiggerAmountWorkers = Qt::CheckState::Unchecked;
    //single.Neglect = Qt::CheckState::Unchecked;

    m_requirements.insert(item->text(), single);
}

void RequirementWidget::slotDeleteRequirement()
{
    // ???????? ???????????? ????????????, ???? ????????????????
    if (ui->p_list_Requirements->selectedItems().isEmpty())
        return;

    // ?????????????????? ?????????????????? ???? ??????????????
    auto item = ui->p_list_Requirements->selectedItems().first();

    // ?????????????????? ???? ???????????????? ??????????????
    auto iterator_pos = m_requirements.find(item->text());
    m_requirements.erase(iterator_pos);

    // ???????? ?????? ?????????????? ???? ???? ?????????????????? ?????????? ??????????????????????, ???? ?????????? ???????????????? ?????????? ?? ?????????????????????? ????
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

        ui->p_line_ID->setEnabled(false);
        ui->p_dateTimeEdit_StartDate->setEnabled(false);
        ui->p_dateTimeEdit_EndDate->setEnabled(false);
        ui->p_spinbox_NumberWorkers->setEnabled(false);
        ui->p_comboBox_Position->setEnabled(false);
    }

    // ?????????????????? ?????????????? ???? ???????????? ??????????
    delete item;
}

void RequirementWidget::slotEditRequirement(QListWidgetItem *item)
{
    // BugFix: ???????? ???????????????????? ????????????, ???? ???????????????? ?????? ???????????????????????? ???????????????????????? ???? ???????? ????????????
    ui->p_list_Requirements->setFocus();

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
}

void RequirementWidget::slotUpdateData(const QString& attributeName)
{
    if (attributeName == "ID")
    {
        auto iter_currentRequirement = m_requirements.find(m_currentEditRequirement);
        if (iter_currentRequirement == m_requirements.end())
            return;

        QString newID_Requirement = ui->p_line_ID->text();

        // ?????????? ???????????????????? ID ?? ???????? ????????????????, ???? ?????????????????? ??????????????????
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

void RequirementWidget::slotSave()
{
    isSaved = true;
    emit signalSendRequirements(m_requirements);
}

void RequirementWidget::closeEvent(QCloseEvent *event)
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

void RequirementWidget::slotSetRequirements(Requirements info)
{
    m_requirements = info;
    QMap<int, SingleRequirement> sortedInfo;
    for (auto it = info.begin(); it != info.end(); it++)
    {
        sortedInfo.insert(it.key().toInt(), it.value());
    }

    for (auto it : sortedInfo)
    {
        ui->p_list_Requirements->addItem(QString::number(it.ID));
    }
}

void RequirementWidget::slotSetPositions(Positions info)
{
    m_positions = info;
    ui->p_comboBox_Position->clear();
    for (auto it : m_positions)
    {
        ui->p_comboBox_Position->addItem(it.Brief);
    }
}

void RequirementWidget::slotSetCurrentAndEditRequirement(int id)
{
    auto listOfItems = ui->p_list_Requirements->findItems(QString::number(id), Qt::MatchFlag::MatchFixedString);
    if (listOfItems.isEmpty())
        return;

    auto item = listOfItems.first();

    ui->p_list_Requirements->setCurrentItem(item);
    slotEditRequirement(item);
}
