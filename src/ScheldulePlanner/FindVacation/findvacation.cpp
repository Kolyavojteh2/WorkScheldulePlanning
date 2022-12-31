#include "findvacation.h"
#include "ui_findvacation.h"

FindVacation::FindVacation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindVacation)
{
    ui->setupUi(this);
    ui->p_line_WorkerID->setValidator(new QIntValidator(this));

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_WorkerID, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_Surname, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_FirstName, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_LastName, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));

    this->setWindowTitle(tr("Find vacations"));
}

FindVacation::~FindVacation()
{
    delete ui;
}


void FindVacation::setWorkers(Workers workers)
{
    m_workers = workers;
}

void FindVacation::setVacations(Vacations list)
{
    m_vacations = list;
}

void FindVacation::setStartDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_StartDate->setDateTime(dateTime);
}

void FindVacation::setEndDate(const QDateTime& dateTime)
{
    ui->p_dateTimeEdit_EndDate->setDateTime(dateTime);
}

bool FindVacation::inRangeDateTime(const QDateTime& value,
                                 const QDateTime& start,
                                 const QDateTime& end)
{
    if (start <= value && value < end)
        return true;
    else
        return false;
}

void FindVacation::updateResults()
{

    QString WorkerID = ui->p_line_WorkerID->text();
    QString Surname = ui->p_line_Surname->text();
    QString FirstName = ui->p_line_FirstName->text();
    QString LastName = ui->p_line_LastName->text();
    QDateTime startDate = ui->p_dateTimeEdit_StartDate->dateTime();
    QDateTime endDate = ui->p_dateTimeEdit_EndDate->dateTime();

    QRegExp Surname_regexp(Surname);
    QRegExp FirstName_regexp(FirstName);
    QRegExp LastName_regexp(LastName);

    int countItems = 0;

    ui->p_treeWidget_Vacations->clear();
    for (auto it : m_vacations)
    {
        if (!WorkerID.isEmpty() && WorkerID.toInt() != it.ID_worker)
            continue;

        Worker currentWorker = m_workers.find(QString::number(it.ID_worker)).value();

        if (Surname_regexp.indexIn(currentWorker.Surname) == -1)
            continue;
        if (FirstName_regexp.indexIn(currentWorker.FirstName) == -1)
            continue;
        if (LastName_regexp.indexIn(currentWorker.LastName) == -1)
            continue;

        QDateTime it_startDate = QDateTime::fromString(it.StartDate);
        QDateTime it_endDate = QDateTime::fromString(it.EndDate);

        if (inRangeDateTime(it_startDate, startDate, endDate) ||
                inRangeDateTime(it_endDate, startDate, endDate))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setData(0, Qt::ItemDataRole::DisplayRole, QVariant(it.ID_vacation));
            item->setData(1, Qt::ItemDataRole::DisplayRole, QVariant(it.ID_worker));
            item->setText(2, currentWorker.Surname);
            item->setText(3, currentWorker.FirstName);
            item->setText(4, currentWorker.LastName);
            item->setText(5, it_startDate.toString("dd.MM.yyyy hh:mm"));
            item->setText(6, it_endDate.toString("dd.MM.yyyy hh:mm"));

            ui->p_treeWidget_Vacations->addTopLevelItem(item);
            countItems++;
        }
    }

    ui->p_treeWidget_Vacations->sortItems(0, Qt::SortOrder::AscendingOrder);

    QString foundTitle;
    if (countItems)
    {
        foundTitle = QString("Found %1 vacations")
                .arg(QString::number(countItems));
    }
    else
    {
        foundTitle = QString("Nothing found");
    }

    ui->p_groupBox_Result->setTitle(foundTitle);
}
