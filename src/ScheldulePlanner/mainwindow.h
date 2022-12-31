#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMap>
#include <QMessageBox>
#include <QSplitter>

#include "../../src/XML_Parsing/Positions.h"
#include "../../src/XML_Parsing/ProjectSetup.h"
#include "../../src/XML_Parsing/SingleRequirement.h"
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/XML_Parsing/SingleWish.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/AssignedShift.h"

#include "./workersprioritylists.h"
#include "./ShiftCharts/shiftcharts.h"
//#include "./FindWorkerDialog/findworkerdialog.h"
#include "./WorkerSearchResult/workersearchresult.h"
#include "./FindRequirements/findrequirements.h"
#include "./FindWishes/findwishes.h"
#include "./FindVacation/findvacation.h"
#include "./FindShifts/findshifts.h"
#include "./FindAssignedShifts/findassignedshifts.h"

#include "./StartApplicationWidget/startapplicationwidget.h"
#include "./CreatingProjectBasedOnExistingDialog/creatingprojectbasedonexistingdialog.h"

#include "./AutomatedAlgorithmsBuildingSchedules/automatedalgorithmsbuildingschedules.h"
#include "./DurationOfWorkShiftDialog/durationofworkshiftdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QMap<QString, Worker> Workers;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    QString projectPath();

private:
    void setupWidget();
    QMessageBox::StandardButton checkChangesInCurrentProject();
    bool askToSaveChanges();

    void clearCentralWidget();

    void clearProjectData();
    void updateDirectoryPath();

    int findFromTextID(const QString&);
    static void removeFilesInDir(const QString&);

    void loadExistingProject();

// Editing data
    //void editWorker(int id);
    void editWish(int id);
    void editShift(int id);

    static Requirements mergeRequirements(Requirements first, Requirements second);
    static int findLastRequirementID(const Requirements& list);
    static Wishes mergeWishes(Wishes first, Wishes second);
    static int findLastWishID(const Wishes& list);
    static int findLastShiftID(const Shifts&);
private:
    // UI
    Ui::MainWindow *ui;

    QMenu* p_menuView;
    QMenu* p_menuCentralWorkspace;

    QMenu* p_menuProject;
    QMenu* p_menuExport;

    QMenu* p_menuData;
    QMenu* p_menuWorker;
    QMenu* p_menuRequirements;
    QMenu* p_menuVacations;
    QMenu* p_menuWishes;

    QMenu* p_menuPlaning;

    QAction* p_showChartRequirements;
    QAction* p_showChartShifts;
    QAction* p_showChartDiffs;
    QAction* p_showCharts;

    QMenuBar* p_menuBar;

    // Paths
    QString m_projectPath;

    QString m_defaultProjectPath = "./projects/";
    QString m_workersPath;
    QString m_requirementsPath;
    QString m_wishesPath;
    QString m_positionsPath;
    QString m_vacationPath;
    QString m_shiftsPath;
    QString m_assignedShiftsPath;

    // Project data
    Positions m_projectPositions;
    ProjectSetup m_projectSetup;
    Requirements m_projectRequirements;
    Vacations m_projectVacations;
    Wishes m_projectWishes;
    Workers m_projectWorkers;
    Shifts m_projectShifts;
    AssignedShifts m_projectAssigned;

    //bool isOpenedProject = false;
    bool isModifiedProject = false;

    bool dockWidgetShiftsVisible = false;
    bool dockWidgetProjectTree = false;

protected:
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

public slots:
// Kits window
    void slotOpenStartWindow();
    void slotShowProjectsKit();

// Project
    void slotCreateNewProject();
    void slotCreateNewProjectBasedOnExisting();
    void slotOpenProject();
    void slotOpenProjectByName(const QString&);
    void slotSaveProject();
    void slotSaveAsProject();
    void slotCloseProject();

// Worker
    void slotCreateNewWorker();
    void slotDeleteWorker();
    void slotFindWorker();

// Data
    void slotOpenPositionManager();
    void slotOpenProjectConfiguration();
// Requirements
    void slotOpenRequirementsManager();
    void slotFindRequrements();
    void slotRemoveAllRequirements();

// Vacations
    void slotOpenVacationsManager();
    void slotFindVacations();
    void slotRemoveAllVacations();
// Wishes
    void slotOpenWishesManager();
    void slotFindWishes();
    void slotRemoveAllWishes();

    //void slotOpenShiftsManager();
// Shift
    void slotAddSingleShift();
    void slotDeleteSingleShift();
    void slotFindShifts();
    void slotRemoveAllShiftsAndAssignments();

// Assigned shift
    void slotAssignShift();
    void slotCancelAssignmentShift();
    void slotFindAssignedShifts();
    void slotRemoveAllAssignedShifts();
    void slotAssignOverTheNormWorkShift();

    void slotSetProjectPath(const QString&);

    void slotUpdateEnablingOfActions(bool);

    void slotUpdateProjectTree();
    void slotModifyProject();

// Automated Algorithms
    void slotMinStandart();
    void slotMinMax();
    void slotAssignByVulnerableShift();
    void slotAssignByVulnerableShiftWithoutWishes();

// Charts
    void slotChartRequirements();
    void slotChartShifts();
    void slotChartDiffs();
    void slotChartsWidget();
    void slotChangeCentralWorkspace();

    void slotUpdateCharts();

// Export
    void slotExportByPositions();
    void slotExportByWorkers();
    void slotExportByTime();

// Edit data
    void slotEditData(QTreeWidgetItem *item, int column);
    void slotEditWorker(int id);
    void slotChangeWorkersInformation(Worker);
    void slotEditShift(SingleShift);

    void slotGetWorker(Worker);
    void slotGetPositions(Positions);
    void slotGetConfiguration(ProjectSetup);
    void slotGetRequirements(Requirements);
    void slotGetVacations(Vacations);
    void slotGetWishes(Wishes);
    //void slotGetShifts(Shifts);
    void slotAddShift(SingleShift);
};
#endif // MAINWINDOW_H
