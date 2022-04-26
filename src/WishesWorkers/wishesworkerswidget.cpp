#include "wishesworkerswidget.h"
#include "ui_wishesworkerswidget.h"
#include "../../src/XML_Parsing/xml_parsing.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QLibrary>
#include <QCloseEvent>

#include <iostream>

WishesWorkersWidget::WishesWorkersWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WishesWorkersWidget)
{
    ui->setupUi(this);
    setupWidget();
}

WishesWorkersWidget::~WishesWorkersWidget()
{
    delete ui;
}

void WishesWorkersWidget::setupWidget()
{
    validateData();
    resetData();
    slotCheckIDWorker();

    // Navigation buttons
    connect(ui->p_button_CreateNew, SIGNAL(clicked()),
            this, SLOT(slotCreateNew()));
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));

    // Modify file
    connect(ui->p_comboBox_Position, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotChangedPosition(int)));
    connect(ui->p_line_ID_wishes, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_line_ID_worker, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_TextEdit_Addition, SIGNAL(textChanged()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_spinBox_MinWorkerDays, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_spinBox_MaxWorkerDays, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_timeEdit_GetStartedLaterThan, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_timeEdit_GetEndEarlierThan, SIGNAL(editingFinished()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_button_AddWishes, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));
    connect(ui->p_button_DeleteWishes, SIGNAL(clicked()),
            this, SLOT(slotModifyFile()));

    // Editing connections
    p_editingMapper = new QSignalMapper;
    connect(p_editingMapper, SIGNAL(mapped(const QString&)),
            this, SLOT(slotUpdateData(const QString&)));

    connect(ui->p_line_ID_wishes, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_line_ID_wishes, "ID_Wishes");

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateTimeEdit_StartDate, "StartDate");

    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_dateTimeEdit_EndDate, "EndDate");

    connect(ui->p_spinBox_MinWorkerDays, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_spinBox_MinWorkerDays, "MinWorkerDays");

    connect(ui->p_spinBox_MaxWorkerDays, SIGNAL(editingFinished()),
            p_editingMapper, SLOT(map()));
    p_editingMapper->setMapping(ui->p_spinBox_MaxWorkerDays, "MaxWorkerDay");


    // Check ID worker, if ID is empty, then block list
    connect(ui->p_line_ID_worker, SIGNAL(editingFinished()),
            this, SLOT(slotCheckIDWorker()));
    connect(ui->p_line_ID_worker, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotChangedIDWorker(const QString&)));

    // List of requirements
    connect(ui->p_button_AddWishes, SIGNAL(clicked()),
            this, SLOT(slotAddWish()));
    connect(ui->p_button_DeleteWishes, SIGNAL(clicked()),
            this, SLOT(slotDeleteWish()));
    connect(ui->p_list_Wishes, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(slotEditWish(QListWidgetItem *)));
}

void WishesWorkersWidget::loadPositions()
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

            for (auto it : m_positions)
            {
                ui->p_comboBox_Position->addItem(it.Brief);
            }
        }
        else
        {
            //ui->p_line_Surname->setText("unknown");
            //ui->p_line_FirstName->setText("unknown");
            //ui->p_line_LastName->setText("unknown");
        }
    }
}

void WishesWorkersWidget::slotChangedIDWorker(const QString &text)
{
    slotCheckIDWorker();
}

void WishesWorkersWidget::slotChangedPosition(int)
{
    m_wishes[m_currentEditWish] = getInformationFromForm();
    slotModifyFile();
}

