#include "workersearchresult.h"
#include "ui_workersearchresult.h"

WorkerSearchResult::WorkerSearchResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkerSearchResult)
{
    ui->setupUi(this);

    QStringList headers;
    headers << "ID" << "Surname" << "First name" << "Last name" << "Positions";
    ui->p_treeWidget_Workers->setHeaderLabels(headers);

    connect(ui->p_line_Surname, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_FirstName, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_line_LastName, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateResults()));
    connect(ui->p_comboBox_Position, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(updateResults()));

    this->setWindowTitle(tr("Find worker"));
}

WorkerSearchResult::~WorkerSearchResult()
{
    delete ui;
}

void WorkerSearchResult::setWorkers(Workers list)
{
    m_listOfWorkers = list;
}

void WorkerSearchResult::updateResults()
{
    QString Surname = ui->p_line_Surname->text();
    QString FirstName = ui->p_line_FirstName->text();
    QString LastName = ui->p_line_LastName->text();
    QString Position = ui->p_comboBox_Position->currentText();

    if (Position == "*")
        Position.clear();

    QRegExp regexp_Surname(Surname, Qt::CaseSensitivity::CaseInsensitive);
    QRegExp regexp_FirstName(FirstName, Qt::CaseSensitivity::CaseInsensitive);
    QRegExp regexp_LastName(LastName, Qt::CaseSensitivity::CaseInsensitive);
    QRegExp regexp_Position(Position, Qt::CaseSensitivity::CaseInsensitive);

    ui->p_treeWidget_Workers->clear();
    int countItems = 0;
    for (auto it : m_listOfWorkers)
    {
        if (regexp_Surname.indexIn(it.Surname) != -1 &&
                regexp_FirstName.indexIn(it.FirstName) != -1 &&
                regexp_LastName.indexIn(it.LastName) != -1)
        {
            if (it.ListPositions.filter(regexp_Position.pattern(), Qt::CaseSensitivity::CaseInsensitive).isEmpty())
                continue;

            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setData(0, Qt::ItemDataRole::DisplayRole, QVariant(it.ID_worker));
            item->setText(1, it.Surname);
            item->setText(2, it.FirstName);
            item->setText(3, it.LastName);
            item->setText(4, it.ListPositions.join('\n'));

            ui->p_treeWidget_Workers->addTopLevelItem(item);
            countItems++;
        }
    }
    ui->p_treeWidget_Workers->sortItems(0, Qt::SortOrder::AscendingOrder);

    QString foundTitle;
    if (countItems)
    {
        foundTitle = QString("Found %1 workers")
                .arg(QString::number(countItems));
    }
    else
    {
        foundTitle = QString("Nothing found");
    }

    ui->p_groupBox_Tree->setTitle(foundTitle);
}

void WorkerSearchResult::setPositions(Positions list)
{
    m_listOfPositions = list;

    QStringList positions;
    for (auto it : m_listOfPositions)
    {
        positions.push_back(it.Brief);
    }

    ui->p_comboBox_Position->addItem("*");
    ui->p_comboBox_Position->addItems(positions);
}
