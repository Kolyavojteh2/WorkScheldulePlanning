#include "positionswidget.h"
#include "ui_positionswidget.h"
#include <QLibrary>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

PositionsWidget::PositionsWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PositionsWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void PositionsWidget::setupWidget()
{
    ui->p_line_Brief->setEnabled(false);
    ui->p_line_FullName->setEnabled(false);
    ui->p_textEdit_Description->setEnabled(false);

    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));

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
}

PositionsWidget::~PositionsWidget()
{
    delete ui;
}

int PositionsWidget::findLastNumberUnnamedPosition()
{
    int number = 0;
    for (int i = 0; i < ui->p_list_Positions->count(); i++)
    {
        if (ui->p_list_Positions->item(i)->text().indexOf("Unnamed position") == -1)
            continue;
        else
        {
            QString str = ui->p_list_Positions->item(i)->text();
            str.remove(0, QString("Unnamed position").count());

            if (number < str.toInt())
                number = str.toInt();
        }
    }

    return number;
}

void PositionsWidget::slotUpdateBrief()
{
    auto iter_currentPosition = m_currentPositions.find(m_currentEditPositionName);
    if (iter_currentPosition == m_currentPositions.end())
        return;

    QString newBrief = ui->p_line_Brief->text();

    // Пошук одинакових коротких імен і якщо знайдено, то присвоїти дефолтний
    if (m_currentEditPositionName != newBrief)
    {
        for (auto it = m_currentPositions.begin(); it != m_currentPositions.end(); it++)
        {
            if (newBrief == it.value().Brief)
            {
                newBrief = "Unnamed position " + QString::number(findLastNumberUnnamedPosition() + 1);
                ui->p_line_Brief->setText(newBrief);
                break;
            }
        }
    }
    //

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
        m_currentPositions.insert(newItem->text(), iter_currentPosition.value());

        delete oldItem;
        m_currentPositions.erase(iter_currentPosition);
    }
}

void PositionsWidget::slotUpdateFullName()
{
    auto iter_currentPosition = m_currentPositions.find(m_currentEditPositionName);
    if (iter_currentPosition == m_currentPositions.end())
        return;

    QString newFullName = ui->p_line_FullName->text();
    iter_currentPosition.value().FullName = newFullName;
}

void PositionsWidget::slotUpdateDescription()
{
    auto iter_currentPosition = m_currentPositions.find(m_currentEditPositionName);
    if (iter_currentPosition == m_currentPositions.end())
        return;

    iter_currentPosition.value().Description = ui->p_textEdit_Description->toPlainText();
}

void PositionsWidget::resetData()
{
    ui->p_list_Positions->clear();
    m_currentEditPositionName.clear();
    setInformationToForm(Position());

    ui->p_line_Brief->setEnabled(false);
    ui->p_line_FullName->setEnabled(false);
    ui->p_textEdit_Description->setEnabled(false);
}

void PositionsWidget::slotSetPositionsData(Positions info)
{
    m_currentPositions = info;
    for (auto it = m_currentPositions.begin(); it != m_currentPositions.end(); it++)
    {
        ui->p_list_Positions->addItem(it->Brief);
    }
}

void PositionsWidget::slotSave()
{
    isSaved = true;
    emit signalSendPositions(m_currentPositions);
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

    auto iterator_existing = m_currentPositions.find(name);
    if (iterator_existing != m_currentPositions.end())
        return;

    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Positions->addItem(item);

    Position pos;
    pos.Brief = name;

    m_currentPositions.insert(item->text(), pos);
}

void PositionsWidget::slotDeletePosition()
{
    if (ui->p_list_Positions->selectedItems().isEmpty())
        return;

    auto item = ui->p_list_Positions->selectedItems().first();

    auto iterator_pos = m_currentPositions.find(item->text());
    m_currentPositions.erase(iterator_pos);

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

    auto foundItem = m_currentPositions.end();
    for (auto it = m_currentPositions.begin(); it != m_currentPositions.end(); it++)
    {
        if (it->Brief == item->text())
        {
            foundItem = it;
            break;
        }
    }

    if (foundItem == m_currentPositions.end())
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
