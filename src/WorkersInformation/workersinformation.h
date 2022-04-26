#ifndef WORKERSINFORMATION_H
#define WORKERSINFORMATION_H

#include <QWidget>
#include <QString>
#include <QMenu>
#include "../../src/XML_Parsing/Worker.h"
#include "../../src/XML_Parsing/Positions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WorkersInformation; }
QT_END_NAMESPACE

class WorkersInformation : public QWidget
{
    Q_OBJECT

public:
    WorkersInformation(QWidget *parent = nullptr);
    ~WorkersInformation();

private:
    Worker getInformationFromForm();
    void resetForm();
    void validateData();
    void openFile(const QString&);

    void setupWidget();

    void loadPositions();
    //int getLastId();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WorkersInformation *ui;
    Positions m_positions;

    QString m_filename;

    QString m_defaultPath;
    QString m_pathPositionsInformation;

    bool isModifiedFile = false;

    //int m_lastID = -1;

public slots:
    void slotCreateNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotAddPosition();
    void slotDeletePosition();

    void slotChangeWindowTitle(const QString&);

    void slotSetDefaulPath(const QString&);
    void slotSetPathPositionsInformation(const QString&);

    void slotShowNavigationButtons(bool showState);

    void slotModifyFile();
};
#endif // WORKERSINFORMATION_H
