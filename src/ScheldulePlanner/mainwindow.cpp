#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectdataloader.h"
#include "projectdatasaver.h"

#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QMessageBox>
#include <QInputDialog>

#include <algorithm>

#include "./WorkerInformation/workersinformation.h"
#include "./PositionsManager/positionswidget.h"
#include "./ProjectSetupWidget/projectsetupwidget.h"
#include "./RequirementsManager/requirement.h"
#include "./VacationsManager/vacationwidget.h"
#include "./WishesManager/wishesworkerswidget.h"
#include "./SingleShiftWidget/singleshiftwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupWidget();
}

void MainWindow::setupWidget()
{
// Tree widget
    this->setWindowTitle(tr("Work scheldule planner"));
    QResource::registerResource("./Icon.qrc");

    this->setWindowIcon(QIcon(":/MainWindowIcon/Resources/icon.png"));

    ui->p_treeWidget_ProjectTree->setColumnCount(2);
    ui->p_treeWidget_ProjectTree->header()->hide();
    ui->p_treeWidget_ProjectTree->hideColumn(1);

    ui->p_treeWidget_Shifts->setColumnCount(2);
    ui->p_treeWidget_Shifts->header()->hide();
    ui->p_treeWidget_Shifts->hideColumn(1);

    connect(ui->p_treeWidget_ProjectTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotEditData(QTreeWidgetItem *, int)));
    connect(ui->p_treeWidget_Shifts, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotEditData(QTreeWidgetItem *, int)));

// Menu
    p_menuBar = new QMenuBar;

// Вид
    p_menuView = new QMenu(tr("&View"));

    // Бокова панель дерева проекту
    QAction* p_visibleProjectTreeCheckBox = new QAction(tr("Project tree"));
    p_visibleProjectTreeCheckBox->setCheckable(true);
    p_visibleProjectTreeCheckBox->setChecked(true);

    // Взаємозалежність видимості дерева від Action в меню.
    connect(p_visibleProjectTreeCheckBox, SIGNAL(toggled(bool)),
            ui->p_dockWidget_ProjectTree, SLOT(setVisible(bool)));
    connect(ui->p_dockWidget_ProjectTree, SIGNAL(visibilityChanged(bool)),
            p_visibleProjectTreeCheckBox, SLOT(setChecked(bool)));
    p_menuView->addAction(p_visibleProjectTreeCheckBox);

    // Бокова панель робочих змін
    QAction* p_visibleShiftsDockCheckBox = new QAction(tr("Shifts"));
    p_visibleShiftsDockCheckBox->setCheckable(true);
    p_visibleShiftsDockCheckBox->setChecked(true);

    // Взаємозалежність видимості дерева від Action в меню.
    connect(p_visibleShiftsDockCheckBox, SIGNAL(toggled(bool)),
            ui->p_dockWidget_Shifts, SLOT(setVisible(bool)));
    connect(ui->p_dockWidget_Shifts, SIGNAL(visibilityChanged(bool)),
            p_visibleShiftsDockCheckBox, SLOT(setChecked(bool)));
    p_menuView->addAction(p_visibleShiftsDockCheckBox);

    // Вибір центрального робочого простору
    p_menuCentralWorkspace = new QMenu(tr("Central workspace"));

    // Відкрити графік вимог
    p_showChartRequirements = new QAction(tr("Chart requirements"));
    p_showChartRequirements->setCheckable(true);
    connect(p_showChartRequirements, SIGNAL(triggered()),
            this, SLOT(slotChartRequirements()));
    connect(p_showChartRequirements, SIGNAL(triggered()),
            this, SLOT(slotChangeCentralWorkspace()));
    p_menuCentralWorkspace->addAction(p_showChartRequirements);

    // Відкрити графік робочих змін
    p_showChartShifts = new QAction(tr("Chart shifts"));
    p_showChartShifts->setCheckable(true);
    connect(p_showChartShifts, SIGNAL(triggered()),
            this, SLOT(slotChartShifts()));
    connect(p_showChartShifts, SIGNAL(triggered()),
            this, SLOT(slotChangeCentralWorkspace()));
    p_menuCentralWorkspace->addAction(p_showChartShifts);

    // Відкрити графік різниці між вимогами та робочими змінами
    p_showChartDiffs = new QAction(tr("Chart diffs between requirements and shifts"));
    p_showChartDiffs->setCheckable(true);
    connect(p_showChartDiffs, SIGNAL(triggered()),
            this, SLOT(slotChartDiffs()));
    connect(p_showChartDiffs, SIGNAL(triggered()),
            this, SLOT(slotChangeCentralWorkspace()));
    p_menuCentralWorkspace->addAction(p_showChartDiffs);

    // Відкрити графіки проекту
    p_showCharts = new QAction(tr("Projects charts"));
    p_showCharts->setCheckable(true);
    connect(p_showCharts, SIGNAL(triggered()),
            this, SLOT(slotChartsWidget()));
    connect(p_showCharts, SIGNAL(triggered()),
            this, SLOT(slotChangeCentralWorkspace()));
    p_menuCentralWorkspace->addAction(p_showCharts);

// Проект
    p_menuProject = new QMenu(tr("&Project"));

    // Створення нового проекту
    QAction* p_CreateNewProject = new QAction(tr("New project..."));
    connect(p_CreateNewProject, SIGNAL(triggered()),
            this, SLOT(slotCreateNewProject()));
    p_menuProject->addAction(p_CreateNewProject);

    // Створення нового проекту на основі існуючого =
    QAction* p_CreateNewProjectBasedExisting = new QAction(tr("New project based on existing one..."));
    connect(p_CreateNewProjectBasedExisting, SIGNAL(triggered()),
            this, SLOT(slotCreateNewProjectBasedOnExisting()));
    p_menuProject->addAction(p_CreateNewProjectBasedExisting);

    // Сепаратор
    p_menuProject->addSeparator();

    // Відкриття існуючого проекту
    QAction* p_OpenProject = new QAction(tr("Open project..."));
    connect(p_OpenProject, SIGNAL(triggered()),
            this, SLOT(slotOpenProject()));
    p_menuProject->addAction(p_OpenProject);

    // Сепаратор
    p_menuProject->addSeparator();

    // Збереження проекту
    QAction* p_SaveProject = new QAction(tr("Save project"));
    connect(p_SaveProject, SIGNAL(triggered()),
            this, SLOT(slotSaveProject()));
    p_menuProject->addAction(p_SaveProject);

    // Збереження проекту як...
    QAction* p_SaveAsProject = new QAction(tr("Save project as..."));
    connect(p_SaveAsProject, SIGNAL(triggered()),
            this, SLOT(slotSaveAsProject()));
    p_menuProject->addAction(p_SaveAsProject);

    // Сепаратор
    p_menuProject->addSeparator();

    // Закриття проекту
    QAction* p_CloseProject = new QAction(tr("Close project"));
    connect(p_CloseProject, SIGNAL(triggered()),
            this, SLOT(slotCloseProject()));
    p_menuProject->addAction(p_CloseProject);

    // Сепаратор
    p_menuProject->addSeparator();

    // Експорт
    p_menuExport = new QMenu(tr("&Export"));
    p_menuProject->addMenu(p_menuExport);

    // Експорт змін за позиціями
    QAction* p_exportByPosition = new QAction(tr("Workshifts by positions..."));
    connect(p_exportByPosition, SIGNAL(triggered()),
            this, SLOT(slotExportByPositions()));
    p_menuExport->addAction(p_exportByPosition);

    // Експорт змін за позиціями
    QAction* p_exportByWorkers = new QAction(tr("Workshifts by workers..."));
    connect(p_exportByWorkers, SIGNAL(triggered()),
            this, SLOT(slotExportByWorkers()));
    p_menuExport->addAction(p_exportByWorkers);

    // Експорт змін за часом
    QAction* p_exportByTime = new QAction(tr("Workshifts by time..."));
    connect(p_exportByTime, SIGNAL(triggered()),
            this, SLOT(slotExportByTime()));
    p_menuExport->addAction(p_exportByTime);

    // Сепаратор
    p_menuProject->addSeparator();

    // Закриття проекту
    QAction* p_ExitProgram = new QAction(tr("Exit"));
    connect(p_ExitProgram, SIGNAL(triggered()),
            this, SLOT(close()));
    p_menuProject->addAction(p_ExitProgram);

// Робітник
    p_menuWorker = new QMenu(tr("&Worker"));

    // Створення нового робітника
    QAction* p_CreateNewWorker = new QAction(tr("New worker..."));
    connect(p_CreateNewWorker, SIGNAL(triggered()),
            this, SLOT(slotCreateNewWorker()));
    p_menuWorker->addAction(p_CreateNewWorker);

    // Редагування робітника
    QAction* p_EditWorker = new QAction(tr("Edit worker..."));
    connect(p_EditWorker, &QAction::triggered,
            [&]()
    {
        bool m_ok;
        int id = QInputDialog::getInt(this,
                                      tr("Editing of worker"),
                                      tr("Enter the ID of worker:"),
                                      1,
                                      1,
                                      65535,
                                      1,
                                      &m_ok);
        if (!m_ok)
            return;

        slotEditWorker(id);
    });
    p_menuWorker->addAction(p_EditWorker);

    // Видалення існуючого робітника
    QAction* p_DeleteWorker = new QAction(tr("Remove worker..."));
    connect(p_DeleteWorker, SIGNAL(triggered()),
            this, SLOT(slotDeleteWorker()));
    p_menuWorker->addAction(p_DeleteWorker);

    // Пошук робітників
    QAction* p_findWorker = new QAction(tr("Find worker..."));
    connect(p_findWorker, SIGNAL(triggered()),
            this, SLOT(slotFindWorker()));
    p_menuWorker->addAction(p_findWorker);

// Менеджер позицій
    QAction* p_PositionManager = new QAction(tr("Position manager..."));
    connect(p_PositionManager, SIGNAL(triggered()),
            this, SLOT(slotOpenPositionManager()));

// Вимоги
    p_menuRequirements = new QMenu(tr("&Requirements"));
    // Менеджер вимог
    QAction* p_RequirementsManager = new QAction(tr("Requirements manager..."));
    connect(p_RequirementsManager, SIGNAL(triggered()),
            this, SLOT(slotOpenRequirementsManager()));
    p_menuRequirements->addAction(p_RequirementsManager);

    // Знайти вимоги
    QAction* p_FindRequirements = new QAction(tr("Find requirements..."));
    connect(p_FindRequirements, SIGNAL(triggered()),
            this, SLOT(slotFindRequrements()));
    p_menuRequirements->addAction(p_FindRequirements);

    // Видалити всі вимоги
    QAction* p_RemoveAllRequirements = new QAction(tr("Remove all requirements..."));
    connect(p_RemoveAllRequirements, SIGNAL(triggered()),
            this, SLOT(slotRemoveAllRequirements()));
    p_menuRequirements->addAction(p_RemoveAllRequirements);