void WishesWorkersWidget::slotCheckIDWorker()
{
    if (ui->p_line_ID_worker->text().isEmpty())
    {
        ui->p_button_AddWishes->setEnabled(false);
        ui->p_button_DeleteWishes->setEnabled(false);

        ui->p_line_Surname->setEnabled(false);
        ui->p_line_FirstName->setEnabled(false);
        ui->p_line_LastName->setEnabled(false);

        ui->p_line_Surname->clear();
        ui->p_line_FirstName->clear();
        ui->p_line_LastName->clear();
    }
    else
    {
        ui->p_button_AddWishes->setEnabled(true);
        ui->p_button_DeleteWishes->setEnabled(true);

        ui->p_line_Surname->setEnabled(true);
        ui->p_line_FirstName->setEnabled(true);
        ui->p_line_LastName->setEnabled(true);

        //QFile worker_file;
        if (!m_pathWorkersInformation.isEmpty())
        {
            QString str = m_pathWorkersInformation + ui->p_line_ID_worker->text() + ".xml";
            if (QFile::exists(str))
            {
                typedef Worker (*func_readWorkerInfo)(const QString&);
                QLibrary xml_parser("XML_Parsing");
                func_readWorkerInfo readWorker = (func_readWorkerInfo)xml_parser.resolve("read_WorkerInfoFromFile");

                Worker currentWorker = readWorker(str);

                ui->p_line_Surname->setText(currentWorker.Surname);
                ui->p_line_FirstName->setText(currentWorker.FirstName);
                ui->p_line_LastName->setText(currentWorker.LastName);
            }
            else
            {
                ui->p_line_Surname->setText("unknown");
                ui->p_line_FirstName->setText("unknown");
                ui->p_line_LastName->setText("unknown");
            }
        }
    }

    if (ui->p_line_ID_worker->text().isEmpty() && ui->p_list_Wishes->count() == 0)
        isModifiedFile = false;
}

void WishesWorkersWidget::slotSetPathWorkersInformation(const QString& path)
{
    m_pathWorkersInformation = path;
}

void WishesWorkersWidget::slotSetPathPositionsInformation(const QString& path)
{
    m_pathPositionsInformation = path;
    ui->p_comboBox_Position->clear();
    loadPositions();
}

void WishesWorkersWidget::validateData()
{
    ui->p_line_ID_wishes->setValidator(new QRegExpValidator(QRegExp("^\\d+$|^$")));
    ui->p_line_ID_worker->setValidator(new QRegExpValidator(QRegExp("^\\d+$|^$")));
}

void WishesWorkersWidget::resetData()
{
    m_wishes.clear();
    ui->p_list_Wishes->clear();
    m_currentEditWish.clear();

    ui->p_line_ID_worker->setText("");
    ui->p_line_ID_wishes->setText("");

    QTime temp_time(8,0,0);
    ui->p_dateTimeEdit_StartDate->setDate(QDate::currentDate());
    ui->p_dateTimeEdit_StartDate->setTime(temp_time);

    temp_time.setHMS(22,0,0);
    ui->p_dateTimeEdit_EndDate->setDate(QDate::currentDate());
    ui->p_dateTimeEdit_EndDate->setTime(temp_time);

    ui->p_TextEdit_Addition->clear();
    ui->p_spinBox_MinWorkerDays->setValue(1);
    ui->p_spinBox_MaxWorkerDays->setValue(1);

    ui->p_timeEdit_GetStartedLaterThan->setTime(QTime(8, 0, 0));
    ui->p_dateTimeEdit_EndDate->setTime(QTime(22, 0, 0));

    ui->p_line_Surname->clear();
    ui->p_line_FirstName->clear();
    ui->p_line_LastName->clear();

    //ui->p_line_ID_worker->setEnabled(false);
    ui->p_line_ID_wishes->setEnabled(false);
    ui->p_dateTimeEdit_StartDate->setEnabled(false);
    ui->p_dateTimeEdit_EndDate->setEnabled(false);
    ui->p_TextEdit_Addition->setEnabled(false);
    ui->p_spinBox_MinWorkerDays->setEnabled(false);
    ui->p_spinBox_MaxWorkerDays->setEnabled(false);
    ui->p_timeEdit_GetStartedLaterThan->setEnabled(false);
    ui->p_timeEdit_GetEndEarlierThan->setEnabled(false);
    ui->p_comboBox_Position->setEnabled(false);

}

