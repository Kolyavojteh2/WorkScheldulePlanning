#ifndef VACATIONWIDGET_H
#define VACATIONWIDGET_H

#include <QWidget>
#include <QMap>
#include <QListWidget>
#include <QSignalMapper>
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/WorkersInformation/workersinformation.h"
#include "../../src/XML_Parsing/xml_parsing.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VacationWidget; }
QT_END_NAMESPACE

class VacationWidget : public QWidget
{
    Q_OBJECT

public:
    VacationWidget(QWidget *parent = nullptr);
    ~VacationWidget();

private:
    void setupWidget();
    void validateData();

    void resetData();
    SingleVacation getInformationFromForm();
    void setInformationToForm(const SingleVacation&);

    int findLastIDInList();
    void openFile(const QString& filename);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::VacationWidget *ui;
    QSignalMapper *p_editingMapper;

    QString m_filename;
    bool isModifiedFile = false;

    QString m_pathWorkersInformation;
    QString m_defaultPath;

    Vacations m_vacations;
    QString m_currentEditVacationName;


public slots:
    void slotAddVacation();
    void slotDeleteVacation();
    void slotEditVacation(QListWidgetItem *item);

    void slotUpdateData(const QString&);

    void slotCreateNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotModifyFile();

    void slotShowNavigationButtons(bool showState);

    void slotSetPathWorkersInformation(const QString&);
    void slotSetDefaulPath(const QString&);

    void slotCheckIDWorker();
    void slotChangedIDWorker(const QString &);

};
#endif // VACATIONWIDGET_H