// Відпустки
    p_menuVacations = new QMenu(tr("Vacations"));
    QAction* p_VacationsManager = new QAction(tr("Vacations manager..."));
    connect(p_VacationsManager, SIGNAL(triggered()),
            this, SLOT(slotOpenVacationsManager()));
    p_menuVacations->addAction(p_VacationsManager);

    QAction* p_FindVacations = new QAction(tr("Find vacations..."));
    connect(p_FindVacations, SIGNAL(triggered()),
            this, SLOT(slotFindVacations()));
    p_menuVacations->addAction(p_FindVacations);

    QAction* p_RemoveAllVacations = new QAction(tr("Remove all vacations..."));
    connect(p_RemoveAllVacations, SIGNAL(triggered()),
            this, SLOT(slotRemoveAllVacations()));
    p_menuVacations->addAction(p_RemoveAllVacations);

// Побажання робітника
    p_menuWishes = new QMenu(tr("Wishes"));
    // Менеджер побажань
    QAction* p_WishesManager = new QAction(tr("Wish manager for worker..."));
    connect(p_WishesManager, SIGNAL(triggered()),
        this, SLOT(slotOpenWishesManager()));
    p_menuWishes->addAction(p_WishesManager);

    // Знайти побажання
    QAction* p_FindWishes = new QAction(tr("Find wishes..."));
    connect(p_FindWishes, SIGNAL(triggered()),
        this, SLOT(slotFindWishes()));
    p_menuWishes->addAction(p_FindWishes);

    // Видалення всіх побажань
    QAction* p_RemoveAllWishes = new QAction(tr("Remove all wishes..."));
    connect(p_RemoveAllWishes, SIGNAL(triggered()),
        this, SLOT(slotRemoveAllWishes()));
    p_menuWishes->addAction(p_RemoveAllWishes);

// Налаштування проекту
    QAction* p_ProjectConfiguration = new QAction(tr("Project configurations..."));
    connect(p_ProjectConfiguration, SIGNAL(triggered()),
            this, SLOT(slotOpenProjectConfiguration()));

// Планування графіку
    // Менеджер змін
    p_menuPlaning = new QMenu(tr("Planing"));
// Зміни
    // Додати нову зміну
    QAction* p_AddShift = new QAction(tr("Add shift..."));
    connect(p_AddShift, SIGNAL(triggered()),
            this, SLOT(slotAddSingleShift()));
    p_menuPlaning->addAction(p_AddShift);

    // Видалити зміну за номером ID
    QAction* p_DeleteShift = new QAction(tr("Delete shift..."));
    connect(p_DeleteShift, SIGNAL(triggered()),
            this, SLOT(slotDeleteSingleShift()));
    p_menuPlaning->addAction(p_DeleteShift);

    // Знайти зміни
    QAction* p_FindShifts = new QAction(tr("Find shifts..."));
    connect(p_FindShifts, SIGNAL(triggered()),
            this, SLOT(slotFindShifts()));
    p_menuPlaning->addAction(p_FindShifts);

    // Очистити зміни
    QAction* p_RemoveAllShiftsAndAssignments = new QAction(tr("Remove all work shifts and all assignments..."));
    connect(p_RemoveAllShiftsAndAssignments, SIGNAL(triggered()),
            this, SLOT(slotRemoveAllShiftsAndAssignments()));
    p_menuPlaning->addAction(p_RemoveAllShiftsAndAssignments);

// Сепаратор
    p_menuPlaning->addSeparator();

// Зв'язані зміни
    // Зв'язати зміну із працівником
    QAction* p_AssignShift = new QAction(tr("Assign shift..."));
    connect(p_AssignShift, SIGNAL(triggered()),
            this, SLOT(slotAssignShift()));
    p_menuPlaning->addAction(p_AssignShift);

    // Зв'язати зміну із працівником без перевірки на норми
    QAction* p_AssignOverTheNormWorkShift = new QAction(tr("Assign work shift without checks..."));
    connect(p_AssignOverTheNormWorkShift, SIGNAL(triggered()),
            this, SLOT(slotAssignOverTheNormWorkShift()));
    p_menuPlaning->addAction(p_AssignOverTheNormWorkShift);

    // Відмінити зв'язування зміни із працівником
    QAction* p_cancelAssignmentShift = new QAction(tr("Cancel assignment..."));
    connect(p_cancelAssignmentShift, SIGNAL(triggered()),
            this, SLOT(slotCancelAssignmentShift()));
    p_menuPlaning->addAction(p_cancelAssignmentShift);

    // Знайти зв'язані зміни
    QAction* p_FindAssignmentShifts = new QAction(tr("Find assigned shifts..."));
    connect(p_FindAssignmentShifts, SIGNAL(triggered()),
            this, SLOT(slotFindAssignedShifts()));
    p_menuPlaning->addAction(p_FindAssignmentShifts);

    // Видалити всі зв'язки робочих змін
    QAction* p_RemoveAllAssignmentShifts = new QAction(tr("Remove all assigned shifts..."));
    connect(p_RemoveAllAssignmentShifts, SIGNAL(triggered()),
            this, SLOT(slotRemoveAllAssignedShifts()));
    p_menuPlaning->addAction(p_RemoveAllAssignmentShifts);

// Сепаратор
    p_menuPlaning->addSeparator();

// Автоматичні алгоритми
    // Min - Standart
    QAction* p_MinStandart = new QAction(tr("Algorithm min-standart..."));
    connect(p_MinStandart, SIGNAL(triggered()),
            this, SLOT(slotMinStandart()));
    p_menuPlaning->addAction(p_MinStandart);

    // Min - Max
    QAction* p_MinMax = new QAction(tr("Algorithm min-max..."));
    connect(p_MinMax, SIGNAL(triggered()),
            this, SLOT(slotMinMax()));
    p_menuPlaning->addAction(p_MinMax);

    // Assignment by vulnarable shift
    QAction* p_AssignByVulnerableShift = new QAction(tr("Algorithm assignment by vulnerable shift..."));
    connect(p_AssignByVulnerableShift, SIGNAL(triggered()),
            this, SLOT(slotAssignByVulnerableShift()));
    p_menuPlaning->addAction(p_AssignByVulnerableShift);

    // Assignment by vulnarable shift without wishes
    QAction* p_AssignByVulnerableShiftWithoutWishes = new QAction(tr("Algorithm assignment by vulnerable shift without wishes..."));
    connect(p_AssignByVulnerableShiftWithoutWishes, SIGNAL(triggered()),
            this, SLOT(slotAssignByVulnerableShiftWithoutWishes()));
    p_menuPlaning->addAction(p_AssignByVulnerableShiftWithoutWishes);

// Присвоєння створених меню до менюбара
    p_menuView->addSeparator();
    p_menuView->addMenu(p_menuCentralWorkspace);

    p_menuData = new QMenu(tr("&Data"));
    p_menuData->addAction(p_PositionManager);
    p_menuData->addMenu(p_menuWorker);
    p_menuData->addMenu(p_menuRequirements);
    p_menuData->addMenu(p_menuVacations);
    p_menuData->addMenu(p_menuWishes);
    p_menuData->addAction(p_ProjectConfiguration);

    p_menuBar->addMenu(p_menuProject);
    p_menuBar->addMenu(p_menuView);
    p_menuBar->addMenu(p_menuData);
    p_menuBar->addMenu(p_menuPlaning);

    this->setMenuBar(p_menuBar);

// Оновлення дерева проекту
    connect(ui->p_button_UpdateProject, SIGNAL(clicked()),
            this, SLOT(slotUpdateProjectTree()));

    slotUpdateEnablingOfActions(false);
    slotOpenStartWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearProjectData()
{
    m_projectPositions.clear();
    m_projectSetup = ProjectSetup();
    m_projectRequirements.clear();
    m_projectVacations.clear();
    m_projectWishes.clear();
    m_projectWorkers.clear();
    m_projectShifts.clear();
    m_projectAssigned.clear();

    slotUpdateProjectTree();

    isModifiedProject = false;
}

void MainWindow::slotCreateNewProject()
{
    if (!askToSaveChanges())
        return;

    bool clickedOK = false;
    QString projectName = QInputDialog::getText(this,
                                                tr("Project name"),
                                                tr("Enter a project name:"),
                                                QLineEdit::EchoMode::Normal,
                                                "",
                                                &clickedOK);
    if (!clickedOK)
        return;

    m_projectPath = m_defaultProjectPath + projectName;
    m_projectPath = QDir(m_projectPath).absolutePath() + "/";
    updateDirectoryPath();

    clearProjectData();

    slotOpenProjectConfiguration();
    slotUpdateProjectTree();
    slotShowProjectsKit();

    slotUpdateEnablingOfActions(true);
}

QString MainWindow::projectPath()
{
    return m_projectPath;
}

void MainWindow::slotSetProjectPath(const QString& path)
{
    m_projectPath = path;
}

QMessageBox::StandardButton MainWindow::checkChangesInCurrentProject()
{
    if (isModifiedProject)
    {
        QMessageBox::StandardButton result = QMessageBox::warning(this,
                                          tr("No changes have been saved to the file"),
                                          tr("Do you want to save the changes to the file"),
                                          QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);
        return result;
    }
    else
    {
        return QMessageBox::No;
    }
}

void MainWindow::updateDirectoryPath()
{
    m_workersPath           = m_projectPath + "workers/";
    m_requirementsPath      = m_projectPath + "requirements/";
    m_wishesPath            = m_projectPath + "wishes/";
    m_positionsPath         = m_projectPath + "positions/";
    m_vacationPath          = m_projectPath + "vacations/";
    m_shiftsPath            = m_projectPath + "shifts/";
    m_assignedShiftsPath    = m_projectPath + "assignedShifts/";
}

void MainWindow::slotOpenProject()
{
    if (!askToSaveChanges())
        return;

    QString newPath = QFileDialog::getExistingDirectory(this,
                                                        tr("Open project directory..."),
                                                        m_defaultProjectPath);
    if (newPath.isEmpty())
        return;

    slotCloseProject();
    m_projectPath = newPath + "/";

    loadExistingProject();
    slotShowProjectsKit();
}