SingleWish WishesWorkersWidget::getInformationFromForm()
{
    SingleWish info;

    info.ID_Workers = ui->p_line_ID_worker->text().toInt();
    info.ID_Wishes = ui->p_line_ID_wishes->text().toInt();
    info.StartDate = ui->p_dateTimeEdit_StartDate->dateTime().toString();
    info.EndDate = ui->p_dateTimeEdit_EndDate->dateTime().toString();
    info.Addition = ui->p_TextEdit_Addition->toPlainText();
    info.Position = ui->p_comboBox_Position->currentText();
    info.MinWorkerDays = ui->p_spinBox_MinWorkerDays->value();
    info.MaxWorkerDay = ui->p_spinBox_MaxWorkerDays->value();
    info.GetStartedLaterThan = ui->p_timeEdit_GetStartedLaterThan->time().toString();
    info.GetEndEarlierThan = ui->p_timeEdit_GetEndEarlierThan->time().toString();

    return info;
}

void WishesWorkersWidget::setInformationToForm(const SingleWish& info)
{
    ui->p_line_ID_worker->setText(QString::number(info.ID_Workers));
    ui->p_line_ID_wishes->setText(QString::number(info.ID_Wishes));

    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime::fromString(info.StartDate));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime::fromString(info.EndDate));

    ui->p_TextEdit_Addition->setPlainText(info.Addition);
    ui->p_comboBox_Position->setCurrentIndex(ui->p_comboBox_Position->findText(info.Position));
    ui->p_spinBox_MinWorkerDays->setValue(info.MinWorkerDays);
    ui->p_spinBox_MaxWorkerDays->setValue(info.MaxWorkerDay);
    ui->p_timeEdit_GetStartedLaterThan->setTime(QTime::fromString(info.GetStartedLaterThan));
    ui->p_timeEdit_GetEndEarlierThan->setTime(QTime::fromString(info.GetEndEarlierThan));
}

int WishesWorkersWidget::findLastIDInList()
{
    int result = 0;
    for (auto it : m_wishes)
    {
        if (it.ID_Wishes > result)
            result = it.ID_Wishes;
    }

    return result;
}

void WishesWorkersWidget::slotAddWish()
{
    // Пошук останнього ID в списку і створення нового імені для нового ID
    int lastID = findLastIDInList() + 1;
    QString name = QString::number(lastID);

    // Створення нового елемента списку
    QListWidgetItem *item = new QListWidgetItem(name);
    ui->p_list_Wishes->addItem(item);

    // Створення нового елемента словника та заповнення його даними
    SingleWish single;
    single.ID_Wishes = lastID;
    single.ID_Workers = ui->p_line_ID_worker->text().toInt();

    single.StartDate = QDateTime(QDate::currentDate(), QTime(8,0,0)).toString();
    single.EndDate = QDateTime(QDate::currentDate(), QTime(22,0,0)).toString();

    single.MinWorkerDays = 1;
    single.MaxWorkerDay = 1;

    single.GetStartedLaterThan = QTime(8, 0, 0).toString();
    single.GetEndEarlierThan = QTime(22, 0, 0).toString();

    m_wishes.insert(item->text(), single);
    ui->p_line_ID_worker->setEnabled(false);
}

