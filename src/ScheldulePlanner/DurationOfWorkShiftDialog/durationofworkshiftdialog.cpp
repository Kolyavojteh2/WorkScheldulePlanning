#include "durationofworkshiftdialog.h"
#include "ui_durationofworkshiftdialog.h"

DurationOfWorkShiftDialog::DurationOfWorkShiftDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DurationOfWorkShiftDialog)
{
    ui->setupUi(this);

    connect(ui->p_timeEdit_Minimum, SIGNAL(timeChanged(const QTime &)),
            this, SLOT(checkTimes()));
    connect(ui->p_timeEdit_Standart, SIGNAL(timeChanged(const QTime &)),
            this, SLOT(checkTimes()));
    connect(ui->p_timeEdit_Maximum, SIGNAL(timeChanged(const QTime &)),
            this, SLOT(checkTimes()));

    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(prepareBeforeSendData()));
}

DurationOfWorkShiftDialog::~DurationOfWorkShiftDialog()
{
    delete ui;
}

void DurationOfWorkShiftDialog::checkTimes()
{
    if (ui->p_timeEdit_Minimum->time() <= ui->p_timeEdit_Standart->time() &&
            ui->p_timeEdit_Standart->time() <= ui->p_timeEdit_Maximum->time())
        return;

    if (!(ui->p_timeEdit_Minimum->time() <= ui->p_timeEdit_Standart->time()))
        ui->p_timeEdit_Standart->setTime(ui->p_timeEdit_Minimum->time());

    if (!(ui->p_timeEdit_Standart->time() <= ui->p_timeEdit_Maximum->time()))
        ui->p_timeEdit_Maximum->setTime(ui->p_timeEdit_Standart->time());
}

void DurationOfWorkShiftDialog::prepareBeforeSendData()
{
    DurationsOfWorkShift info;
    info.Minimum = ui->p_timeEdit_Minimum->time();
    info.Standart = ui->p_timeEdit_Standart->time();
    info.Maximum = ui->p_timeEdit_Maximum->time();

    emit EnteredDurations(info);
}