void MainWindow::slotUpdateProjectTree()
{
    if (m_projectPath.isEmpty())
        return;

    ui->p_treeWidget_ProjectTree->clear();

    QFont topElementFont;
    topElementFont.setBold(true);

    QTreeWidgetItem *p_tree_Workers = new QTreeWidgetItem;
    p_tree_Workers->setText(0, tr("Workers"));
    p_tree_Workers->setFont(0, topElementFont);

    QTreeWidgetItem *p_tree_Positions = new QTreeWidgetItem;
    p_tree_Positions->setText(0, tr("Positions"));
    p_tree_Positions->setFont(0, topElementFont);

    QTreeWidgetItem *p_tree_Requirements = new QTreeWidgetItem;
    p_tree_Requirements->setText(0, tr("Requirements"));
    p_tree_Requirements->setFont(0, topElementFont);

    QTreeWidgetItem *p_tree_Wishes = new QTreeWidgetItem;
    p_tree_Wishes->setText(0, tr("Wishes"));
    p_tree_Wishes->setFont(0, topElementFont);

    QTreeWidgetItem *p_tree_Vacations = new QTreeWidgetItem;
    p_tree_Vacations->setText(0, tr("Vacations"));
    p_tree_Vacations->setFont(0, topElementFont);

    ui->p_treeWidget_ProjectTree->addTopLevelItem(p_tree_Workers);
    ui->p_treeWidget_ProjectTree->addTopLevelItem(p_tree_Positions);
    ui->p_treeWidget_ProjectTree->addTopLevelItem(p_tree_Requirements);
    ui->p_treeWidget_ProjectTree->addTopLevelItem(p_tree_Wishes);
    ui->p_treeWidget_ProjectTree->addTopLevelItem(p_tree_Vacations);

    // Workers
    for (auto it = m_projectWorkers.begin(); it != m_projectWorkers.end(); it++)
    {
        QTreeWidgetItem *singleWorker = new QTreeWidgetItem;
        QString workerName;
        workerName.append("ID:" + QString::number(it.value().ID_worker));
        workerName.append(" " + it.value().Surname + " ");
        workerName.append(it.value().FirstName + " ");
        workerName.append(it.value().LastName);
        singleWorker->setText(0, workerName);
        singleWorker->setData(1, Qt::ItemDataRole::DisplayRole, QVariant(it.value().ID_worker));
        p_tree_Workers->addChild(singleWorker);
    }

    // Positions
    for (auto it = m_projectPositions.begin(); it != m_projectPositions.end(); it++)
    {
        QTreeWidgetItem *singlePosition = new QTreeWidgetItem;
        QString positionName;
        positionName.append(it.value().Brief);
        singlePosition->setText(0, positionName);
        p_tree_Positions->addChild(singlePosition);
    }

    // Requirements
    for (auto it = m_projectRequirements.begin(); it != m_projectRequirements.end(); it++)
    {
        QTreeWidgetItem *singleRequirement = new QTreeWidgetItem;
        QString requirementName;
        requirementName.append("ID:" + QString::number(it.value().ID) + " ");
        requirementName.append(it.value().Position);
        requirementName.append("\n" + tr("From:") + QDateTime::fromString(it.value().StartDate).toString("dd.MM.yyyy hh:mm"));
        requirementName.append("\n" + tr("To:") + QDateTime::fromString(it.value().EndDate).toString("dd.MM.yyyy hh:mm"));
        if (it.value().NumberWorkers != 1)
            requirementName.append("\n" + tr("Number of workers: ") + QString::number(it.value().NumberWorkers));

        singleRequirement->setText(0, requirementName);
        singleRequirement->setData(1, Qt::ItemDataRole::DisplayRole, it.value().ID);
        p_tree_Requirements->addChild(singleRequirement);
    }

    // Wishes
    for (auto it_worker = m_projectWorkers.begin(); it_worker != m_projectWorkers.end(); it_worker++)
    {
        Wishes filter;
        for (auto it_wish = m_projectWishes.begin(); it_wish != m_projectWishes.end(); it_wish++)
        {
            if (it_wish.value().ID_Workers == it_worker.value().ID_worker)
                filter.insert(it_wish.key(), it_wish.value());
        }

        int numberOfWishes = filter.count();

        QTreeWidgetItem *singleWorkerWithWish = new QTreeWidgetItem;
        QString wishName;

        if (numberOfWishes == 0)
        {
            wishName.append(tr("Worker ID: %1 has no wishes").arg(QString::number(it_worker.value().ID_worker)));
            singleWorkerWithWish->setTextColor(0, QColor(Qt::GlobalColor::gray));
        }
        else
        {
            wishName.append(tr("Worker ID: %1 has %2 ").arg(QString::number(it_worker.value().ID_worker)).arg(QString::number(numberOfWishes)));

            if (numberOfWishes == 1)
            {
                wishName.append(tr("wish"));
            }
            else
            {
                wishName.append(tr("wishes"));
            }
        }

        singleWorkerWithWish->setText(0, wishName);
        singleWorkerWithWish->setData(1, Qt::ItemDataRole::DisplayRole, it_worker.value().ID_worker);
        p_tree_Wishes->addChild(singleWorkerWithWish);
    }

    // Vacations
    for (auto it = m_projectVacations.begin(); it != m_projectVacations.end(); it++)
    {
        QTreeWidgetItem *singleVacation = new QTreeWidgetItem;
        QString vacationName;
        vacationName.append("ID:" + QString::number(it.value().ID_vacation) + " ");
        vacationName.append(tr("Worker:") + QString::number(it.value().ID_worker));
        vacationName.append("\n" + tr("From:") + QDateTime::fromString(it.value().StartDate).toString("dd.MM.yyyy hh:mm"));
        vacationName.append("\n" + tr("To:") + QDateTime::fromString(it.value().EndDate).toString("dd.MM.yyyy hh:mm"));

        singleVacation->setText(0, vacationName);
        singleVacation->setData(1, Qt::ItemDataRole::DisplayRole, it.value().ID_vacation);
        p_tree_Vacations->addChild(singleVacation);
    }

    // Shifts
    ui->p_treeWidget_Shifts->clear();
    for (auto it = m_projectShifts.begin(); it != m_projectShifts.end(); it++)
    {
        QTreeWidgetItem *singleShift = new QTreeWidgetItem;
        QString shiftName;
        shiftName.append(QString("ID: %1 %2\n").arg(QString::number(it.value().ID_Shifts)).arg(it.value().Position));
        shiftName.append(QString("From: %1\n").arg(QDateTime::fromString(it.value().StartDate).toString("dd.MM.yyyy hh:mm")));
        shiftName.append(QString("To: %1\n").arg(QDateTime::fromString(it.value().EndDate).toString("dd.MM.yyyy hh:mm")));

        auto seconds = QDateTime::fromString(it.value().StartDate).secsTo(QDateTime::fromString(it.value().EndDate));
        int hours = seconds / (60 * 60);
        int minutes = (seconds - (hours * 60 * 60)) / 60;

        shiftName.append(QString("Duration: %1 hours ").arg(hours));
        if (minutes)
            shiftName.append(QString("%1 minutes").arg(minutes));

        ui->p_treeWidget_Shifts->addTopLevelItem(singleShift);

        // Якщо зміна прив'язана до робітника, то вона буде сірого кольору і матиме таку мітку
        // щоб не акцентувати увагу на неї при створенні графіку
        auto iteratorAssigned = m_projectAssigned.find(QString::number(it.value().ID_Shifts));
        if (iteratorAssigned == m_projectAssigned.end())
        {
            singleShift->setTextColor(0, QColor(Qt::GlobalColor::black));

            singleShift->setText(0, shiftName);
            singleShift->setData(1, Qt::ItemDataRole::DisplayRole, it.value().ID_Shifts);
        }
        else
        {
            singleShift->setTextColor(0, QColor(Qt::GlobalColor::gray));

            auto assignedWorker = m_projectWorkers.find(QString::number(iteratorAssigned.value().ID_Worker));
            if (assignedWorker == m_projectWorkers.end())
                continue;

            shiftName.append(QString("\nAssigned to worker #%1(%2 %3 %4)")
                             .arg(QString::number(assignedWorker.value().ID_worker))
                             .arg(assignedWorker.value().Surname)
                             .arg(assignedWorker.value().FirstName)
                             .arg(assignedWorker.value().LastName));
            singleShift->setText(0, shiftName);
            singleShift->setData(1, Qt::ItemDataRole::DisplayRole, it.value().ID_Shifts);
        }
    }

    ui->p_treeWidget_ProjectTree->sortByColumn(1, Qt::SortOrder::AscendingOrder);
    ui->p_treeWidget_Shifts->sortByColumn(1, Qt::SortOrder::AscendingOrder);

    slotUpdateCharts();
}

Wishes MainWindow::mergeWishes(Wishes first, Wishes second)
{
    for (auto it = second.begin(); it != second.end();)
    {
        if (first.find(QString::number(it.value().ID_Wishes)) != first.end())
        {
            int newID = findLastWishID(first) + 1;
            first.insert(QString::number(newID), it.value());
            first[QString::number(newID)].ID_Wishes = newID;

            auto delete_iterator = it;
            it++;
            second.erase(delete_iterator);
        }
        else
        {
            first.insert(it.key(), it.value());

            auto delete_iterator = it;
            it++;
            second.erase(delete_iterator);
        }
    }
    return first;
}

int MainWindow::findLastWishID(const Wishes& list)
{
    int result = 0;
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (result < it.value().ID_Wishes)
            result = it.value().ID_Wishes;
    }

    return result;
}

void MainWindow::slotModifyProject()
{
    isModifiedProject = true;
}

int MainWindow::findLastRequirementID(const Requirements& list)
{
    int result = 0;
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (result < it.value().ID)
            result = it.value().ID;
    }

    return result;
}

Requirements MainWindow::mergeRequirements(Requirements first, Requirements second)
{
    for (auto it = second.begin(); it != second.end();)
    {
        if (first.find(QString::number(it.value().ID)) != first.end())
        {
            int newID = findLastRequirementID(first) + 1;
            first.insert(QString::number(newID), it.value());
            first[QString::number(newID)].ID = newID;

            auto delete_iterator = it;
            it++;
            second.erase(delete_iterator);
        }
        else
        {
            first.insert(it.key(), it.value());

            auto delete_iterator = it;
            it++;
            second.erase(delete_iterator);
        }
    }

    return first;
}

void MainWindow::removeFilesInDir(const QString& path)
{
    QDir currentDir(path);
    auto listOfFiles = currentDir.entryList(QDir::Filter::Files);

    for (auto it : listOfFiles)
    {
        QFile(path + "/" + it).remove();
    }
}

void MainWindow::slotCloseProject()
{
    if (!askToSaveChanges())
        return;

    clearProjectData();

    m_projectPath.clear();
    m_workersPath.clear();
    m_requirementsPath.clear();
    m_wishesPath.clear();
    m_positionsPath.clear();
    m_vacationPath.clear();

    ui->p_treeWidget_ProjectTree->clear();
    clearCentralWidget();
    slotChangeCentralWorkspace();
    ui->p_dockWidget_Shifts->hide();
    ui->p_dockWidget_ProjectTree->hide();

    slotUpdateEnablingOfActions(false);
    slotOpenStartWindow();
}