void WishesWorkersWidget::slotDeleteWish()
{
    // Якщо список пустий, то виходимо
    if (ui->p_list_Wishes->selectedItems().isEmpty())
        return;

    // Отримуємо вказівник на елемент
    auto item = ui->p_list_Wishes->selectedItems().first();

    // Видаляємо із словника елемент
    auto iterator_pos = m_wishes.find(item->text());
    m_wishes.erase(iterator_pos);

    // Якщо цей елемент що ми видаляємо зараз редагується, то треба очистити форму і заблокувати її
    if (item->text() == m_currentEditWish)
    {
        m_currentEditWish.clear();

        //ui->p_line_ID_worker->setText("");
        ui->p_line_ID_wishes->setText("");

        QTime temp_time(8,0,0);
        ui->p_dateTimeEdit_StartDate->setDate(QDate::currentDate());
        ui->p_dateTimeEdit_StartDate->setTime(temp_time);

        temp_time.setHMS(22,0,0);
        ui->p_dateTimeEdit_EndDate->setDate(QDate::currentDate());
        ui->p_dateTimeEdit_EndDate->setTime(temp_time);

        ui->p_TextEdit_Addition->clear();
        ui->p_spinBox_MinWorkerDays->setValue(1);
        ui->p_spinBox_MaxWorkerDays->setValue(1);

        ui->p_timeEdit_GetStartedLaterThan->setTime(QTime(8, 0, 0));
        ui->p_dateTimeEdit_EndDate->setTime(QTime(22, 0, 0));

        ui->p_line_Surname->clear();
        ui->p_line_FirstName->clear();
        ui->p_line_LastName->clear();

        //ui->p_line_ID_worker->setEnabled(false);
        ui->p_line_ID_wishes->setEnabled(false);
        ui->p_dateTimeEdit_StartDate->setEnabled(false);
        ui->p_dateTimeEdit_EndDate->setEnabled(false);
        ui->p_TextEdit_Addition->setEnabled(false);
        ui->p_spinBox_MinWorkerDays->setEnabled(false);
        ui->p_spinBox_MaxWorkerDays->setEnabled(false);
        ui->p_timeEdit_GetStartedLaterThan->setEnabled(false);
        ui->p_timeEdit_GetEndEarlierThan->setEnabled(false);
        ui->p_comboBox_Position->setEnabled(false);
    }

    // Вказуємо що файл модифіковано
    slotModifyFile();

    // Видаляємо елемент із списку форми
    delete item;

    // Якщо список пустий, то відновлюємо доступність вводу ID робітника
    if (ui->p_list_Wishes->selectedItems().isEmpty())
        ui->p_line_ID_worker->setEnabled(true);
}

void WishesWorkersWidget::slotModifyFile()
{
    isModifiedFile = true;
}

void WishesWorkersWidget::slotEditWish(QListWidgetItem *item)
{
    // save data from form to memory
    if (m_currentEditWish != "")
    {
        m_wishes[m_currentEditWish] = getInformationFromForm();
    }

    // Load new data
    m_currentEditWish = item->text();

    auto foundItem = m_wishes.end();
    for (auto it = m_wishes.begin(); it != m_wishes.end(); it++)
    {
        if (QString::number(it->ID_Wishes) == item->text())
        {
            foundItem = it;
            break;
        }
    }

    if (foundItem == m_wishes.end())
        return;

    setInformationToForm(*foundItem);

    ui->p_line_ID_wishes->setEnabled(true);
    ui->p_dateTimeEdit_StartDate->setEnabled(true);
    ui->p_dateTimeEdit_EndDate->setEnabled(true);
    ui->p_TextEdit_Addition->setEnabled(true);
    ui->p_spinBox_MinWorkerDays->setEnabled(true);
    ui->p_spinBox_MaxWorkerDays->setEnabled(true);
    ui->p_timeEdit_GetStartedLaterThan->setEnabled(true);
    ui->p_timeEdit_GetEndEarlierThan->setEnabled(true);
    ui->p_comboBox_Position->setEnabled(true);
}

