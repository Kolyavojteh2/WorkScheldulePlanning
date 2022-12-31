#include "projectsetupwidget.h"
#include "ui_projectsetupwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QLibrary>
#include <QCloseEvent>

ProjectSetupWidget::ProjectSetupWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProjectSetupWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void ProjectSetupWidget::setupWidget()
{
    // Navigation buttons
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));

    // Changing rest time
    connect(ui->p_timeEdit_MinRestTime, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotTimeModified(const QTime&)));
    connect(ui->p_timeEdit_StandartRestTime, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotTimeModified(const QTime&)));

    // Set default time
    ui->p_dateTimeEdit_StartPeriod->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0, 0)));
    ui->p_dateTimeEdit_EndPeriod->setDateTime(QDateTime(QDate::currentDate().addDays(7), QTime(0, 0, 0)));

    // Changing period
    connect(ui->p_dateTimeEdit_StartPeriod, SIGNAL(dateTimeChanged(const QDateTime&)),
            this, SLOT(slotDateTimeModified(const QDateTime&)));
    connect(ui->p_dateTimeEdit_EndPeriod, SIGNAL(dateTimeChanged(const QDateTime&)),
            this, SLOT(slotDateTimeModified(const QDateTime&)));
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
    info.MaxHoursPerPeriod = ui->p_spinBox_MaxHoursPerPeriod->value();

    info.StartPeriod = ui->p_dateTimeEdit_StartPeriod->dateTime().toString();
    info.EndPeriod = ui->p_dateTimeEdit_EndPeriod->dateTime().toString();

    return info;
}

void ProjectSetupWidget::setInformationToForm(const ProjectSetup& info)
{
    ui->p_timeEdit_MinRestTime->setTime(QTime().fromString(info.MinRestTime));
    ui->p_timeEdit_StandartRestTime->setTime(QTime().fromString(info.StandartRestTime));
    ui->p_spinBox_MaxHoursPerPeriod->setValue(info.MaxHoursPerPeriod);

    ui->p_dateTimeEdit_StartPeriod->setDateTime(QDateTime::fromString(info.StartPeriod));
    ui->p_dateTimeEdit_EndPeriod->setDateTime(QDateTime::fromString(info.EndPeriod));
}

void ProjectSetupWidget::slotTimeModified(const QTime&)
{
    if (ui->p_timeEdit_MinRestTime->time() > ui->p_timeEdit_StandartRestTime->time())
        ui->p_timeEdit_StandartRestTime->setTime(ui->p_timeEdit_MinRestTime->time());
}

void ProjectSetupWidget::slotDateTimeModified(const QDateTime&)
{
    if (ui->p_dateTimeEdit_StartPeriod->dateTime() > ui->p_dateTimeEdit_EndPeriod->dateTime())
        ui->p_dateTimeEdit_EndPeriod->setDateTime(ui->p_dateTimeEdit_StartPeriod->dateTime());
}

void ProjectSetupWidget::slotSave()
{
    isSaved = true;
    emit signalSendConfiguration(getInformationFromForm());
}

void ProjectSetupWidget::closeEvent(QCloseEvent *event)
{
    if (!isSaved)
    {
        int result = QMessageBox::warning(this,
                                      tr("No changes have been saved to the file"),
                                      tr("Do you want to save the changes to the file?"),
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

void ProjectSetupWidget::slotSetConfiguration(ProjectSetup conf)
{
    setInformationToForm(conf);
}