void MainWindow::slotSaveProject()
{
    if (m_projectPath.isEmpty())
        return;

    // Перевірка директорій на існування,
    // якщо вони існують, то їх треба очистити перед збереженням файлів
    // якщо їх не існує, то необхідно створити

    // Лямбда функція для того щоб ця функція не зберігалась в класі, так як використовується лише тут
    auto prepareDir = [&](const QString& path)
    {
        QDir someDir(path);
        if (someDir.exists())
            removeFilesInDir(path);
        else
            someDir.mkpath(path);
    };

    prepareDir(m_projectPath);
    prepareDir(m_requirementsPath);
    prepareDir(m_workersPath);
    prepareDir(m_wishesPath);
    prepareDir(m_positionsPath);
    prepareDir(m_vacationPath);
    prepareDir(m_shiftsPath);
    prepareDir(m_assignedShiftsPath);

    // Збереження файлів проекту
    ProjectDataSaver::saveProjectSetup(m_projectPath + "setup.xml", m_projectSetup);
    ProjectDataSaver::savePositions(m_positionsPath + "positions.xml", m_projectPositions);
    ProjectDataSaver::saveWishes(m_wishesPath + "wishes.xml", m_projectWishes);
    ProjectDataSaver::saveRequirements(m_requirementsPath + "requirements.xml", m_projectRequirements);
    ProjectDataSaver::saveVacations(m_vacationPath + "vacations.xml", m_projectVacations);
    ProjectDataSaver::saveWorkShifts(m_shiftsPath + "shifts.xml", m_projectShifts);
    ProjectDataSaver::saveAssignedShifts(m_assignedShiftsPath + "assigned.xml", m_projectAssigned);

    for (auto it : m_projectWorkers)
    {
        ProjectDataSaver::saveWorker(m_workersPath + QString::number(it.ID_worker) + ".xml", it);
    }

    isModifiedProject = false;
}

void MainWindow::slotSaveAsProject()
{
    QString str = QFileDialog::getExistingDirectory(this,
                                                    tr("Choise the path for project..."),
                                                    m_defaultProjectPath);

    if (str.isEmpty())
        return;

    str.append("/");
    m_projectPath = str;
    updateDirectoryPath();
    slotSaveProject();
}

void MainWindow::slotUpdateEnablingOfActions(bool b)
{
    auto listProject = p_menuProject->actions();
    for (auto it : listProject)
    {
        if (it->text().contains(tr("Save project")))
            it->setEnabled(b);
        if (it->text().contains(tr("Close project")))
            it->setEnabled(b);
    }
    p_menuExport->setEnabled(b);

    p_menuView->setEnabled(b);
    p_menuData->setEnabled(b);
    p_menuPlaning->setEnabled(b);
}

void MainWindow::slotCreateNewWorker()
{
    WorkersInformation *workerWidget = new WorkersInformation(this);
    workerWidget->setWindowModality(Qt::WindowModality::WindowModal);
    workerWidget->slotSetPathPositionsInformation(m_positionsPath);

    connect(workerWidget, SIGNAL(signalSendWorker(Worker)),
            this, SLOT(slotGetWorker(Worker)));

    workerWidget->show();
}

void MainWindow::slotGetWorker(Worker worker)
{
    auto iter = m_projectWorkers.find(QString::number(worker.ID_worker));
    if (iter == m_projectWorkers.end())
    {
        m_projectWorkers.insert(QString::number(worker.ID_worker), worker);

        QMessageBox::information(this,
                                 tr("Success"),
                                 tr("The worker has been successfully added."));
        QWidget* wgt = dynamic_cast<QWidget*>(sender());
        wgt->close();

        auto prepareDir = [&](const QString& path)
        {
            QDir someDir(path);
            if (someDir.exists())
                removeFilesInDir(path);
            else
                someDir.mkpath(path);
        };

        prepareDir(m_workersPath);
        for (auto it : m_projectWorkers)
        {
            ProjectDataSaver::saveWorker(m_workersPath + QString::number(it.ID_worker) + ".xml", it);
        }

        slotUpdateProjectTree();
        slotModifyProject();
    }
    else
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("A worker with this ID already exists."));
    }

}

void MainWindow::slotDeleteWorker()
{
    bool isOK;
    int ID = QInputDialog::getInt(this,
                                  tr("Removing a worker"),
                                  tr("Enter the ID of the worker you want to delete."),
                                  1,
                                  1,
                                  65535,
                                  1,
                                  &isOK);

    if (!isOK)
        return;

    auto iter = m_projectWorkers.find(QString::number(ID));
    if (iter == m_projectWorkers.end())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("This worker does not exist."));
    }
    else
    {
        m_projectWorkers.erase(iter);
        QMessageBox::information(this,
                              tr("Success"),
                              tr("The worker was successfully removed."));

        auto prepareDir = [&](const QString& path)
        {
            QDir someDir(path);
            if (someDir.exists())
                removeFilesInDir(path);
            else
                someDir.mkpath(path);
        };

        prepareDir(m_workersPath);
        for (auto it : m_projectWorkers)
        {
            ProjectDataSaver::saveWorker(m_workersPath + QString::number(it.ID_worker) + ".xml", it);
        }

        slotUpdateProjectTree();
        slotModifyProject();
    }
}

void MainWindow::slotOpenPositionManager()
{
    PositionsWidget* manager = new PositionsWidget(this);
    manager->setModal(true);
    manager->slotSetPositionsData(m_projectPositions);
    manager->setWindowTitle(tr("Position manager"));

    connect(manager, SIGNAL(signalSendPositions(Positions)),
            this, SLOT(slotGetPositions(Positions)));

    manager->show();
}

void MainWindow::slotGetPositions(Positions info)
{
    m_projectPositions = info;

    QWidget* wgt = dynamic_cast<QWidget*>(sender());
    wgt->close();

    ProjectDataSaver::savePositions(m_positionsPath + "positions.xml", m_projectPositions);

    slotUpdateProjectTree();
    slotModifyProject();
}

void MainWindow::slotOpenProjectConfiguration()
{
    ProjectSetupWidget* configurations = new ProjectSetupWidget(this);
    configurations->setModal(true);
    configurations->slotSetConfiguration(m_projectSetup);
    configurations->setWindowTitle(tr("Configurations"));

    connect(configurations, SIGNAL(signalSendConfiguration(ProjectSetup)),
            this, SLOT(slotGetConfiguration(ProjectSetup)));

    configurations->show();
}

void MainWindow::slotGetConfiguration(ProjectSetup conf)
{
    m_projectSetup = conf;

    QWidget* wgt = dynamic_cast<QWidget*>(sender());
    wgt->close();

    slotUpdateProjectTree();
    slotModifyProject();
}

void MainWindow::slotOpenRequirementsManager()
{
    RequirementWidget* manager = new RequirementWidget(this);
    manager->setModal(true);
    manager->slotSetRequirements(m_projectRequirements);
    manager->slotSetPositions(m_projectPositions);
    manager->setWindowTitle(tr("Requirements manager"));

    connect(manager, SIGNAL(signalSendRequirements(Requirements)),
            this, SLOT(slotGetRequirements(Requirements)));

    manager->show();
}

void MainWindow::slotGetRequirements(Requirements info)
{
    m_projectRequirements = info;

    QWidget* wgt = dynamic_cast<QWidget*>(sender());
    wgt->close();

    slotUpdateProjectTree();
    slotModifyProject();
}

void MainWindow::slotOpenVacationsManager()
{
    VacationWidget* manager = new VacationWidget(this);
    manager->setModal(true);
    manager->slotSetVacations(m_projectVacations);
    manager->slotSetPathWorkersInformation(m_workersPath);
    manager->setWindowTitle(tr("Vacation manager"));

    connect(manager, SIGNAL(signalSendVacations(Vacations)),
            this, SLOT(slotGetVacations(Vacations)));

    manager->show();
}

void MainWindow::slotGetVacations(Vacations info)
{
    m_projectVacations = info;

    QWidget* wgt = dynamic_cast<QWidget*>(sender());
    wgt->close();

    slotUpdateProjectTree();
    slotModifyProject();
}

void MainWindow::slotOpenWishesManager()
{
    bool m_ok;
    int n = QInputDialog::getInt(this,
                                 tr("Choise workers ID"),
                                 tr("Enter the workers ID:"),
                                 1,
                                 1,
                                 65535,
                                 1,
                                 &m_ok);
    if (!m_ok)
        return;

    if (m_projectWorkers.find(QString::number(n)) == m_projectWorkers.end())
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("The worker ID %1 does not exists.").arg(n));

    WishesWorkersWidget* manager = new WishesWorkersWidget(this);
    manager->setModal(true);
    manager->slotSetPathWorkersInformation(m_workersPath);

    manager->slotDisableWorkerID(true);
    manager->slotSetWorkerID(n);

    Wishes filterWishes;
    for (auto it = m_projectWishes.begin(); it != m_projectWishes.end(); it++)
    {
        if (it.value().ID_Workers == n)
        {
            filterWishes.insert(it.key(), it.value());
        }
    }

    manager->slotSetWishes(filterWishes);
    manager->setWindowTitle(tr("Wishes manager"));

    connect(manager, SIGNAL(signalSendWishes(Wishes)),
            this, SLOT(slotGetWishes(Wishes)));

    manager->show();
}

void MainWindow::slotGetWishes(Wishes info)
{
    WishesWorkersWidget* wgt = dynamic_cast<WishesWorkersWidget*>(sender());
    int id = wgt->getWorkerID();

    for (auto it_wishes = m_projectWishes.begin(); it_wishes != m_projectWishes.end();)
    {
        if (it_wishes.value().ID_Workers == id)
        {
            auto forErasing = it_wishes;
            it_wishes++;
            m_projectWishes.erase(forErasing);
        }
        else
        {
            it_wishes++;
        }
    }

    m_projectWishes = mergeWishes(m_projectWishes, info);

    wgt->close();

    slotUpdateProjectTree();
    slotModifyProject();
}

int MainWindow::findLastShiftID(const Shifts& info)
{
    int result = 0;
    for (auto it = info.begin(); it != info.end(); it++)
    {
        if (it.value().ID_Shifts > result)
            result = it.value().ID_Shifts;
    }

    return result;
}

void MainWindow::slotAddSingleShift()
{
    int newID = findLastShiftID(m_projectShifts) + 1;
    SingleShiftWidget* wgt = new SingleShiftWidget(this);

    wgt->slotSetShiftID(newID);
    wgt->slotSetPositions(m_projectPositions);
    wgt->slotChangeTextOKButton(tr("Add"));
    wgt->setWindowTitle(tr("Add shift"));

    connect(wgt, SIGNAL(singnalSendShift(SingleShift)),
            this, SLOT(slotAddShift(SingleShift)));

    wgt->show();

}

void MainWindow::slotAddShift(SingleShift info)
{
    m_projectShifts.insert(QString::number(info.ID_Shifts), info);

    QWidget* wgt = dynamic_cast<QWidget*>(sender());
    wgt->close();

    slotUpdateProjectTree();
    slotModifyProject();
}

void MainWindow::slotDeleteSingleShift()
{
    bool isOK;
    int ID = QInputDialog::getInt(this,
                                  tr("Removing a shift"),
                                  tr("Enter the ID of the shift you want to delete."),
                                  1,
                                  1,
                                  65535,
                                  1,
                                  &isOK);

    if (!isOK)
        return;

    auto iter = m_projectShifts.find(QString::number(ID));
    if (iter == m_projectShifts.end())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("This shift does not exist."));
    }
    else
    {
        m_projectShifts.erase(iter);
        QMessageBox::information(this,
                              tr("Success"),
                              tr("The shift was successfully removed."));
        slotUpdateProjectTree();
        slotModifyProject();
    }
}

