#include "projectsetupwidget.h"
#include "ui_projectsetupwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QLibrary>
#include <QCloseEvent>

ProjectSetupWidget::ProjectSetupWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectSetupWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void ProjectSetupWidget::validateData() {}

void ProjectSetupWidget::setupWidget()
{
    // Navigation buttons
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));
    // Modyfing file
    connect(ui->p_checkBox_RoundTheClockMode, SIGNAL(stateChanged(int)),
            this, SLOT(slotSpinBoxOrCheckBoxModified(int)));
    connect(ui->p_checkBox_EveryMounthGraphic, SIGNAL(stateChanged(int)),
            this, SLOT(slotSpinBoxOrCheckBoxModified(int)));
    connect(ui->p_spinBox_GraphicForeverDay, SIGNAL(valueChanged(int)),
            this, SLOT(slotSpinBoxOrCheckBoxModified(int)));
    connect(ui->p_timeEdit_OpeningCorporation, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotTimeModified(const QTime&)));
    connect(ui->p_timeEdit_ClosingCorporation, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotTimeModified(const QTime&)));
    connect(ui->p_timeEdit_MinRestTime, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotTimeModified(const QTime&)));
    connect(ui->p_timeEdit_StandartRestTime, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotTimeModified(const QTime&)));

    // Enabling a time widgets
    connect(ui->p_checkBox_RoundTheClockMode, SIGNAL(stateChanged(int)),
            this, SLOT(slotRoundTheClockModeChanged(int)));
    connect(ui->p_checkBox_EveryMounthGraphic, SIGNAL(stateChanged(int)),
            this, SLOT(slotEveryMounthGraphicChanged(int)));

    validateData();
}

void ProjectSetupWidget::slotRoundTheClockModeChanged(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        ui->p_timeEdit_OpeningCorporation->setEnabled(false);
        ui->p_timeEdit_ClosingCorporation->setEnabled(false);
    }
    else
    {
        ui->p_timeEdit_OpeningCorporation->setEnabled(true);
        ui->p_timeEdit_ClosingCorporation->setEnabled(true);
    }
}

void ProjectSetupWidget::slotEveryMounthGraphicChanged(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        ui->p_spinBox_GraphicForeverDay->setEnabled(false);
        ui->p_spinBox_GraphicForeverDay->setValue(QDate::currentDate().daysInMonth());
    }
    else
    {
        ui->p_spinBox_GraphicForeverDay->setEnabled(true);
    }
}

ProjectSetupWidget::~ProjectSetupWidget()
{
    delete ui;
}

ProjectSetup ProjectSetupWidget::getInformationFromForm()
{
    ProjectSetup info;

    info.MinRestTime = ui->p_timeEdit_MinRestTime->time().toString();
    info.StandartRestTime = ui->p_timeEdit_StandartRestTime->time().toString();
    info.GraphicForeverDay = ui->p_spinBox_GraphicForeverDay->value();
    info.RoundTheClockMode = ui->p_checkBox_RoundTheClockMode->checkState();
    info.ClosingCorporation = ui->p_timeEdit_ClosingCorporation->time().toString();
    info.EveryMounthGraphic = ui->p_checkBox_EveryMounthGraphic->checkState();
    info.OpeningCorporation = ui->p_timeEdit_OpeningCorporation->time().toString();

    return info;
}

void ProjectSetupWidget::setInformationToForm(const ProjectSetup& info)
{
    ui->p_timeEdit_MinRestTime->setTime(QTime().fromString(info.MinRestTime));
    ui->p_timeEdit_StandartRestTime->setTime(QTime().fromString(info.StandartRestTime));
    ui->p_spinBox_GraphicForeverDay->setValue(info.GraphicForeverDay);
    ui->p_checkBox_RoundTheClockMode->setCheckState(static_cast<Qt::CheckState>(info.RoundTheClockMode));
    ui->p_timeEdit_ClosingCorporation->setTime(QTime().fromString(info.ClosingCorporation));
    ui->p_checkBox_EveryMounthGraphic->setCheckState(static_cast<Qt::CheckState>(info.EveryMounthGraphic));
    ui->p_timeEdit_OpeningCorporation->setTime(QTime().fromString(info.OpeningCorporation));

    ui->p_checkBox_RoundTheClockMode->setTristate(false);
    ui->p_checkBox_EveryMounthGraphic->setTristate(false);
}

void ProjectSetupWidget::slotModifyFile()
{
    isModifiedFile = true;
}

void ProjectSetupWidget::slotTimeModified(const QTime&)
{
    if (ui->p_timeEdit_MinRestTime->time() > ui->p_timeEdit_StandartRestTime->time())
        ui->p_timeEdit_StandartRestTime->setTime(ui->p_timeEdit_MinRestTime->time());

    slotModifyFile();
}

void ProjectSetupWidget::slotSpinBoxOrCheckBoxModified(int)
{
    slotModifyFile();
}

void ProjectSetupWidget::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_buttons_layout->count(); i++)
    {
        ui->p_buttons_layout->itemAt(i)->widget()->setVisible(showState);
    }
}

void ProjectSetupWidget::slotOpen()
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

void ProjectSetupWidget::openFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef ProjectSetup (*func_readProjectSetup)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readProjectSetup readProjectSetup = (func_readProjectSetup)xmlParser.resolve("read_ProjectSetupFromFile");

    ProjectSetup info = readProjectSetup(filename);

    setInformationToForm(info);
    isModifiedFile = false;
}

void ProjectSetupWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writeProjectSetupToFile)(const QString&, const ProjectSetup&);
    QLibrary xmlParser("XML_Parsing");
    func_writeProjectSetupToFile writeProjectSetup = (func_writeProjectSetupToFile)xmlParser.resolve("write_ProjectSetupToFile");

    writeProjectSetup(m_filename, getInformationFromForm());
    isModifiedFile = false;
}

void ProjectSetupWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", "");
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

void ProjectSetupWidget::closeEvent(QCloseEvent *event)
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
