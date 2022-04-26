#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <QWidget>
#include "../../src/XML_Parsing/SingleRequirement.h"
#include <QListWidget>
#include <QSignalMapper>
#include "../../src/XML_Parsing/Positions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Requirement; }
QT_END_NAMESPACE

class RequirementWidget : public QWidget
{
    Q_OBJECT

public:
    RequirementWidget(QWidget *parent = nullptr);
    ~RequirementWidget();

private:
    void setupWidget();
    void validateData();

    void resetData();
    SingleRequirement getInformationFromForm();
    void setInformationToForm(const SingleRequirement&);

    int findLastIDInList();
    void openFile(const QString& filename);

    void loadPositions();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Requirement *ui;

    Positions m_positions;
    Requirements m_requirements;
    QString m_currentEditRequirement;

    QString m_pathPositionsInformation;

    QString m_filename;
    bool isModifiedFile = false;

    QSignalMapper *p_editingMapper;

public slots:
    void slotAddRequirement();
    void slotDeleteRequirement();
    void slotEditRequirement(QListWidgetItem *item);

    void slotModifyFile();
    void slotChangedPosition(int);

    void slotCreateNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotUpdateData(const QString&);

    void slotSetPathPositionsInformation(const QString&);

    void slotShowNavigationButtons(bool showState);
};
#endif // REQUIREMENT_H