int MainWindow::findFromTextID(const QString& str)
{
    QString searchText = "ID:";

    int index = str.indexOf(searchText);
    if (index == -1)
        return str.toInt();

    QString resultText;

    for (int i = index + searchText.size(); i < str.size(); i++)
    {
        if (i == index + searchText.size() && str.at(i) == ' ')
            continue;

        if (str.at(i) >= '0' && str.at(i) <= '9')
            resultText += str.at(i);
        else
            break;

    }

    return resultText.toInt();
}

void MainWindow::slotAssignShift()
{
    QList<QString> notAssignedID;

    for (auto iter_shift = m_projectShifts.begin(); iter_shift != m_projectShifts.end(); iter_shift++)
    {
        auto iteratorOnAssigned = m_projectAssigned.find(iter_shift.key());
        if (iteratorOnAssigned == m_projectAssigned.end())
            notAssignedID.push_back(iter_shift.key());
    }

    bool m_ok;

    int shiftID = findFromTextID(QInputDialog::getItem(this,
                                        tr("Shift ID"),
                                        tr("Choise the shift ID:"),
                                        notAssignedID,
                                        0,
                                        false,
                                        &m_ok));

    if (!m_ok)
        return;

    WorkersPriorityLists priority(m_projectVacations,
                                  m_projectShifts,
                                  m_projectAssigned,
                                  m_projectWishes,
                                  m_projectWorkers,
                                  m_projectSetup);

    auto iteratorOnShift = m_projectShifts.find(QString::number(shiftID));
    if (iteratorOnShift == m_projectShifts.end())
    {
        QMessageBox::critical(this,
                              tr("Error!"),
                              tr("This shift does not exist."));
        return;
    }

    auto list = priority.getWorkersListPriority(*iteratorOnShift);

    if (list.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("For the current work shift, there are no workers who can get it."));
        return;
    }
    int workerID = findFromTextID(QInputDialog::getItem(this,
                                         tr("Worker ID"),
                                         tr("Choise the workers ID:"),
                                         list,
                                         -1,
                                         false,
                                         &m_ok));

    if (!m_ok)
        return;

    SingleAssignedShift currentAssign;
    currentAssign.ID_Shift = shiftID;
    currentAssign.ID_Worker = workerID;
    m_projectAssigned.insert(QString::number(currentAssign.ID_Shift), currentAssign);
    slotModifyProject();

    slotUpdateProjectTree();
}

void MainWindow::slotCancelAssignmentShift()
{
    if (m_projectAssigned.size() == 0)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("At the moment, there is no assigned work shift to the worker."));
        return;
    }

    QList<QString> assignedShiftsList;
    for (auto it = m_projectAssigned.begin(); it != m_projectAssigned.end(); it++)
    {
        assignedShiftsList.push_back(QString::number(it.value().ID_Shift));
    }

    bool m_ok;
    int shiftID = QInputDialog::getItem(this,
                                        tr("Cancel assignment"),
                                        tr("Choise a shift ID:"),
                                        assignedShiftsList,
                                        0,
                                        false,
                                        &m_ok).toInt();

    if (!m_ok)
        return;

    m_projectAssigned.erase(m_projectAssigned.find(QString::number(shiftID)));

    QMessageBox::information(this,
                             tr("Success"),
                             tr("The work shift was successfully dismissed."));

    slotModifyProject();
    slotUpdateProjectTree();
}

void MainWindow::clearCentralWidget()
{
    for (int widgetNumber = 0; widgetNumber < ui->p_layoutCentralWidget->count(); widgetNumber++)
    {
        delete ui->p_layoutCentralWidget->itemAt(widgetNumber)->widget();
        ui->p_layoutCentralWidget->removeItem(ui->p_layoutCentralWidget->itemAt(widgetNumber));
    }
}

void MainWindow::slotChartShifts()
{
    ShiftCharts* chart = new ShiftCharts;
    chart->setMinDateTime(QDateTime::fromString(m_projectSetup.StartPeriod));
    chart->setMaxDateTime(QDateTime::fromString(m_projectSetup.EndPeriod));
    chart->setLabelAngle(-90);
    chart->setHorizontalTickCount(25);
    chart->setChartTitle("Shifts");

    chart->setShiftsChart(m_projectShifts);

    clearCentralWidget();
    ui->p_layoutCentralWidget->addWidget(chart);
}

void MainWindow::slotChartRequirements()
{
    ShiftCharts* chart = new ShiftCharts;
    chart->setMinDateTime(QDateTime::fromString(m_projectSetup.StartPeriod));
    chart->setMaxDateTime(QDateTime::fromString(m_projectSetup.EndPeriod));
    chart->setLabelAngle(-90);
    chart->setHorizontalTickCount(25);
    chart->setChartTitle("Requirements");

    chart->setRequirementsChart(m_projectRequirements);

    clearCentralWidget();
    ui->p_layoutCentralWidget->addWidget(chart);
}

void MainWindow::slotChartDiffs()
{
    ShiftCharts* chart = new ShiftCharts;
    chart->setMinDateTime(QDateTime::fromString(m_projectSetup.StartPeriod));
    chart->setMaxDateTime(QDateTime::fromString(m_projectSetup.EndPeriod));
    chart->setLabelAngle(-90);
    chart->setHorizontalTickCount(25);
    chart->setChartTitle("Diffs");

    chart->setDiffRequirementsAndShiftsChart(m_projectRequirements, m_projectShifts);

    clearCentralWidget();
    ui->p_layoutCentralWidget->addWidget(chart);
}

void MainWindow::slotChartsWidget()
{
    QSplitter* p_splitter = new QSplitter(Qt::Orientation::Vertical, this);

    ShiftCharts* chartRequirements = new ShiftCharts;
    chartRequirements->setMinDateTime(QDateTime::fromString(m_projectSetup.StartPeriod));
    chartRequirements->setMaxDateTime(QDateTime::fromString(m_projectSetup.EndPeriod));
    chartRequirements->setLabelAngle(-90);
    chartRequirements->setHorizontalTickCount(25);
    chartRequirements->setChartTitle("Requirements");
    chartRequirements->setRequirementsChart(m_projectRequirements);

    ShiftCharts* chartShifts = new ShiftCharts;
    chartShifts->setMinDateTime(QDateTime::fromString(m_projectSetup.StartPeriod));
    chartShifts->setMaxDateTime(QDateTime::fromString(m_projectSetup.EndPeriod));
    chartShifts->setLabelAngle(-90);
    chartShifts->setHorizontalTickCount(25);
    chartShifts->setChartTitle("Shifts");
    chartShifts->setShiftsChart(m_projectShifts);

    ShiftCharts* chartDiff = new ShiftCharts;
    chartDiff->setMinDateTime(QDateTime::fromString(m_projectSetup.StartPeriod));
    chartDiff->setMaxDateTime(QDateTime::fromString(m_projectSetup.EndPeriod));
    chartDiff->setLabelAngle(-90);
    chartDiff->setHorizontalTickCount(25);
    chartDiff->setChartTitle("Diffs");
    chartDiff->setDiffRequirementsAndShiftsChart(m_projectRequirements, m_projectShifts);

    p_splitter->addWidget(chartRequirements);
    p_splitter->addWidget(chartShifts);
    p_splitter->addWidget(chartDiff);

    connect(chartRequirements,      SIGNAL(changedLabelAngle(int)),
            chartShifts,            SLOT(setLabelAngle(int)));
    connect(chartRequirements,      SIGNAL(changedLabelAngle(int)),
            chartDiff,              SLOT(setLabelAngle(int)));
    connect(chartShifts,            SIGNAL(changedLabelAngle(int)),
            chartRequirements,      SLOT(setLabelAngle(int)));
    connect(chartShifts,            SIGNAL(changedLabelAngle(int)),
            chartDiff,              SLOT(setLabelAngle(int)));
    connect(chartDiff,              SIGNAL(changedLabelAngle(int)),
            chartRequirements,      SLOT(setLabelAngle(int)));
    connect(chartDiff,              SIGNAL(changedLabelAngle(int)),
            chartShifts,            SLOT(setLabelAngle(int)));

    connect(chartRequirements,      SIGNAL(changedHorizontalTickCount(int)),
            chartShifts,            SLOT(setHorizontalTickCount(int)));
    connect(chartRequirements,      SIGNAL(changedHorizontalTickCount(int)),
            chartDiff,              SLOT(setHorizontalTickCount(int)));
    connect(chartShifts,            SIGNAL(changedHorizontalTickCount(int)),
            chartRequirements,      SLOT(setHorizontalTickCount(int)));
    connect(chartShifts,            SIGNAL(changedHorizontalTickCount(int)),
            chartDiff,              SLOT(setHorizontalTickCount(int)));
    connect(chartDiff,              SIGNAL(changedHorizontalTickCount(int)),
            chartRequirements,      SLOT(setHorizontalTickCount(int)));
    connect(chartDiff,              SIGNAL(changedHorizontalTickCount(int)),
            chartShifts,            SLOT(setHorizontalTickCount(int)));

    connect(chartRequirements,      SIGNAL(changedVisibleSeries(const QString&, bool)),
            chartShifts,            SLOT(setVisibleSeries(const QString&, bool)));
    connect(chartRequirements,      SIGNAL(changedVisibleSeries(const QString&, bool)),
            chartDiff,              SLOT(setVisibleSeries(const QString&, bool)));
    connect(chartShifts,            SIGNAL(changedVisibleSeries(const QString&, bool)),
            chartRequirements,      SLOT(setVisibleSeries(const QString&, bool)));
    connect(chartShifts,            SIGNAL(changedVisibleSeries(const QString&, bool)),
            chartDiff,              SLOT(setVisibleSeries(const QString&, bool)));
    connect(chartDiff,              SIGNAL(changedVisibleSeries(const QString&, bool)),
            chartRequirements,      SLOT(setVisibleSeries(const QString&, bool)));
    connect(chartDiff,              SIGNAL(changedVisibleSeries(const QString&, bool)),
            chartShifts,            SLOT(setVisibleSeries(const QString&, bool)));

    connect(chartRequirements,      SIGNAL(changedColorSeries(const QString&, const QColor&)),
            chartShifts,            SLOT(setSeriesColor(const QString&, const QColor&)));
    connect(chartRequirements,      SIGNAL(changedColorSeries(const QString&, const QColor&)),
            chartDiff,              SLOT(setSeriesColor(const QString&, const QColor&)));
    connect(chartShifts,            SIGNAL(changedColorSeries(const QString&, const QColor&)),
            chartRequirements,      SLOT(setSeriesColor(const QString&, const QColor&)));
    connect(chartShifts,            SIGNAL(changedColorSeries(const QString&, const QColor&)),
            chartDiff,              SLOT(setSeriesColor(const QString&, const QColor&)));
    connect(chartDiff,              SIGNAL(changedColorSeries(const QString&, const QColor&)),
            chartRequirements,      SLOT(setSeriesColor(const QString&, const QColor&)));
    connect(chartDiff,              SIGNAL(changedColorSeries(const QString&, const QColor&)),
            chartShifts,            SLOT(setSeriesColor(const QString&, const QColor&)));

    connect(chartRequirements,      SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            chartShifts,            SLOT(setRange(const QDateTime&, const QDateTime&)));
    connect(chartRequirements,      SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            chartDiff,              SLOT(setRange(const QDateTime&, const QDateTime&)));
    connect(chartShifts,            SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            chartRequirements,      SLOT(setRange(const QDateTime&, const QDateTime&)));
    connect(chartShifts,            SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            chartDiff,              SLOT(setRange(const QDateTime&, const QDateTime&)));
    connect(chartDiff,              SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            chartRequirements,      SLOT(setRange(const QDateTime&, const QDateTime&)));
    connect(chartDiff,              SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            chartShifts,            SLOT(setRange(const QDateTime&, const QDateTime&)));

    clearCentralWidget();
    ui->p_layoutCentralWidget->addWidget(p_splitter);
}

