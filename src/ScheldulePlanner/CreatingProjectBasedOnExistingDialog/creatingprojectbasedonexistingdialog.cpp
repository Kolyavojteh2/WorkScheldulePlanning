#include "creatingprojectbasedonexistingdialog.h"
#include "ui_creatingprojectbasedonexistingdialog.h"

CreatingProjectBasedOnExistingDialog::CreatingProjectBasedOnExistingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatingProjectBasedOnExistingDialog)
{
    ui->setupUi(this);

    connect(ui->p_checkBox_UseShifts, SIGNAL(stateChanged(int)),
            this, SLOT(enablingCheckBoxes()));
    connect(ui->p_checkBox_UseWishes, SIGNAL(stateChanged(int)),
            this, SLOT(enablingCheckBoxes()));
    connect(ui->p_checkBox_UseVacations, SIGNAL(stateChanged(int)),
            this, SLOT(enablingCheckBoxes()));
    connect(ui->p_checkBox_UseRequirements, SIGNAL(stateChanged(int)),
            this, SLOT(enablingCheckBoxes()));

    connect(ui->p_dateTimeEdit_StartPeriod, SIGNAL(dateTimeChanged(const QDateTime&)),
            this, SLOT(changeDateTime()));
    connect(ui->p_line_ProjectName, SIGNAL(editingFinished()),
            this, SLOT(checkExistingProject()));

    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(prepareConfiguration()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SIGNAL(rejected()));
}

CreatingProjectBasedOnExistingDialog::~CreatingProjectBasedOnExistingDialog()
{
    delete ui;
}

void CreatingProjectBasedOnExistingDialog::enablingCheckBoxes()
{
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(sender());
    if (!checkBox)
        return;

    if (checkBox == ui->p_checkBox_UseRequirements)
    {
        ui->p_checkBox_AdaptingRequirements->setEnabled(checkBox->isChecked());
        return;
    }

    if (checkBox == ui->p_checkBox_UseShifts)
    {
        ui->p_checkBox_AdaptingShifts->setEnabled(checkBox->isChecked());
        return;
    }

    if (checkBox == ui->p_checkBox_UseWishes)
    {
        ui->p_checkBox_AdaptingWishes->setEnabled(checkBox->isChecked());
        return;
    }

    if (checkBox == ui->p_checkBox_UseVacations)
    {
        ui->p_checkBox_AdaptingVacations->setEnabled(checkBox->isChecked());
        return;
    }
}

void CreatingProjectBasedOnExistingDialog::setPeriod(const QDateTime& start, const QDateTime& end)
{
    ui->p_dateTimeEdit_StartPeriod->setDateTime(start);
    ui->p_dateTimeEdit_EndPeriod->setDateTime(end);

    duration = ui->p_dateTimeEdit_StartPeriod->dateTime().secsTo(ui->p_dateTimeEdit_EndPeriod->dateTime());
}

void CreatingProjectBasedOnExistingDialog::prepareConfiguration()
{
    ConfigurationCreationProjectBasedExisting conf;

    conf.startPeriod = ui->p_dateTimeEdit_StartPeriod->dateTime();
    conf.endPeriod = ui->p_dateTimeEdit_EndPeriod->dateTime();

    conf.shifts = ui->p_checkBox_UseShifts->isChecked();
    conf.wishes = ui->p_checkBox_UseWishes->isChecked();
    conf.workers = ui->p_checkBox_UseWorkers->isChecked();
    conf.assigned = ui->p_checkBox_UseAssigned->isChecked();
    conf.positions = ui->p_checkBox_UsePositions->isChecked();
    conf.vacations = ui->p_checkBox_UseVacations->isChecked();
    conf.requirements = ui->p_checkBox_UseRequirements->isChecked();

    conf.adaptingShifts = ui->p_checkBox_AdaptingShifts->isChecked();
    conf.adaptingWishes = ui->p_checkBox_AdaptingWishes->isChecked();
    conf.adaptingVacations = ui->p_checkBox_AdaptingVacations->isChecked();
    conf.adaptingRequirements = ui->p_checkBox_AdaptingRequirements->isChecked();

    conf.projectName = ui->p_line_ProjectName->text();

    emit definedConfiguration(conf);
}

void CreatingProjectBasedOnExistingDialog::changeDateTime()
{
    ui->p_dateTimeEdit_EndPeriod->setDateTime(QDateTime(ui->p_dateTimeEdit_StartPeriod->dateTime().addSecs(duration)));
}

void CreatingProjectBasedOnExistingDialog::checkExistingProject()
{
    QDir dir(m_defaultProjectsPath + ui->p_line_ProjectName->text());
    if (ui->p_line_ProjectName->text().isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
        return;
    }

    if (dir.exists())
    {
        ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The project with this name already exists.\nEnter another name."));
        return;
    }

    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
}

void CreatingProjectBasedOnExistingDialog::setDefaultProjectPath(const QString& path)
{
    m_defaultProjectsPath = path;
}
