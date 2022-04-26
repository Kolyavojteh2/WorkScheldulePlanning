#ifndef POSITIONSWIDGET_H
#define POSITIONSWIDGET_H

#include <QWidget>
#include "../../src/XML_Parsing/Positions.h"
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PositionsWidget; }
QT_END_NAMESPACE

class PositionsWidget : public QWidget
{
    Q_OBJECT

public:
    PositionsWidget(QWidget *parent = nullptr);
    ~PositionsWidget();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupWidget();
    Position getInformationFromForm();
    void setInformationToForm(const Position& info);
    void openFile(const QString & filename);
    void resetData();
    void validateData();

    int findLastNumberUnnamedPosition();

private:
    Ui::PositionsWidget *ui;

    QString m_defaultPath;
    QString m_filename;
    bool isModifiedFile = false;

    Positions m_currentPositions;
    QString m_currentEditPositionName;

public slots:
    void slotCreateNew();
    void slotOpen();
    void slotSave();
    void slotSaveAs();

    void slotAddPosition();
    void slotDeletePosition();
    void slotEditPosition(QListWidgetItem *item);

    void slotUpdateBrief();
    void slotUpdateFullName();
    void slotUpdateDescription();

    void slotModifyFile();

    void slotSetDefaulPath(const QString&);

    void slotShowNavigationButtons(bool showState);
};
#endif // POSITIONSWIDGET_H
