#include "workersinformation.h"
#include "ui_workersinformation.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>
#include <QInputDialog>
#include "../projectdataloader.h"

void WorkersInformation::setupWidget()
{
    validateData();
    slotChangeWindowTitle(tr("Worker ID:") +tr("Unnamed"));

    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));

    connect(ui->p_button_AddPosition, SIGNAL(clicked()),
            this, SLOT(slotAddPosition()));
    connect(ui->p_button_DeletePosition, SIGNAL(clicked()),
            this, SLOT(slotDeletePosition()));
}

WorkersInformation::WorkersInformation(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WorkersInformation)
    , m_defaultPath("./projects/")
{
    ui->setupUi(this);
    setupWidget();
}

Worker WorkersInformation::getInformationFromForm()
{
    Worker info;

    info.ID_worker = ui->p_line_ID->text().toInt();
    info.Surname = ui->p_line_Surname->text();
    info.FirstName = ui->p_line_FirstName->text();
    info.LastName = ui->p_line_LastName->text();
    info.PhoneNumber = ui->p_line_PhoneNumber->text();
    info.Email = ui->p_line_Email->text();

    for (int n_elem = 0; n_elem < ui->p_list_Positions->count(); n_elem++)
    {
        info.ListPositions.append(ui->p_list_Positions->item(n_elem)->text());
    }

    return info;
}

void WorkersInformation::slotSave()
{
    if (ui->p_line_ID->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("The workers ID is not exists.\nSet workers ID."));
        return;
    }

    Worker info = getInformationFromForm();
    emit signalSendWorker(info);

    slotChangeWindowTitle(tr("Worker ID: ") + QString().number(info.ID_worker));
}

void WorkersInformation::slotAddPosition()
{
    QStringList positionList = m_positions.uniqueKeys();
    bool ok;
    QString newItemName = QInputDialog::getItem(this,
                          "New position",
                          "Add the position of worker:",
                          positionList,
                          0,
                          false,
                          &ok);
    if (!ok)
        return;

    QListWidgetItem* elem = new QListWidgetItem(newItemName);

    if (ui->p_list_Positions->findItems(newItemName, Qt::MatchFlag::MatchFixedString).isEmpty())
        ui->p_list_Positions->addItem(elem);
}

void WorkersInformation::slotDeletePosition()
{
    // Якщо список пустий, то виходимо
    if (ui->p_list_Positions->selectedItems().isEmpty())
        return;
    auto item = ui->p_list_Positions->currentItem();

    // Вказуємо що файл модифіковано
    //slotModifyFile();

    delete item;
}

void WorkersInformation::slotDisableID()
{
    ui->p_line_ID->setEnabled(false);
}

void WorkersInformation::slotChangeWindowTitle(const QString& str)
{
    setWindowTitle(str);
}

WorkersInformation::~WorkersInformation()
{
    delete ui;
}

void WorkersInformation::validateData()
{
    ui->p_line_ID->setValidator(new QRegExpValidator(QRegExp("^\\d+$")));
    ui->p_line_Surname->setValidator(new QRegExpValidator(QRegExp("\\D+")));
    ui->p_line_LastName->setValidator(new QRegExpValidator(QRegExp("\\D+")));
    ui->p_line_FirstName->setValidator(new QRegExpValidator(QRegExp("\\D+")));
    ui->p_line_PhoneNumber->setValidator(new QRegExpValidator(QRegExp("^[+]\\d+$|^\\d+$")));
    ui->p_line_Email->setValidator(new QRegExpValidator(QRegExp("^[\\w-\\.]+@([\\w-]+\\.)+[\\w-]{2,4}$")));
}

void WorkersInformation::slotSetPathPositionsInformation(const QString& path)
{
    m_pathPositionsInformation = path;
    m_positions.clear();
    loadPositions();
}

void WorkersInformation::loadPositions()
{
    if (!m_pathPositionsInformation.isEmpty())
    {
        QString str = m_pathPositionsInformation + "positions.xml";
        if (QFile::exists(str))
        {
            m_positions = ProjectDataLoader::loadPositions(str);
        }
    }
}

void WorkersInformation::slotSetWorkerData(Worker info)
{
    ui->p_line_ID->setText(QString().number(info.ID_worker));
    ui->p_line_Surname->setText(info.Surname);
    ui->p_line_FirstName->setText(info.FirstName);
    ui->p_line_LastName->setText(info.LastName);
    ui->p_line_PhoneNumber->setText(info.PhoneNumber);
    ui->p_line_Email->setText(info.Email);

    ui->p_list_Positions->clear();
    ui->p_list_Positions->addItems(info.ListPositions);
}

void WorkersInformation::slotChangeButtonText(const QString& str)
{
    ui->p_button_Save->setText(str);
}
