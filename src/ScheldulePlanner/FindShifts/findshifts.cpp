#include "findshifts.h"
#include "ui_findshifts.h"

FindShifts::FindShifts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindShifts)
{
    ui->setupUi(this);

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_comboBox_Positions, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_checkBox_OnlyNotAssigned, SIGNAL(stateChanged(int)),
            this, SLOT(updateResults()));

    this->setWindowTitle(tr("Find shifts"));
}

FindShifts::~FindShifts()
{
    delete ui;
}

void FindShifts::setPositions(Positions list)
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

void FindShifts::setShifts(Shifts list)
{
    m_shifts = list;
}

void FindShifts::setAssignedShifts(AssignedShifts list)
{
    m_assigned = list;
}

void FindShifts::setStartDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_StartDate->setDateTime(dateTime);
}

void FindShifts::setEndDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_EndDate->setDateTime(dateTime);
}

bool FindShifts::inRangeDateTime(const QDateTime& value,
                                 const QDateTime& start,
                                 const QDateTime& end)
{
    if (start <= value && value < end)
        return true;
    else
        return false;
}


void FindShifts::updateResults()
{
    QString position = ui->p_comboBox_Positions->currentText();
    QDateTime startDate = ui->p_dateTimeEdit_StartDate->dateTime();
    QDateTime endDate = ui->p_dateTimeEdit_EndDate->dateTime();
    bool onlyNotAssigned = ui->p_checkBox_OnlyNotAssigned->isChecked();

    if (position == "*")
        position.clear();

    QRegExp position_regexp(position);

    int countItems = 0;

    ui->p_treeWidget_Shifts->clear();
    for (auto it : m_shifts)
    {
        if (onlyNotAssigned)
        {
            auto iter_assigned = m_assigned.find(QString::number(it.ID_Shifts));
            if (iter_assigned != m_assigned.end())
                continue;
        }
         ///////

        if (position_regexp.indexIn(it.Position) == -1)
            continue;

        QDateTime it_startDate = QDateTime::fromString(it.StartDate);
        QDateTime it_endDate = QDateTime::fromString(it.EndDate);

        if (inRangeDateTime(it_startDate, startDate, endDate) ||
                inRangeDateTime(it_endDate, startDate, endDate))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setData(0, Qt::ItemDataRole::DisplayRole, QVariant(it.ID_Shifts));
            item->setText(1, it.Position);
            item->setText(2, it_startDate.toString("dd.MM.yyyy hh:mm"));
            item->setText(3, it_endDate.toString("dd.MM.yyyy hh:mm"));

            int duration = it_startDate.secsTo(it_endDate);
            int hours = duration / (60*60);
            int minutes = (duration - (hours * 60 * 60)) / 60;

            QString strDuration;
            strDuration.append(QString::number(hours) + tr(" hours"));
            if (minutes != 0)
                strDuration.append(QString::number(minutes) + tr(" minutes"));

            item->setText(4, strDuration);

            ui->p_treeWidget_Shifts->addTopLevelItem(item);
            countItems++;
        }
    }

    ui->p_treeWidget_Shifts->sortItems(0, Qt::SortOrder::AscendingOrder);

    QString foundTitle;
    if (countItems)
    {
        foundTitle = QString("Found %1 shifts")
                .arg(QString::number(countItems));
    }
    else
    {
        foundTitle = QString("Nothing found");
    }

    ui->p_groupBox_Result->setTitle(foundTitle);
}
