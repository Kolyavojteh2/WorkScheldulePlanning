#ifndef VACATIONWIDGET_H
#define VACATIONWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QMap>
#include <QListWidget>
#include <QSignalMapper>
#include "../../src/XML_Parsing/SingleVacation.h"
#include "../../src/WorkersInformation/workersinformation.h"
#include "../../src/XML_Parsing/xml_parsing.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VacationWidget; }
QT_END_NAMESPACE

class VacationWidget : public QDialog
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

    bool isSaved = false;

    QString m_pathWorkersInformation;

    Vacations m_vacations;
    QString m_currentEditVacationName;

signals:
    void signalSendVacations(Vacations);

public slots:
    void slotAddVacation();
    void slotDeleteVacation();
    void slotEditVacation(QListWidgetItem *item);

    void slotUpdateData(const QString&);

    void slotSave();

    void slotSetPathWorkersInformation(const QString&);

    void slotCheckIDWorker();
    void slotChangedIDWorker(const QString&);

    void slotSetVacations(Vacations);

};
#endif // VACATIONWIDGET_H
