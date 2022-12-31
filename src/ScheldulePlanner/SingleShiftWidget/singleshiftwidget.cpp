#include "singleshiftwidget.h"
#include "ui_singleshiftwidget.h"
#include <QPushButton>

SingleShiftWidget::SingleShiftWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingleShiftWidget)
{
    ui->setupUi(this);
    ui->p_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);

    ui->p_dateTimeEdit_StartDate->setDate(QDate::currentDate());
    ui->p_dateTimeEdit_EndDate->setDate(QDate::currentDate());

    connect(ui->p_dateTimeEdit_StartDate, SIGNAL(dateTimeChanged(const QDateTime&)),
            this, SLOT(slotCheckDates(const QDateTime&)));
    connect(ui->p_dateTimeEdit_EndDate, SIGNAL(dateTimeChanged(const QDateTime&)),
            this, SLOT(slotCheckDates(const QDateTime&)));

    connect(ui->p_comboBox_Position, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotEnablingOK(int)));
    connect(ui->p_buttonBox, &QDialogButtonBox::accepted,
            [&]()
    {
        emit singnalSendShift(getInformationFromForm());
    });
}

SingleShiftWidget::~SingleShiftWidget()
{
    delete ui;
}

void SingleShiftWidget::setInformationToForm(const SingleShift& info)
{
    ui->p_spinBox_ShiftID->setValue(info.ID_Shifts);
    ui->p_dateTimeEdit_StartDate->setDateTime(QDateTime::fromString(info.StartDate));
    ui->p_dateTimeEdit_EndDate->setDateTime(QDateTime::fromString(info.EndDate));
    //ui->p_comboBox_Position->setCurrentText(info.Position);
    ui->p_comboBox_Position->setCurrentIndex(ui->p_comboBox_Position->findText(info.Position));
}

SingleShift SingleShiftWidget::getInformationFromForm()
{
    SingleShift info;

    info.ID_Shifts = ui->p_spinBox_ShiftID->value();
    info.StartDate = ui->p_dateTimeEdit_StartDate->dateTime().toString();
    info.EndDate = ui->p_dateTimeEdit_EndDate->dateTime().toString();
    info.Position = ui->p_comboBox_Position->currentText();

    return info;
}

void SingleShiftWidget::updatePositionsComboBox()
{
    for (auto it : m_positions)
    {
        ui->p_comboBox_Position->addItem(it.Brief);
    }
}

void SingleShiftWidget::slotCheckDates(const QDateTime&)
{
    if (ui->p_dateTimeEdit_StartDate->dateTime() > ui->p_dateTimeEdit_EndDate->dateTime())
    {
        ui->p_dateTimeEdit_EndDate->setDateTime(ui->p_dateTimeEdit_StartDate->dateTime());
    }
}

void SingleShiftWidget::slotSetPositions(Positions info)
{
    m_positions = info;
    updatePositionsComboBox();
}

void SingleShiftWidget::slotSetSingleShift(SingleShift info)
{
    setInformationToForm(info);
}

void SingleShiftWidget::slotSetShiftID(int id)
{
    ui->p_spinBox_ShiftID->setValue(id);
    ui->p_spinBox_ShiftID->setEnabled(false);
}

void SingleShiftWidget::slotEnablingOK(int index)
{
    if (index != -1)
        ui->p_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
}

void SingleShiftWidget::slotChangeTextOKButton(const QString& text)
{
    ui->p_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText(text);
}
