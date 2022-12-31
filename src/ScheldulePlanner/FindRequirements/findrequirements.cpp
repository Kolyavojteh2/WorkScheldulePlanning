#include "findrequirements.h"
#include "ui_findrequirements.h"

FindRequirements::FindRequirements(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindRequirements)
{
    ui->setupUi(this);

    QStringList headers;
    headers << "ID" << "Position" << "Start date" << "End date" << "Count of workers";
    ui->p_treeWidget_Requirements->setHeaderLabels(headers);

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_comboBox_Positions, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(updateResults()));

    this->setWindowTitle(tr("Find requirements"));
}

FindRequirements::~FindRequirements()
{
    delete ui;
}

void FindRequirements::setPositions(Positions list)
{
    m_listOfPositions = list;

    QStringList positions;
    for (auto it : m_listOfPositions)
    {
        positions.push_back(it.Brief);
    }

    ui->p_comboBox_Positions->addItem("*");
    ui->p_comboBox_Positions->addItems(positions);
}

void FindRequirements::setRequirements(Requirements list)
{
    m_requirements = list;
}

void FindRequirements::updateResults()
{
    QString position = ui->p_comboBox_Positions->currentText();
    QDateTime startDate = ui->p_dateTimeEdit_StartDate->dateTime();
    QDateTime endDate = ui->p_dateTimeEdit_EndDate->dateTime();

    if (position == "*")
        position.clear();

    QRegExp position_regexp(position);

    int countItems = 0;

    ui->p_treeWidget_Requirements->clear();
    for (auto it : m_requirements)
    {
        if (position_regexp.indexIn(it.Position) == -1)
            continue;

        QDateTime it_startDate = QDateTime::fromString(it.StartDate);
        QDateTime it_endDate = QDateTime::fromString(it.EndDate);

        if (inRangeDateTime(it_startDate, startDate, endDate) ||
                inRangeDateTime(it_endDate, startDate, endDate))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setData(0, Qt::ItemDataRole::DisplayRole, QVariant(it.ID));
            item->setText(1, it.Position);
            item->setText(2, it_startDate.toString("dd.MM.yyyy hh:mm"));
            item->setText(3, it_endDate.toString("dd.MM.yyyy hh:mm"));
            item->setText(4, QString::number(it.NumberWorkers));

            ui->p_treeWidget_Requirements->addTopLevelItem(item);
            countItems++;
        }

    }

    ui->p_treeWidget_Requirements->sortItems(0, Qt::SortOrder::AscendingOrder);

    QString foundTitle;
    if (countItems)
    {
        foundTitle = QString("Found %1 requirements")
                .arg(QString::number(countItems));
    }
    else
    {
        foundTitle = QString("Nothing found");
    }

    ui->p_groupBox_Result->setTitle(foundTitle);
}

void FindRequirements::setStartDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_StartDate->setDateTime(dateTime);
}

void FindRequirements::setEndDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_EndDate->setDateTime(dateTime);
}

bool FindRequirements::inRangeDateTime(const QDateTime& value,
                                       const QDateTime& start,
                                       const QDateTime& end)
{
    if (start <= value && value < end)
        return true;
    else
        return false;
}