void MainWindow::slotChangeCentralWorkspace()
{
    for (auto it : p_menuCentralWorkspace->actions())
    {
        it->setChecked(false);
    }

    QAction* current = dynamic_cast<QAction*>(sender());
    if (!current)
        return;
    current->setChecked(true);
}

void MainWindow::slotExportByTime()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                                                         tr("Export dir..."),
                                                         m_projectPath);

    if (filePath.isEmpty())
        return;
    filePath.append("/");

    // Вибір тільки присвоєних змін
    Shifts onlyAssigned;
    for (auto it_assigned : m_projectAssigned)
    {
        auto it_shift = m_projectShifts.find(QString::number(it_assigned.ID_Shift));

        if (it_shift == m_projectShifts.end())
            continue;

        onlyAssigned.insert(it_shift.key(), it_shift.value());
    }

    if (onlyAssigned.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The list of assigned shifts is empty."));
        return;
    }

    // Сортування по даті
    QVector<SingleShift> sortedShifts;
    for (auto it : onlyAssigned)
    {
        sortedShifts.push_back(it);
    }

    auto lessThan = [](const SingleShift& first, const SingleShift& second)
    {
        if (QDateTime::fromString(first.StartDate) < QDateTime::fromString(second.StartDate))
            return true;
        else
            return false;
    };
    std::sort(sortedShifts.begin(), sortedShifts.end(), lessThan);

    QString shiftsData;
    for (auto it_shift : sortedShifts)
    {
        auto it_assigned = m_projectAssigned.find(QString::number(it_shift.ID_Shifts));
        auto it_worker = m_projectWorkers.find(QString::number(it_assigned.value().ID_Worker));

        Worker worker;
        if (it_worker != m_projectWorkers.end())
        {
            worker = *it_worker;
        }
        else
        {
            worker.Surname = "Unknown";
            worker.FirstName = "Unknown";
            worker.LastName = "Unknown";
            worker.ID_worker = it_assigned.value().ID_Worker;
        }

        shiftsData.append(QString("%1\t%2 %3 %4(ID:%5)\t%6-%7\n")
                          .arg(it_shift.Position)
                          .arg(worker.Surname)
                          .arg(worker.FirstName)
                          .arg(worker.LastName)
                          .arg(worker.ID_worker)
                          .arg(QDateTime::fromString(it_shift.StartDate).toString("dd.MM.yyyy hh:mm"))
                          .arg(QDateTime::fromString(it_shift.EndDate).toString("dd.MM.yyyy hh:mm")));
    }

    if (shiftsData.isEmpty())
    {
        return;
    }

    QFile fileShifts(filePath + "shiftsByTime");
    fileShifts.open(QFile::OpenModeFlag::Text | QFile::OpenModeFlag::WriteOnly);

    if (!fileShifts.isOpen())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("File %1 not opened").arg(fileShifts.fileName()));
        return;
    }

    fileShifts.write(shiftsData.toUtf8());
    fileShifts.close();

    QString msgText = tr("A files have been exported.");
    //msgText.append(fileShifts.fileName());

    QMessageBox::information(this,
                             tr("Success"),
                             msgText);
}

void MainWindow::slotExportByWorkers()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                                                         tr("Export dir..."),
                                                         m_projectPath);

    if (filePath.isEmpty())
        return;
    filePath.append("/");

    // Вибір тільки присвоєних змін
    Shifts onlyAssigned;
    for (auto it_assigned : m_projectAssigned)
    {
        auto it_shift = m_projectShifts.find(QString::number(it_assigned.ID_Shift));

        if (it_shift == m_projectShifts.end())
            continue;

        onlyAssigned.insert(it_shift.key(), it_shift.value());
    }

    if (onlyAssigned.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The list of assigned shifts is empty."));
        return;
    }

    // Сортування по даті
    QVector<SingleShift> sortedShifts;
    for (auto it : onlyAssigned)
    {
        sortedShifts.push_back(it);
    }

    auto lessThan = [](const SingleShift& first, const SingleShift& second)
    {
        if (QDateTime::fromString(first.StartDate) < QDateTime::fromString(second.StartDate))
            return true;
        else
            return false;
    };

    std::sort(sortedShifts.begin(), sortedShifts.end(), lessThan);

    // Запис до файлу
    QList<QString> createdFiles;

    for (auto it_worker : m_projectWorkers)
    {
        QString workerData;
        workerData.append(QString("%1 %2 %3(ID:%4)\n")
                          .arg(it_worker.Surname)
                          .arg(it_worker.FirstName)
                          .arg(it_worker.LastName)
                          .arg(it_worker.ID_worker));

        QString shiftsData;
        for (auto it_shift : sortedShifts)
        {
            auto it_assigned = m_projectAssigned.find(QString::number(it_shift.ID_Shifts));
            if (it_assigned.value().ID_Worker != it_worker.ID_worker)
                continue;

            shiftsData.append(QString("%1\t%2-%3\n")
                              .arg(it_shift.Position)
                              .arg(QDateTime::fromString(it_shift.StartDate).toString("dd.MM.yyyy hh:mm"))
                              .arg(QDateTime::fromString(it_shift.EndDate).toString("dd.MM.yyyy hh:mm")));
        }

        if (shiftsData.isEmpty())
        {
            continue;
        }

        QFile fileWorker(filePath + QString::number(it_worker.ID_worker));
        fileWorker.open(QFile::OpenModeFlag::Text | QFile::OpenModeFlag::WriteOnly);

        if (!fileWorker.isOpen())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("File %1 not opened").arg(fileWorker.fileName()));
            return;
        }

        shiftsData = workerData + shiftsData;

        fileWorker.write(shiftsData.toUtf8());
        createdFiles.push_back(fileWorker.fileName());
        fileWorker.close();
    }

    QString msgText = tr("A files have been exported.");
    //msgText.append(createdFiles.join('\n'));

    QMessageBox::information(this,
                             tr("Success"),
                             msgText);
}

void MainWindow::slotExportByPositions()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
                                                         tr("Export dir..."),
                                                         m_projectPath);

    if (filePath.isEmpty())
        return;
    filePath.append("/");

    // Вибір тільки присвоєних змін
    Shifts onlyAssigned;
    for (auto it_assigned : m_projectAssigned)
    {
        auto it_shift = m_projectShifts.find(QString::number(it_assigned.ID_Shift));

        if (it_shift == m_projectShifts.end())
            continue;

        onlyAssigned.insert(it_shift.key(), it_shift.value());
    }

    if (onlyAssigned.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The list of assigned shifts is empty."));
        return;
    }

    // Сортування по даті
    QVector<SingleShift> sortedShifts;
    for (auto it : onlyAssigned)
    {
        sortedShifts.push_back(it);
    }

    auto lessThan = [](const SingleShift& first, const SingleShift& second)
    {
        if (QDateTime::fromString(first.StartDate) < QDateTime::fromString(second.StartDate))
            return true;
        else
            return false;
    };

    std::sort(sortedShifts.begin(), sortedShifts.end(), lessThan);

    // Запис до файлу
    QList<QString> createdFiles;

    for (auto it_position : m_projectPositions)
    {
        QString shiftsData;

        for (auto it_shift : sortedShifts)
        {
            if (it_shift.Position == it_position.Brief)
            {
                auto it_assigned = m_projectAssigned.find(QString::number(it_shift.ID_Shifts));
                auto it_worker = m_projectWorkers.find(QString::number(it_assigned.value().ID_Worker));

                Worker worker;
                if (it_worker != m_projectWorkers.end())
                {
                    worker = *it_worker;
                }
                else
                {
                    worker.Surname = "Unknown";
                    worker.FirstName = "Unknown";
                    worker.LastName = "Unknown";
                    worker.ID_worker = it_assigned.value().ID_Worker;
                }

                shiftsData.append(QString("%1 %2 %3(ID:%4)\t%5-%6\n")
                                  .arg(worker.Surname)
                                  .arg(worker.FirstName)
                                  .arg(worker.LastName)
                                  .arg(worker.ID_worker)
                                  .arg(QDateTime::fromString(it_shift.StartDate).toString("dd.MM.yyyy hh:mm"))
                                  .arg(QDateTime::fromString(it_shift.EndDate).toString("dd.MM.yyyy hh:mm")));

            }
        }

        if (shiftsData.isEmpty())
        {
            continue;
        }

        QFile filePosition(filePath + it_position.Brief);
        filePosition.open(QFile::OpenModeFlag::Text | QFile::OpenModeFlag::WriteOnly);

        if (!filePosition.isOpen())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("File %1 not opened").arg(filePosition.fileName()));
            return;
        }

        shiftsData = it_position.Brief + "\n" + shiftsData;

        filePosition.write(shiftsData.toUtf8());
        createdFiles.push_back(filePosition.fileName());
        filePosition.close();
    }

    QString msgText = tr("A files have been exported.");
    //msgText.append(createdFiles.join('\n'));

    QMessageBox::information(this,
                             tr("Success"),
                             msgText);
}

void MainWindow::slotEditData(QTreeWidgetItem *item, int column)
{
    if (item->treeWidget() == ui->p_treeWidget_ProjectTree)
    {
        if (!item->parent())
            return;

        if (item->parent()->text(0) == "Workers")
        {
            slotEditWorker(item->text(1).toInt());
            return;
        }

        if (item->parent()->text(0) == "Positions")
        {
            slotOpenPositionManager();
            return;
        }

        if (item->parent()->text(0) == "Requirements")
        {
            slotOpenRequirementsManager();
            return;
        }

        if (item->parent()->text(0) == "Wishes")
        {
            editWish(item->text(1).toInt());
            return;
        }

        if (item->parent()->text(0) == "Vacations")
        {
            slotOpenVacationsManager();
            return;
        }
    }

    if (item->treeWidget() == ui->p_treeWidget_Shifts)
    {
        // TO DO
        editShift(item->text(1).toInt());
        return;
    }
}