void WishesWorkersWidget::slotUpdateData(const QString& attributeName)
{
    if (attributeName == "ID_Wishes")
    {
        auto iter_currentWish = m_wishes.find(m_currentEditWish);
        if (iter_currentWish == m_wishes.end())
            return;

        QString newID_Wish = ui->p_line_ID_wishes->text();

        // Пошук одинакових ID і якщо знайдено, то присвоїти дефолтний
        if (m_currentEditWish.toInt() != newID_Wish.toInt())
        {
            for (auto it = m_wishes.begin(); it != m_wishes.end(); it++)
            {
                if (newID_Wish.toInt() == it.value().ID_Wishes)
                {
                    newID_Wish = QString::number(findLastIDInList() + 1);
                    ui->p_line_ID_wishes->setText(newID_Wish);
                    break;
                }
            }
        }
        //

        iter_currentWish.value().ID_Wishes = newID_Wish.toInt();

        if (QString::number(iter_currentWish.value().ID_Wishes) == iter_currentWish.key())
        {
            return;
        }
        else
        {
            QListWidgetItem *newItem = new QListWidgetItem(newID_Wish);
            QListWidgetItem *oldItem = ui->p_list_Wishes->currentItem();
            ui->p_list_Wishes->addItem(newItem);
            m_currentEditWish = newItem->text();
            m_wishes.insert(newItem->text(), iter_currentWish.value());

            delete oldItem;
            m_wishes.erase(iter_currentWish);

            ui->p_list_Wishes->setCurrentItem(newItem);
        }

        return;
    }

    //if (attributeName == "ID worker") {}

    if (attributeName == "StartDate" || attributeName == "EndDate")
    {
        if (ui->p_dateTimeEdit_StartDate->date() > ui->p_dateTimeEdit_EndDate->date())
        {
            ui->p_dateTimeEdit_EndDate->setDate(ui->p_dateTimeEdit_StartDate->date());
        }
    }

    if (attributeName == "MinWorkerDays" || attributeName == "MaxWorkerDay")
    {
        if (ui->p_spinBox_MinWorkerDays->value() > ui->p_spinBox_MaxWorkerDays->value())
        {
            ui->p_spinBox_MaxWorkerDays->setValue(ui->p_spinBox_MinWorkerDays->value());
        }
    }

    // save data from form to memory
    if (m_currentEditWish != "")
    {
        m_wishes[m_currentEditWish] = getInformationFromForm();
    }
}

void WishesWorkersWidget::slotCreateNew()
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
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();
            if (!isModifiedFile)
                resetData();
            else
                return;
        }
        if (result == QMessageBox::Cancel)
        {
            return;
        }
    }
    else
    {
        resetData();
    }
}

void WishesWorkersWidget::slotOpen()
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
                                                       "",
                                                       "*.xml");
            openFile(str);
        }
        if (result == QMessageBox::Yes)
        {
            slotSave();

            QString str = QFileDialog::getOpenFileName(this,
                                                       "Open...",
                                                       "",
                                                       "*.xml");
            openFile(str);
        }
    }
    else
    {
        QString str = QFileDialog::getOpenFileName(this,
                                                   "Open...",
                                                   "",
                                                   "*.xml");
        openFile(str);
    }
}

void WishesWorkersWidget::openFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    resetData();
    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef Wishes (*func_readWishes)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readWishes readWishes = (func_readWishes)xmlParser.resolve("read_WishesFromFile");

    m_wishes = readWishes(filename);

    if (!m_wishes.isEmpty())
    {
        ui->p_line_ID_worker->setEnabled(false);
    }

    ui->p_list_Wishes->clear();

    for (auto it : m_wishes)
    {
        ui->p_list_Wishes->addItem(QString::number(it.ID_Wishes));
    }

    isModifiedFile = false;
}

void WishesWorkersWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writeWishesToFile)(const QString&, const Wishes&);
    QLibrary xmlParser("XML_Parsing");
    func_writeWishesToFile writeWishes = (func_writeWishesToFile)xmlParser.resolve("write_WishesToFile");

    writeWishes(m_filename, m_wishes);
    isModifiedFile = false;
}

void WishesWorkersWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", "");
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

void WishesWorkersWidget::closeEvent(QCloseEvent *event)
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

void WishesWorkersWidget::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_layout_navigationButtons->count(); i++)
    {
        ui->p_layout_navigationButtons->itemAt(i)->widget()->setVisible(showState);
    }
}
