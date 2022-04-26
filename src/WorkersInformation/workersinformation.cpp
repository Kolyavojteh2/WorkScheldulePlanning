#include "workersinformation.h"
#include "ui_workersinformation.h"
#include <QLibrary>
#include <../../src/XML_Parsing/xml_parsing.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>
#include <QInputDialog>
/*
int WorkersInformation::getLastId()
{
    QFile file_lastID(m_defaultPath + "lastID");
    if (!file_lastID.exists())
    {
        file_lastID.open(QFile::OpenModeFlag::WriteOnly);
        file_lastID.write("1");
        file_lastID.close();
    }
    if (file_lastID.open(QFile::OpenModeFlag::ReadOnly))
    {
        auto byte_arr = file_lastID.readAll();
        m_lastID = byte_arr.toInt();
        file_lastID.close();
    }
}
*/

void WorkersInformation::setupWidget()
{
    validateData();
    slotChangeWindowTitle(tr("Worker ID:") +tr("Unnamed"));

    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));
    connect(ui->p_button_NewWorker, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));

    connect(ui->p_button_AddPosition, SIGNAL(clicked()),
            this, SLOT(slotAddPosition()));
    connect(ui->p_button_DeletePosition, SIGNAL(clicked()),
            this, SLOT(slotDeletePosition()));
}

WorkersInformation::WorkersInformation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkersInformation)
    , m_defaultPath("./projects/")
{
    ui->setupUi(this);
    setupWidget();
}

void WorkersInformation::slotSetDefaulPath(const QString& path)
{
    m_defaultPath = path;
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

    if (m_filename.isEmpty())
    {
        if (QFile::exists(m_defaultPath + "/" + ui->p_line_ID->text() + ".xml"))
        {
            QString msg_text = tr("The worker with ID:") + ui->p_line_ID->text() + tr(" is exists.\nChange ID.");
            QMessageBox::critical(this, tr("Error"), msg_text);
            return;
        }

        if (!m_defaultPath.isEmpty())
        {
            m_filename = m_defaultPath + ui->p_line_ID->text() + ".xml";
        }
        else
        {
            slotSaveAs();
            return;
        }
    }

    Worker info = getInformationFromForm();

    typedef void (*func_writeWorkerToFile)(const QString&, const Worker&);
    QLibrary xmlParser("XML_Parsing");
    func_writeWorkerToFile writeWorker = (func_writeWorkerToFile)xmlParser.resolve("write_WorkerInfoToFile");

    writeWorker(m_filename, info);
    if (!m_filename.isEmpty())
        ui->p_line_ID->setEnabled(false);
    else
        ui->p_line_ID->setEnabled(true);

    isModifiedFile = false;

    slotChangeWindowTitle(tr("Worker ID: ") + QString().number(info.ID_worker));
}

void WorkersInformation::slotSaveAs()
{
    QString str = QFileDialog::getExistingDirectory(this, "Choise dir...", m_defaultPath);
    if (!str.isEmpty())
    {
        m_defaultPath = str + "/";
        m_filename = m_defaultPath + ui->p_line_ID->text() + ".xml";
        slotSave();
    }
}

void WorkersInformation::resetForm()
{
    ui->p_line_ID->setText("0");
    ui->p_line_Surname->setText("");
    ui->p_line_FirstName->setText("");
    ui->p_line_LastName->setText("");
    ui->p_line_PhoneNumber->setText("");
    ui->p_line_Email->setText("");
    ui->p_list_Positions->clear();
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
    slotModifyFile();

    delete item;
}

void WorkersInformation::slotOpen()
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

    if (!m_filename.isEmpty())
        ui->p_line_ID->setEnabled(false);
    else
        ui->p_line_ID->setEnabled(true);
}

void WorkersInformation::openFile(const QString & filename)
{
    if (filename.isEmpty())
        return;

    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef Worker (*func_readWorker)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readWorker readWorker = (func_readWorker)xmlParser.resolve("read_WorkerInfoFromFile");

    Worker info = readWorker(filename);

    // Присвоєння даних робітника елементам форми
    ui->p_line_ID->setText(QString().number(info.ID_worker));
    ui->p_line_Surname->setText(info.Surname);
    ui->p_line_FirstName->setText(info.FirstName);
    ui->p_line_LastName->setText(info.LastName);
    ui->p_line_PhoneNumber->setText(info.PhoneNumber);
    ui->p_line_Email->setText(info.Email);

    ui->p_list_Positions->clear();
    ui->p_list_Positions->addItems(info.ListPositions);
    for (int n_elem = 0; n_elem < ui->p_list_Positions->count(); n_elem++)
    {
        auto elem = ui->p_list_Positions->item(n_elem);
        elem->setFlags(elem->flags() | Qt::ItemIsEditable);
    }

    // Створення копії інформації для порівняння
    isModifiedFile = false;

    // Зміна титулки програми
    slotChangeWindowTitle(tr("Worker ID:") + QString().number(info.ID_worker));
}

void WorkersInformation::slotChangeWindowTitle(const QString& str)
{
    setWindowTitle(str);
}

void WorkersInformation::slotCreateNew()
{
    if (isModifiedFile)
    {
        int result = QMessageBox::warning(this,
                                          tr("No changes have been saved to the file"),
                                          tr("Do you want to save the changes to the file"),
                                          QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);

        if (result == QMessageBox::No)
        {
            resetForm();
            m_filename.clear();
            isModifiedFile = false;
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();
        }
    }
    else
    {
        resetForm();
        m_filename.clear();
        isModifiedFile = false;
    }

    if (!m_filename.isEmpty())
        ui->p_line_ID->setEnabled(false);
    else
        ui->p_line_ID->setEnabled(true);

    slotChangeWindowTitle(tr("Worker ID:") +tr("Unnamed"));

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

void WorkersInformation::closeEvent(QCloseEvent *event)
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

void WorkersInformation::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_layout_navigationButtons->count(); i++)
    {
        ui->p_layout_navigationButtons->itemAt(i)->widget()->setVisible(showState);
    }
}

void WorkersInformation::slotModifyFile()
{
    isModifiedFile = true;
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
            typedef Positions (*func_readPositionsInfo)(const QString&);
            QLibrary xml_parser("XML_Parsing");
            func_readPositionsInfo readPositions = (func_readPositionsInfo)xml_parser.resolve("read_PositionsFromFile");

            m_positions = readPositions(str);
        }
    }
}