void MainWindow::slotEditShift(SingleShift shift)
{
    auto oldShift = m_projectShifts.find(QString::number(shift.ID_Shifts));
    if (oldShift != m_projectShifts.end())
        m_projectShifts.erase(oldShift);
    m_projectShifts.insert(QString::number(shift.ID_Shifts), shift);

    QWidget* wgt = dynamic_cast<QWidget*>(sender());
    wgt->close();

    slotUpdateProjectTree();
    slotModifyProject();
}

void MainWindow::editShift(int id)
{
    SingleShiftWidget* wgt = new SingleShiftWidget(this);

    wgt->slotSetShiftID(id);
    wgt->slotSetPositions(m_projectPositions);
    wgt->slotSetSingleShift(*m_projectShifts.find(QString::number(id)));
    wgt->slotChangeTextOKButton(tr("Save"));
    wgt->setWindowTitle(tr("Edit shift"));

    connect(wgt, SIGNAL(singnalSendShift(SingleShift)),
            this, SLOT(slotEditShift(SingleShift)));

    wgt->show();
}

void MainWindow::editWish(int id)
{
    if (m_projectWorkers.find(QString::number(id)) == m_projectWorkers.end())
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("The worker ID %1 does not exists.").arg(id));

    WishesWorkersWidget* manager = new WishesWorkersWidget(this);
    manager->setModal(true);
    manager->slotSetPathWorkersInformation(m_workersPath);

    manager->slotDisableWorkerID(true);
    manager->slotSetWorkerID(id);

    Wishes filterWishes;
    for (auto it = m_projectWishes.begin(); it != m_projectWishes.end(); it++)
    {
        if (it.value().ID_Workers == id)
        {
            filterWishes.insert(it.key(), it.value());
        }
    }

    manager->slotSetWishes(filterWishes);
    manager->setWindowTitle(tr("Wishes manager"));

    connect(manager, SIGNAL(signalSendWishes(Wishes)),
            this, SLOT(slotGetWishes(Wishes)));

    manager->show();
}

void MainWindow::slotEditWorker(int id)
{
    auto it_worker = m_projectWorkers.find(QString::number(id));

    WorkersInformation* wgt = new WorkersInformation();
    wgt->slotSetWorkerData(it_worker.value());
    wgt->setModal(true);
    wgt->slotSetPathPositionsInformation(m_positionsPath);
    wgt->slotDisableID();
    wgt->slotChangeButtonText(tr("Save changes"));

    connect(wgt, SIGNAL(signalSendWorker(Worker)),
            this, SLOT(slotChangeWorkersInformation(Worker)));

    wgt->show();
}

void MainWindow::slotChangeWorkersInformation(Worker w)
{
    m_projectWorkers.erase(m_projectWorkers.find(QString::number(w.ID_worker)));
    m_projectWorkers.insert(QString::number(w.ID_worker), w);
    slotModifyProject();
    slotUpdateProjectTree();

    WorkersInformation* currentWidget = dynamic_cast<WorkersInformation*>(sender());
    currentWidget->close();
}

void MainWindow::slotFindWorker()
{
    WorkerSearchResult* searchWidget = new WorkerSearchResult;
    searchWidget->setWorkers(m_projectWorkers);
    searchWidget->setPositions(m_projectPositions);
    searchWidget->updateResults();

    searchWidget->show();
}

void MainWindow::slotFindRequrements()
{
    FindRequirements* searchWidget = new FindRequirements;
    searchWidget->setPositions(m_projectPositions);
    searchWidget->setRequirements(m_projectRequirements);
    searchWidget->setStartDate(QDateTime::fromString(m_projectSetup.StartPeriod));
    searchWidget->setEndDate(QDateTime::fromString(m_projectSetup.EndPeriod));
    searchWidget->updateResults();

    searchWidget->show();
}

void MainWindow::slotFindWishes()
{
    FindWishes* searchWidget = new FindWishes;
    searchWidget->setPositions(m_projectPositions);
    searchWidget->setWishes(m_projectWishes);
    searchWidget->setWorkers(m_projectWorkers);
    searchWidget->setStartDate(QDateTime::fromString(m_projectSetup.StartPeriod));
    searchWidget->setEndDate(QDateTime::fromString(m_projectSetup.EndPeriod));
    searchWidget->updateResults();

    searchWidget->show();
}

void MainWindow::slotFindVacations()
{
    FindVacation* searchWidget = new FindVacation;
    searchWidget->setVacations(m_projectVacations);
    searchWidget->setWorkers(m_projectWorkers);
    searchWidget->setStartDate(QDateTime::fromString(m_projectSetup.StartPeriod));
    searchWidget->setEndDate(QDateTime::fromString(m_projectSetup.EndPeriod));
    searchWidget->updateResults();

    searchWidget->show();
}

void MainWindow::slotFindShifts()
{
    FindShifts* searchWidget = new FindShifts;
    searchWidget->setShifts(m_projectShifts);
    searchWidget->setAssignedShifts(m_projectAssigned);
    searchWidget->setPositions(m_projectPositions);
    searchWidget->setStartDate(QDateTime::fromString(m_projectSetup.StartPeriod));
    searchWidget->setEndDate(QDateTime::fromString(m_projectSetup.EndPeriod));
    searchWidget->updateResults();

    searchWidget->show();
}

void MainWindow::slotFindAssignedShifts()
{
    FindAssignedShifts* searchWidget = new FindAssignedShifts;

    searchWidget->setShifts(m_projectShifts);
    searchWidget->setPositions(m_projectPositions);
    searchWidget->setAssigned(m_projectAssigned);
    searchWidget->setWorkers(m_projectWorkers);
    searchWidget->setStartDate(QDateTime::fromString(m_projectSetup.StartPeriod));
    searchWidget->setEndDate(QDateTime::fromString(m_projectSetup.EndPeriod));
    searchWidget->updateResults();

    searchWidget->show();
}

void MainWindow::slotOpenStartWindow()
{
    for (auto it : p_menuView->actions())
    {
        if (it->isCheckable())
            it->setChecked(false);
    }

    StartApplicationWidget* wgt = new StartApplicationWidget(m_defaultProjectPath);
    connect(wgt, SIGNAL(clickedCreateNewProject()),
            this, SLOT(slotCreateNewProject()));
    connect(wgt, SIGNAL(clickedOpenProject()),
            this, SLOT(slotOpenProject()));
    connect(wgt, SIGNAL(clickedCreateNewProjectBasedOnTheExisting()),
            this, SLOT(slotCreateNewProjectBasedOnExisting()));

    connect(wgt, SIGNAL(clickedOpenLastProject(const QString&)),
            this, SLOT(slotOpenProjectByName(const QString&)));

    clearCentralWidget();
    ui->p_layoutCentralWidget->addWidget(wgt);
}

void MainWindow::loadExistingProject()
{
    updateDirectoryPath();

    // Load project setup
    m_projectSetup = ProjectDataLoader::loadProjectSetup(m_projectPath + "setup.xml");
    // Load positions
    m_projectPositions = ProjectDataLoader::loadPositions(m_positionsPath + "positions.xml");
    // Load vacations
    m_projectVacations = ProjectDataLoader::loadVacations(m_vacationPath + "vacations.xml");

    // Load workers
    QDir workersDir(m_workersPath);
    QStringList workersFiles = workersDir.entryList(QDir::Filter::Files);
    for (auto it : workersFiles)
    {
        Worker tempWorker = ProjectDataLoader::loadWorker(m_workersPath + it);
        m_projectWorkers.insert(QString::number(tempWorker.ID_worker), tempWorker);
    }

    // Load requirements
    QDir requirementsDir(m_requirementsPath);
    QStringList requirementsFiles = requirementsDir.entryList(QDir::Filter::Files);
    for (auto it : requirementsFiles)
    {
        Requirements tempRequirements = ProjectDataLoader::loadRequirements(m_requirementsPath + it);
        m_projectRequirements = mergeRequirements(m_projectRequirements, tempRequirements);
    }

    // Load wishes
    QDir wishesDir(m_wishesPath);
    QStringList wishesFiles = wishesDir.entryList(QDir::Filter::Files);
    for (auto it : wishesFiles)
    {
        Wishes tempWishes = ProjectDataLoader::loadWishes(m_wishesPath + it);
        m_projectWishes = mergeWishes(m_projectWishes, tempWishes);
    }

    // Load shifts
    m_projectShifts = ProjectDataLoader::loadShifts(m_shiftsPath + "shifts.xml");

    // Load assigned shifts
    m_projectAssigned = ProjectDataLoader::loadAssignedShifts(m_assignedShiftsPath + "assigned.xml");

    slotUpdateProjectTree();
    isModifiedProject = false;
    slotUpdateEnablingOfActions(true);
}

void MainWindow::slotOpenProjectByName(const QString& name)
{
    if (!askToSaveChanges())
        return;

    slotCloseProject();

    QString newPath = m_defaultProjectPath + "/" + name;
    m_projectPath = newPath + "/";

    loadExistingProject();
    slotShowProjectsKit();
}

