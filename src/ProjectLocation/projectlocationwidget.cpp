#include "projectlocationwidget.h"
#include "ui_projectlocationwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QLibrary>
#include <QCloseEvent>

ProjectLocationWidget::ProjectLocationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectLocationWidget)
{
    ui->setupUi(this);
    setupWidget();
}

void ProjectLocationWidget::setupWidget()
{
    // Navigation buttons
    connect(ui->p_button_Open, SIGNAL(clicked()),
            this, SLOT(slotOpen()));
    connect(ui->p_button_Save, SIGNAL(clicked()),
            this, SLOT(slotSave()));
    connect(ui->p_button_SaveAs, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));

    // Checking changes into lines
    connect(ui->p_line_PositionsLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));
    connect(ui->p_line_WorkGraphicLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));
    connect(ui->p_line_RequirementsLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));
    connect(ui->p_line_WishesWorkersLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));
    connect(ui->p_line_WorkerGraphicLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));
    connect(ui->p_line_WorkersInformationLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));
    connect(ui->p_line_VacationInformationLocation, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotModifyFile(const QString&)));

    // Getter buttons
    m_sigmapperButtons = new QSignalMapper(this);
    connect(m_sigmapperButtons, SIGNAL(mapped(QWidget*)),
            this, SLOT(slotGetDirPath(QWidget*)));

    connect(ui->p_button_PositionsLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_PositionsLocation, ui->p_line_PositionsLocation);

    connect(ui->p_button_WorkGraphicLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_WorkGraphicLocation, ui->p_line_WorkGraphicLocation);

    connect(ui->p_button_RequirementsLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_RequirementsLocation, ui->p_line_RequirementsLocation);

    connect(ui->p_button_WishesWorkersLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_WishesWorkersLocation, ui->p_line_WishesWorkersLocation);

    connect(ui->p_button_WorkerGraphicLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_WorkerGraphicLocation, ui->p_line_WorkerGraphicLocation);

    connect(ui->p_button_WorkersInformationLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_WorkersInformationLocation, ui->p_line_WorkersInformationLocation);

    connect(ui->p_button_VacationInformationLocation, SIGNAL(clicked()),
            m_sigmapperButtons, SLOT(map()));
    m_sigmapperButtons->setMapping(ui->p_button_VacationInformationLocation, ui->p_line_VacationInformationLocation);
}

void ProjectLocationWidget::validateData() {}

void ProjectLocationWidget::slotGetDirPath(QWidget* wgt)
{
    QString str = QFileDialog::getExistingDirectory(this,
                                                    tr("Choise directory..."),
                                                    "./");
    if (str.isEmpty())
        return;

    QLineEdit *line = dynamic_cast<QLineEdit*>(wgt);
    if (line)
        line->setText(str);
}

void ProjectLocationWidget::slotModifyFile(const QString&)
{
    isModifiedFile = true;
}

ProjectLocations ProjectLocationWidget::getInformationFromForm()
{
    ProjectLocations info;

    info.PositionsLocation = ui->p_line_PositionsLocation->text();
    info.WorkGraphicLocation = ui->p_line_WorkGraphicLocation->text();
    info.RequirementsLocation = ui->p_line_RequirementsLocation->text();
    info.WishesWorkersLocation = ui->p_line_WishesWorkersLocation->text();
    info.WorkerGraphicLocation = ui->p_line_WorkerGraphicLocation->text();
    info.WorkersInformationLocation = ui->p_line_WorkersInformationLocation->text();
    info.VacationInformationLocation = ui->p_line_VacationInformationLocation->text();

    return info;
}

void ProjectLocationWidget::setInformationToForm(const ProjectLocations& info)
{
    ui->p_line_PositionsLocation->setText(info.PositionsLocation);
    ui->p_line_WorkGraphicLocation->setText(info.WorkGraphicLocation);
    ui->p_line_RequirementsLocation->setText(info.RequirementsLocation);
    ui->p_line_WishesWorkersLocation->setText(info.WishesWorkersLocation);
    ui->p_line_WorkerGraphicLocation->setText(info.WorkerGraphicLocation);
    ui->p_line_WorkersInformationLocation->setText(info.WorkersInformationLocation);
    ui->p_line_VacationInformationLocation->setText(info.VacationInformationLocation);
}

ProjectLocationWidget::~ProjectLocationWidget()
{
    delete ui;
}

void ProjectLocationWidget::slotOpen()
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

void ProjectLocationWidget::openFile(const QString & filename)
{
    //resetData();
    if (filename.isEmpty())
        return;

    m_filename = filename;

    // Завантаження даних про робітника із файлу
    typedef ProjectLocations (*func_readProjectLocations)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readProjectLocations readProjectLocations = (func_readProjectLocations)xmlParser.resolve("read_ProjectLocationsFromFile");

    ProjectLocations info = readProjectLocations(filename);

    setInformationToForm(info);
    isModifiedFile = false;
}

void ProjectLocationWidget::slotSave()
{
    if (m_filename.isEmpty())
    {
        slotSaveAs();
        return;
    }

    typedef void (*func_writeProjectLocationsToFile)(const QString&, const ProjectLocations&);
    QLibrary xmlParser("XML_Parsing");
    func_writeProjectLocationsToFile writeProjectLocations = (func_writeProjectLocationsToFile)xmlParser.resolve("write_ProjectLocationsToFile");

    writeProjectLocations(m_filename, getInformationFromForm());
    isModifiedFile = false;
}

void ProjectLocationWidget::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(this, "Save as...", "");
    if (!str.isEmpty())
    {
        m_filename = str;
        slotSave();
    }
}

void ProjectLocationWidget::closeEvent(QCloseEvent *event)
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

void ProjectLocationWidget::slotShowNavigationButtons(bool showState)
{
    for (int i = 0; i < ui->p_layout_navigationButtons->count(); i++)
    {
        ui->p_layout_navigationButtons->itemAt(i)->widget()->setVisible(showState);
    }
}
