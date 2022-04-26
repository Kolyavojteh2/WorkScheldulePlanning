#include "workersinformation.h"
#include "ui_workersinformation.h"
#include <QLibrary>
#include <../../src/XML_Parsing/xml_parsing.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>

void WorkersInformation::setupWidget()
{
    // Створення об'єкту контекстного меню
    p_contextMenu = new QMenu(tr("Context Menu"));
    p_contextMenu->addAction(tr("Add new position"), this, SLOT(slotAddPosition()));
    p_contextMenu->addSeparator();
    p_contextMenu->addAction(tr("Delete selected"), this, SLOT(slotDeletePosition()));

    validateData();

    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_list_Positions, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(slotContextMenu(const QPoint&)));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_Close, SIGNAL(clicked()),
            this, SLOT(slotClose()));
    connect(ui->p_button_NewWorker, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));
}

WorkersInformation::WorkersInformation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkersInformation)
    , m_defaultPath("./projects/")
{
    ui->setupUi(this);
    setupWidget();
}

WorkersInformation::WorkersInformation(const QString filename, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkersInformation)
    , m_filename(filename)
    , m_defaultPath("./projects/")
{
    // Базове налаштування віджета
    ui->setupUi(this);
    setupWidget();

    openFile(filename);
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
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    Worker info = getInformationFromForm();

    typedef void (*func_writeWorkerToFile)(const QString&, const Worker&);
    QLibrary xmlParser("XML_Parsing");
    func_writeWorkerToFile writeWorker = (func_writeWorkerToFile)xmlParser.resolve("write_WorkerInfoToFile");

    writeWorker(m_filename, info);
    m_oldVersion = info;

    slotChangeWindowTitle(tr("Worker ID: ") + QString().number(info.ID_worker));
}

void WorkersInformation::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", m_defaultPath);
    if (!str.isEmpty())
    {
        m_filename = str;
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
    QListWidgetItem* elem = new QListWidgetItem(tr("Unnamed position"));
    elem->setFlags(elem->flags() | Qt::ItemIsEditable);
    ui->p_list_Positions->addItem(elem);
}

void WorkersInformation::slotDeletePosition()
{
    auto item = ui->p_list_Positions->currentItem();
    delete item;
}

void WorkersInformation::slotContextMenu(const QPoint& p)
{
    if (ui->p_list_Positions->selectedItems().isEmpty())
        p_contextMenu->actions().last()->setEnabled(false);
    else
        p_contextMenu->actions().last()->setEnabled(true);

    QPoint global = ui->p_list_Positions->mapToGlobal(p);
    p_contextMenu->exec(global);
}

void WorkersInformation::slotClose()
{
    this->close();
}

void WorkersInformation::slotOpen()
{
    if (m_oldVersion != getInformationFromForm())
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
    m_oldVersion = getInformationFromForm();

    // Зміна титулки програми
    slotChangeWindowTitle(tr("Worker ID:") + QString().number(info.ID_worker));
}

void WorkersInformation::slotChangeWindowTitle(const QString& str)
{
    setWindowTitle(str);
}

void WorkersInformation::slotCreateNew()
{
    if (m_oldVersion != getInformationFromForm())
    {
        int result = QMessageBox::warning(this,
                                          tr("No changes have been saved to the file"),
                                          tr("Do you want to save the changes to the file"),
                                          QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);

        if (result == QMessageBox::No)
        {
            resetForm();
            m_filename.clear();
            m_oldVersion = Worker();
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
        m_oldVersion = Worker();
    }
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
    if (m_oldVersion != getInformationFromForm())
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
