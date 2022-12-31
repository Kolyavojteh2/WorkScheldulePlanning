#include "findassignedshifts.h"
#include "ui_findassignedshifts.h"

FindAssignedShifts::FindAssignedShifts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindAssignedShifts)
{
    ui->setupUi(this);

    ui->p_line_WorkerID->setValidator(new QIntValidator(this));

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_comboBox_Positions, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_WorkerID, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_Surname, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_FirstName, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_LastName, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));

    this->setWindowTitle(tr("Find assigned shifts"));
}

FindAssignedShifts::~FindAssignedShifts()
{
    delete ui;
}

void FindAssignedShifts::setWorkers(Workers workers)
{
    m_workers = workers;
}

void FindAssignedShifts::setPositions(Positions list)
{
    m_positions = list;

    QStringList positions;
    for (auto it : m_positions)
    {
        positions.push_back(it.Brief);
    }

    ui->p_comboBox_Positions->addItem("*");
    ui->p_comboBox_Positions->addItems(positions);
}

void FindAssignedShifts::setShifts(Shifts list)
{
    m_shifts = list;
}

void FindAssignedShifts::setAssigned(AssignedShifts list)
{
    m_assigned = list;
}

void FindAssignedShifts::setStartDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_StartDate->setDateTime(dateTime);
}

void FindAssignedShifts::setEndDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_EndDate->setDateTime(dateTime);
}

bool FindAssignedShifts::inRangeDateTime(const QDateTime& value,
                                 const QDateTime& start,
                                 const QDateTime& end)
{
    if (start <= value && value < end)
        return true;
    else
        return false;
}


void FindAssignedShifts::updateResults()
{

    QString WorkerID = ui->p_line_WorkerID->text();
    QString Surname = ui->p_line_Surname->text();
    QString FirstName = ui->p_line_FirstName->text();
    QString LastName = ui->p_line_LastName->text();
    QString position = ui->p_comboBox_Positions->currentText();
    QDateTime startDate = ui->p_dateTimeEdit_StartDate->dateTime();
    QDateTime endDate = ui->p_dateTimeEdit_EndDate->dateTime();

    if (position == "*")
        position.clear();

    QRegExp Surname_regexp(Surname);
    QRegExp FirstName_regexp(FirstName);
    QRegExp LastName_regexp(LastName);
    QRegExp position_regexp(position);

    int countItems = 0;

    ui->p_treeWidget_Wishes->clear();
    for (auto it : m_assigned)
    {
        if (!WorkerID.isEmpty() && WorkerID.toInt() != it.ID_Worker)
            continue;

        Worker currentWorker = m_workers.find(QString::number(it.ID_Worker)).value();

        if (Surname_regexp.indexIn(currentWorker.Surname) == -1)
            continue;
        if (FirstName_regexp.indexIn(currentWorker.FirstName) == -1)
            continue;
        if (LastName_regexp.indexIn(currentWorker.LastName) == -1)
            continue;

        SingleShift shift = m_shifts.find(QString::number(it.ID_Shift)).value();
        if (position_regexp.indexIn(shift.Position) == -1)
            continue;

        QDateTime it_startDate = QDateTime::fromString(shift.StartDate);
        QDateTime it_endDate = QDateTime::fromString(shift.EndDate);

        if (inRangeDateTime(it_startDate, startDate, endDate) ||
                inRangeDateTime(it_endDate, startDate, endDate))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setData(0, Qt::ItemDataRole::DisplayRole, QVariant(it.ID_Shift));
            item->setData(1, Qt::ItemDataRole::DisplayRole, QVariant(it.ID_Worker));
            item->setText(2, currentWorker.Surname);
            item->setText(3, currentWorker.FirstName);
            item->setText(4, currentWorker.LastName);
            item->setText(5, shift.Position);
            item->setText(6, it_startDate.toString("dd.MM.yyyy hh:mm"));
            item->setText(7, it_endDate.toString("dd.MM.yyyy hh:mm"));

            ui->p_treeWidget_Wishes->addTopLevelItem(item);
            countItems++;
        }
    }

    ui->p_treeWidget_Wishes->sortItems(0, Qt::SortOrder::AscendingOrder);

    QString foundTitle;
    if (countItems)
    {
        foundTitle = QString("Found %1 assigned shifts")
                .arg(QString::number(countItems));
    }
    else
    {
        foundTitle = QString("Nothing found");
    }

    ui->p_groupBox_Result->setTitle(foundTitle);
}

