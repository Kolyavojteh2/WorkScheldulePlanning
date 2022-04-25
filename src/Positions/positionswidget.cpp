#include "positionswidget.h"
#include "ui_positionswidget.h"
#include <QLibrary>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

PositionsWidget::PositionsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PositionsWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void PositionsWidget::validateData() {}

void PositionsWidget::setupWidget()
{
    validateData();

    ui->p_line_Brief->setEnabled(false);
    ui->p_line_FullName->setEnabled(false);
    ui->p_textEdit_Description->setEnabled(false);

    connect(ui->p_button_CreateNew, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));

    connect(ui->p_list_Positions, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(slotEditPosition(QListWidgetItem *)));
    connect(ui->p_button_AddPosition, SIGNAL(clicked()),
            this, SLOT(slotAddPosition()));
    connect(ui->p_button_DeletePosition, SIGNAL(clicked()),
            this, SLOT(slotDeletePosition()));

    connect(ui->p_line_Brief, SIGNAL(editingFinished()),
            this, SLOT(slotUpdateBrief()));
    connect(ui->p_line_FullName, SIGNAL(editingFinished()),
            this, SLOT(slotUpdateFullName()));
    connect(ui->p_textEdit_Description, SIGNAL(textChanged()),
            this, SLOT(slotUpdateDescription()));

    connect(ui->p_button_AddPosition, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_button_DeletePosition, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_line_Brief, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_line_FullName, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_textEdit_Description, SIGNAL(textChanged()),
            this, SLOT(slotModifyFile()));
}

void PositionsWidget::slotSetDefaulPath(const QString& path)
{
    m_defaultPath = path;
}

PositionsWidget::~PositionsWidget()
{
    delete ui;
}

void PositionsWidget::slotUpdateBrief()
{
    auto iter_currentPosition = m_currentPositions.list.find(m_currentEditPositionName);
    if (iter_currentPosition == m_currentPositions.list.end())
        return;

    QString newBrief = ui->p_line_Brief->text();
    iter_currentPosition.value().Brief = newBrief;

    if (iter_currentPosition.value().Brief == iter_currentPosition.key())
    {
        return;
    }
    else
    {
        QListWidgetItem *newItem = new QListWidgetItem(newBrief);
        QListWidgetItem *oldItem = ui->p_list_Positions->selectedItems().first();
        ui->p_list_Positions->addItem(newItem);
        m_currentEditPositionName = newItem->text();
        m_currentPositions.list.insert(newItem->text(), iter_currentPosition.value());

        delete oldItem;
        m_currentPositions.list.erase(iter_currentPosition);
    }
}

void PositionsWidget::slotUpdateFullName()
{
    auto iter_currentPosition = m_currentPositions.list.find(m_currentEditPositionName);
    if (iter_currentPosition == m_currentPositions.list.end())
        return;

    QString newFullName = ui->p_line_FullName->text();
    iter_currentPosition.value().FullName = newFullName;
}

void PositionsWidget::slotUpdateDescription()
{
    auto iter_currentPosition = m_currentPositions.list.find(m_currentEditPositionName);
    if (iter_currentPosition == m_currentPositions.list.end())
        return;

    iter_currentPosition.value().Description = ui->p_textEdit_Description->toPlainText();
}

void PositionsWidget::slotOpen()
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
}

void PositionsWidget::resetData()
{
    ui->p_list_Positions->clear();
    m_currentEditPositionName.clear();
    setInformationToForm(Position());

    ui->p_line_Brief->setEnabled(false);
    ui->p_line_FullName->setEnabled(false);
    ui->p_textEdit_Description->setEnabled(false);

    isModifiedFile = false;
}

void PositionsWidget::slotCreateNew()
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
        resetData();
        m_filename.clear();
        isModifiedFile = false;
    }
}

void PositionsWidget::openFile(const QString & filename)
{
    resetData();
    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef Positions (*func_readPositions)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readPositions readPositions = (func_readPositions)xmlParser.resolve("read_PositionsFromFile");

    Positions info = readPositions(filename);

    m_currentPositions = info;
    for (auto it = m_currentPositions.list.begin(); it != m_currentPositions.list.end(); it++)
    {
        ui->p_list_Positions->addItem(it->Brief);
    }
}

void PositionsWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writePositionsToFile)(const QString&, const Positions&);
    QLibrary xmlParser("XML_Parsing");
    func_writePositionsToFile writePositions = (func_writePositionsToFile)xmlParser.resolve("write_PositionsToFile");

    writePositions(m_filename, m_currentPositions);
    isModifiedFile = false;
}

void PositionsWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", m_defaultPath);
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

Position PositionsWidget::getInformationFromForm()
{
    Position info;
    info.Brief = ui->p_line_Brief->text();
    info.FullName = ui->p_line_FullName->text();
    info.Description = ui->p_textEdit_Description->toPlainText();

    return info;
}

void PositionsWidget::slotAddPosition()
{
    QString name = tr("Unnamed position");

    auto iterator_existing = m_currentPositions.list.find(name);
    if (iterator_existing != m_currentPositions.list.end())
        return;

    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Positions->addItem(item);

    Position pos;
    pos.Brief = name;

    m_currentPositions.list.insert(item->text(), pos);
}

void PositionsWidget::slotModifyFile()
{
    isModifiedFile = true;
}

void PositionsWidget::slotDeletePosition()
{
    if (ui->p_list_Positions->selectedItems().isEmpty())
        return;

    auto item = ui->p_list_Positions->selectedItems().first();

    auto iterator_pos = m_currentPositions.list.find(item->text());
    m_currentPositions.list.erase(iterator_pos);

    if (item->text() == m_currentEditPositionName)
    {
        m_currentEditPositionName.clear();

        Position empty_data;

        setInformationToForm(empty_data);
        ui->p_line_Brief->setEnabled(false);
        ui->p_line_FullName->setEnabled(false);
        ui->p_textEdit_Description->setEnabled(false);
    }

    delete item;
}

void PositionsWidget::slotEditPosition(QListWidgetItem *item)
{
    m_currentEditPositionName = item->text();

    auto foundItem = m_currentPositions.list.end();
    for (auto it = m_currentPositions.list.begin(); it != m_currentPositions.list.end(); it++)
    {
        if (it->Brief == item->text())
        {
            foundItem = it;
            break;
        }
    }

    if (foundItem == m_currentPositions.list.end())
        return;

    setInformationToForm(*foundItem);

    ui->p_line_Brief->setEnabled(true);
    ui->p_line_FullName->setEnabled(true);
    ui->p_textEdit_Description->setEnabled(true);
}

void PositionsWidget::setInformationToForm(const Position& info)
{
    ui->p_line_Brief->setText(info.Brief);
    ui->p_line_FullName->setText(info.FullName);
    ui->p_textEdit_Description->setPlainText(info.Description);
}

void PositionsWidget::closeEvent(QCloseEvent *event)
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