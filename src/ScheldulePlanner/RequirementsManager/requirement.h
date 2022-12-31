#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <QWidget>
#include <QDialog>
#include "../../src/XML_Parsing/SingleRequirement.h"
#include <QListWidget>
#include <QSignalMapper>
#include "../../src/XML_Parsing/Positions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Requirement; }
QT_END_NAMESPACE

class RequirementWidget : public QDialog
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

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Requirement *ui;

    Positions m_positions;
    Requirements m_requirements;
    QString m_currentEditRequirement;

    bool isSaved = false;

    QSignalMapper *p_editingMapper;

signals:
    void signalSendRequirements(Requirements);

public slots:
    void slotAddRequirement();
    void slotDeleteRequirement();
    void slotEditRequirement(QListWidgetItem *item);

    void slotSetCurrentAndEditRequirement(int id);

    void slotChangedPosition(int);

    void slotSave();

    void slotUpdateData(const QString&);

    void slotSetRequirements(Requirements);
    void slotSetPositions(Positions);
};
#endif // REQUIREMENT_H
