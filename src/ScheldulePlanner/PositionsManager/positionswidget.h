#ifndef POSITIONSWIDGET_H
#define POSITIONSWIDGET_H

#include <QWidget>
#include <QDialog>
#include "../../src/XML_Parsing/Positions.h"
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PositionsWidget; }
QT_END_NAMESPACE

class PositionsWidget : public QDialog
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
    void resetData();

    int findLastNumberUnnamedPosition();

private:
    Ui::PositionsWidget *ui;

    Positions m_currentPositions;
    QString m_currentEditPositionName;

    bool isSaved = false;

signals:
    void signalSendPositions(Positions);

public slots:
    void slotSave();

    void slotAddPosition();
    void slotDeletePosition();
    void slotEditPosition(QListWidgetItem *item);

    void slotUpdateBrief();
    void slotUpdateFullName();
    void slotUpdateDescription();

    void slotSetPositionsData(Positions);
};
#endif // POSITIONSWIDGET_H
