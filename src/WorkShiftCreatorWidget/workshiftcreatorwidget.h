#ifndef WORKSHIFTCREATORWIDGET_H
#define WORKSHIFTCREATORWIDGET_H

#include <QWidget>
#include "../../src/XML_Parsing/ProjectSetup.h"
#include "../../src/XML_Parsing/SingleRequirement.h"
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/SingleWish.h"
#include <QList>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class WorkShiftCreatorWidget; }
QT_END_NAMESPACE

class WorkShiftCreatorWidget : public QWidget
{
    Q_OBJECT

public:
    WorkShiftCreatorWidget(QWidget *parent = nullptr);
    ~WorkShiftCreatorWidget();

public:
    QString defaultPath();

private:
    void setupWidget();
    void validateData();

    void resetData();
    void setInformationToForm(const SingleShift&);
    SingleShift getInformationFromForm();

    static int findLastRequirementID(const Requirements&);
    static int findLastShiftID(const Shifts&);
    static int findLastWishID(const Wishes&);
    static int findLastVacationID(const Vacations&);

    Requirements mergeRequirements(Requirements first, Requirements second);
    Vacations mergeVacations(Vacations first, Vacations second);
    Wishes mergeWishes(Wishes first, Wishes second);
    bool inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max);
    void openFile(const QString&);

    void updatePositionsComboBox();
    void updateWorkersComboBox();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WorkShiftCreatorWidget *ui;

    QString m_defaultPath;

    Shifts m_shifts;

    ProjectSetup m_setupEnterprise;
    Requirements m_requirements;
    Wishes m_wishes;
    Vacations m_vacations;
    QList<Worker> m_allWorkers;

    QString m_filename;
    QString m_currentEditShift;
    bool isModifiedFile = false;

    QString recomendedString = tr("(Recomended)");
    QString unwantedString = tr("(Unwanted)");

public slots:
    // Setup
    void slotSetEnterpriseSetup(const ProjectSetup&);

    // Requirements
    void slotAddRequirements(const Requirements&);
    void slotAddRequirementsFromFile(const QString&);
    void slotClearRequirements();

    // Vacations
    void slotAddVacations(const Vacations&);
    void slotAddVacationsFromFile(const QString&);
    void slotClearVacations();

    // Wishes
    void slotAddWishes(const Wishes&);
    void slotAddWishesFromFile(const QString&);
    void slotClearWishes();

    // Shift
    void slotAddShift();
    void slotDeleteShift();
    void slotEditShift(QListWidgetItem *item);

    void slotUpdateData(const QString& attributeName);
    void slotModifyFile();

    void slotCreateNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotSetDefaultPath(const QString&);

    void slotShowNavigationButtons(bool showState);
};
#endif // WORKSHIFTCREATORWIDGET_H