void MainWindow::slotCreateNewProjectBasedOnExisting()
{
    if (!askToSaveChanges())
        return;

    QString newPath = QFileDialog::getExistingDirectory(this,
                                                        tr("Open project directory..."),
                                                        m_defaultProjectPath);
    if (newPath.isEmpty())
        return;

    slotCloseProject();
    m_projectPath = newPath + "/";
    updateDirectoryPath();

    // Project setup
    m_projectSetup = ProjectDataLoader::loadProjectSetup(m_projectPath + "setup.xml");

    CreatingProjectBasedOnExistingDialog* wgt = new CreatingProjectBasedOnExistingDialog(this);
    wgt->setPeriod(QDateTime::fromString(m_projectSetup.StartPeriod),
                   QDateTime::fromString(m_projectSetup.EndPeriod));
    wgt->setDefaultProjectPath(m_defaultProjectPath);
    wgt->setModal(true);
    wgt->show();

    connect(wgt, &CreatingProjectBasedOnExistingDialog::definedConfiguration,
            [&](const ConfigurationCreationProjectBasedExisting& conf)
    {
        int adaptationSecs = QDateTime::fromString(m_projectSetup.StartPeriod).secsTo(conf.startPeriod);

        // Load positions
        if (conf.positions)
        {
            m_projectPositions = ProjectDataLoader::loadPositions(m_positionsPath + "positions.xml");
        }

        // Load vacations
        if (conf.vacations)
        {
            m_projectVacations = ProjectDataLoader::loadVacations(m_vacationPath + "vacations.xml");

            if (conf.adaptingVacations)
            {
                for (auto &it : m_projectVacations)
                {
                    it.StartDate = QDateTime::fromString(it.StartDate).addSecs(adaptationSecs).toString();
                    it.EndDate = QDateTime::fromString(it.EndDate).addSecs(adaptationSecs).toString();
                }
            }
        }

        // Load workers
        if (conf.workers)
        {
            QDir workersDir(m_workersPath);
            QStringList workersFiles = workersDir.entryList(QDir::Filter::Files);
            for (auto it : workersFiles)
            {
                Worker tempWorker = ProjectDataLoader::loadWorker(m_workersPath + it);
                m_projectWorkers.insert(QString::number(tempWorker.ID_worker), tempWorker);
            }
        }

        // Load requirements
        if (conf.requirements)
        {
            m_projectRequirements = ProjectDataLoader::loadRequirements(m_requirementsPath + "requirements.xml");

            if (conf.adaptingRequirements)
            {
                for (auto &it : m_projectRequirements)
                {
                    it.StartDate = QDateTime::fromString(it.StartDate).addSecs(adaptationSecs).toString();
                    it.EndDate = QDateTime::fromString(it.EndDate).addSecs(adaptationSecs).toString();
                }
            }
        }

        // Load wishes
        if (conf.wishes)
        {
            m_projectWishes = ProjectDataLoader::loadWishes(m_wishesPath + "wishes.xml");

            if (conf.adaptingWishes)
            {
                for (auto &it : m_projectWishes)
                {
                    it.StartDate = QDateTime::fromString(it.StartDate).addSecs(adaptationSecs).toString();
                    it.EndDate = QDateTime::fromString(it.EndDate).addSecs(adaptationSecs).toString();
                }
            }
        }

        // Load shifts
        if (conf.shifts)
        {
            m_projectShifts = ProjectDataLoader::loadShifts(m_shiftsPath + "shifts.xml");

            if (conf.adaptingShifts)
            {
                for (auto &it : m_projectShifts)
                {
                    it.StartDate = QDateTime::fromString(it.StartDate).addSecs(adaptationSecs).toString();
                    it.EndDate = QDateTime::fromString(it.EndDate).addSecs(adaptationSecs).toString();
                }
            }
        }

        // Load assigned shifts
        if (conf.assigned)
        {
            m_projectAssigned = ProjectDataLoader::loadAssignedShifts(m_assignedShiftsPath + "assigned.xml");
        }

        m_projectSetup.StartPeriod = conf.startPeriod.toString();
        m_projectSetup.EndPeriod = conf.endPeriod.toString();

        m_projectPath = m_defaultProjectPath + "/" + conf.projectName + "/";
        updateDirectoryPath();

        slotShowProjectsKit();
        slotUpdateEnablingOfActions(true);
        isModifiedProject = false;
    });

    connect(wgt, &CreatingProjectBasedOnExistingDialog::rejected,
            [&]()
    {
        slotCloseProject();
        slotOpenStartWindow();
        slotUpdateEnablingOfActions(false);
    });
}

void MainWindow::slotShowProjectsKit()
{
    slotUpdateProjectTree();
    ui->p_dockWidget_ProjectTree->show();
    ui->p_dockWidget_Shifts->show();

    p_showCharts->trigger();
    //slotChartsWidget();
    slotUpdateEnablingOfActions(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (askToSaveChanges())
        event->accept();
    else
        event->ignore();
}

bool MainWindow::askToSaveChanges()
{
    QMessageBox::StandardButton resultButton = checkChangesInCurrentProject();

    switch (resultButton)
    {
    case QMessageBox::StandardButton::No:
        return true;
    case QMessageBox::StandardButton::Yes:
        slotSaveProject();
        return true;
    case QMessageBox::StandardButton::Cancel:
        return false;
    default:
        return false;
    }
}

void MainWindow::slotRemoveAllVacations()
{
    auto result = QMessageBox::warning(this,
                               tr("Removing of all vacations"),
                               tr("Are you sure you want to remove all vacations?"),
                               QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

    if (result == QMessageBox::StandardButton::Yes)
    {
        m_projectVacations.clear();
        slotModifyProject();
        slotUpdateProjectTree();
    }
}

void MainWindow::slotRemoveAllRequirements()
{
    auto result = QMessageBox::warning(this,
                               tr("Removing of all requirements"),
                               tr("Are you sure you want to remove all requirements?"),
                               QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

    if (result == QMessageBox::StandardButton::Yes)
    {
        m_projectRequirements.clear();
        slotModifyProject();
        slotUpdateProjectTree();
    }
}

void MainWindow::slotRemoveAllAssignedShifts()
{
    auto result = QMessageBox::warning(this,
                               tr("Removing of all assigned shifts"),
                               tr("Are you sure you want to remove all assigned shifts?"),
                               QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

    if (result == QMessageBox::StandardButton::Yes)
    {
        m_projectAssigned.clear();
        slotModifyProject();
        slotUpdateProjectTree();
    }
}

void MainWindow::slotRemoveAllShiftsAndAssignments()
{
    auto result = QMessageBox::warning(this,
                               tr("Removing of all work shifts and all assignments"),
                               tr("Are you sure you want to remove all work shifts and all assignments?"),
                               QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

    if (result == QMessageBox::StandardButton::Yes)
    {
        m_projectShifts.clear();
        m_projectAssigned.clear();
        slotModifyProject();
        slotUpdateProjectTree();
    }
}

void MainWindow::slotRemoveAllWishes()
{
    auto result = QMessageBox::warning(this,
                               tr("Removing of all wishes"),
                               tr("Are you sure you want to remove all wishes?"),
                               QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

    if (result == QMessageBox::StandardButton::Yes)
    {
        m_projectWishes.clear();
        slotModifyProject();
        slotUpdateProjectTree();
    }
}

// AutomatedAlgorithmsBuildingSchedules
void MainWindow::slotMinStandart()
{
    DurationOfWorkShiftDialog *wgt = new DurationOfWorkShiftDialog;
    connect(wgt, &DurationOfWorkShiftDialog::EnteredDurations,
            [&](const DurationsOfWorkShift& info)
    {
        QList<QString> positions;
        for (auto it : m_projectPositions)
        {
            positions.push_back(it.Brief);
        }

        m_projectShifts = AutomatedAlgorithmsBuildingSchedules::MinStandart(info,
                                                                            m_projectSetup,
                                                                            m_projectRequirements,
                                                                            positions,
                                                                            m_projectShifts);

        slotModifyProject();
        slotUpdateProjectTree();
    });

    wgt->show();
}

void MainWindow::slotMinMax()
{
    DurationOfWorkShiftDialog *wgt = new DurationOfWorkShiftDialog;
    connect(wgt, &DurationOfWorkShiftDialog::EnteredDurations,
            [&](const DurationsOfWorkShift& info)
    {
        QList<QString> positions;
        for (auto it : m_projectPositions)
        {
            positions.push_back(it.Brief);
        }

        m_projectShifts = AutomatedAlgorithmsBuildingSchedules::MinMax(info,
                                                                       m_projectSetup,
                                                                       m_projectRequirements,
                                                                       positions,
                                                                       m_projectShifts);

        slotModifyProject();
        slotUpdateProjectTree();
    });

    wgt->show();
}

void MainWindow::slotAssignByVulnerableShift()
{
    m_projectAssigned = AutomatedAlgorithmsBuildingSchedules::AssigningShiftsToTheMostVulnerableShift(m_projectShifts,
                                                                                                      m_projectWishes,
                                                                                                      m_projectWorkers,
                                                                                                      m_projectVacations,
                                                                                                      m_projectAssigned,
                                                                                                      m_projectSetup);

    slotModifyProject();
    slotUpdateProjectTree();
}

void MainWindow::slotAssignByVulnerableShiftWithoutWishes()
{
    m_projectAssigned = AutomatedAlgorithmsBuildingSchedules::AssigningShiftsToTheMostVulnerableShiftWithoutWishes(m_projectShifts,
                                                                                                      m_projectWishes,
                                                                                                      m_projectWorkers,
                                                                                                      m_projectVacations,
                                                                                                      m_projectAssigned,
                                                                                                      m_projectSetup);

    slotModifyProject();
    slotUpdateProjectTree();
}

void MainWindow::slotAssignOverTheNormWorkShift()
{
    QList<QString> notAssignedID;

    for (auto iter_shift = m_projectShifts.begin(); iter_shift != m_projectShifts.end(); iter_shift++)
    {
        auto iteratorOnAssigned = m_projectAssigned.find(iter_shift.key());
        if (iteratorOnAssigned == m_projectAssigned.end())
            notAssignedID.push_back(iter_shift.key());
    }

    bool m_ok;

    int shiftID = findFromTextID(QInputDialog::getItem(this,
                                        tr("Shift ID"),
                                        tr("Choise the shift ID:"),
                                        notAssignedID,
                                        0,
                                        false,
                                        &m_ok));

    if (!m_ok)
        return;

    auto iteratorOnShift = m_projectShifts.find(QString::number(shiftID));
    if (iteratorOnShift == m_projectShifts.end())
    {
        QMessageBox::critical(this,
                              tr("Error!"),
                              tr("This shift does not exist."));
        return;
    }

    int workerID = QInputDialog::getInt(this,
                                        tr("Worker ID"),
                                        tr("Enter the worker ID:"),
                                        1,
                                        1,
                                        65535,
                                        1,
                                        &m_ok);

    if (!m_ok)
        return;

    SingleAssignedShift currentAssign;
    currentAssign.ID_Shift = shiftID;
    currentAssign.ID_Worker = workerID;
    m_projectAssigned.insert(QString::number(currentAssign.ID_Shift), currentAssign);

    slotModifyProject();
    slotUpdateProjectTree();
}

void MainWindow::hideEvent(QHideEvent *event)
{
    dockWidgetShiftsVisible = ui->p_dockWidget_Shifts->isVisible();
    dockWidgetProjectTree = ui->p_dockWidget_ProjectTree->isVisible();

    QMainWindow::hideEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    ui->p_dockWidget_Shifts->setVisible(dockWidgetShiftsVisible);
    ui->p_dockWidget_ProjectTree->setVisible(dockWidgetProjectTree);

    QMainWindow::showEvent(event);
}

void MainWindow::slotUpdateCharts()
{
    auto wgt = centralWidget()->layout()->itemAt(0)->widget();

    if (dynamic_cast<QSplitter*>(wgt))
    {
        QSplitter *splitter = dynamic_cast<QSplitter*>(wgt);

        dynamic_cast<ShiftCharts*>(splitter->widget(0))->setRequirementsChart(m_projectRequirements);
        dynamic_cast<ShiftCharts*>(splitter->widget(1))->setShiftsChart(m_projectShifts);
        dynamic_cast<ShiftCharts*>(splitter->widget(2))->setDiffRequirementsAndShiftsChart(m_projectRequirements, m_projectShifts);

        return;
    }

    if (dynamic_cast<ShiftCharts*>(wgt))
    {
        ShiftCharts* chart = dynamic_cast<ShiftCharts*>(wgt);

        for (auto it : p_menuCentralWorkspace->actions())
        {
            if (!it->isChecked())
                continue;

            if (it->text().contains(tr("diff")))
            {
                chart->setDiffRequirementsAndShiftsChart(m_projectRequirements, m_projectShifts);
                return;
            }

            if (it->text().contains(tr("requirement")))
            {
                chart->setRequirementsChart(m_projectRequirements);
                return;
            }

            if (it->text().contains(tr("shift")))
            {
                chart->setShiftsChart(m_projectShifts);
                return;
            }
        }

        return;
    }
}
