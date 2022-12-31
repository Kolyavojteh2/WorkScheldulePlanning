#ifndef WORKERSINFORMATION_H
#define WORKERSINFORMATION_H

#include <QWidget>
#include <QString>
#include <QMenu>
#include <QDialog>
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/Positions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WorkersInformation; }
QT_END_NAMESPACE

class WorkersInformation : public QDialog
{
    Q_OBJECT

public:
    WorkersInformation(QWidget *parent = nullptr);
    ~WorkersInformation();

private:
    Worker getInformationFromForm();
    void validateData();
    void setupWidget();

    void loadPositions();

private:
    Ui::WorkersInformation *ui;
    Positions m_positions;

    QString m_defaultPath;
    QString m_pathPositionsInformation;

private slots:
    void slotAddPosition();
    void slotDeletePosition();

public slots:
    void slotSave();

    void slotChangeWindowTitle(const QString&);
    void slotSetPathPositionsInformation(const QString&);
    void slotDisableID();
    void slotChangeButtonText(const QString&);

    void slotSetWorkerData(Worker);
signals:
    void signalSendWorker(Worker);
};
#endif // WORKERSINFORMATION_H
