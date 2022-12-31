#include "charttimerangedialog.h"
#include "ui_charttimerangedialog.h"

ChartTimeRangeDialog::ChartTimeRangeDialog(const QDateTime& start,
                                           const QDateTime& end,
                                           QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartTimeRangeDialog)
{
    ui->setupUi(this);

    connect(ui->p_dateTimeEdit_StartRange, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(changedDateTime(const QDateTime &)));
    connect(ui->p_dateTimeEdit_EndRange, SIGNAL(dateTimeChanged(const QDateTime &)),
            this, SLOT(changedDateTime(const QDateTime &)));

    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(sendData()));

    ui->p_dateTimeEdit_StartRange->setDateTime(start);
    ui->p_dateTimeEdit_EndRange->setDateTime(end);
}

ChartTimeRangeDialog::~ChartTimeRangeDialog()
{
    delete ui;
}

void ChartTimeRangeDialog::changedDateTime(const QDateTime &)
{
    if (ui->p_dateTimeEdit_StartRange->dateTime() > ui->p_dateTimeEdit_EndRange->dateTime())
        ui->p_dateTimeEdit_EndRange->setDateTime(ui->p_dateTimeEdit_StartRange->dateTime());
}

void ChartTimeRangeDialog::sendData()
{
    /*
    emit sendStartDate(ui->p_dateTimeEdit_StartRange->dateTime());
    emit sendEndDate(ui->p_dateTimeEdit_EndRange->dateTime());
    */

    emit sendRange(ui->p_dateTimeEdit_StartRange->dateTime(), ui->p_dateTimeEdit_EndRange->dateTime());
}

