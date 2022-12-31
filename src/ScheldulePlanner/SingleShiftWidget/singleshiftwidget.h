#ifndef SINGLESHIFTWIDGET_H
#define SINGLESHIFTWIDGET_H

#include <QDialog>
#include "../../src/XML_Parsing/Shifts.h"
#include "../../src/XML_Parsing/Positions.h"

namespace Ui {
class SingleShiftWidget;
}

class SingleShiftWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SingleShiftWidget(QWidget *parent = nullptr);
    ~SingleShiftWidget();

private:
    void setInformationToForm(const SingleShift&);
    SingleShift getInformationFromForm();

    void updatePositionsComboBox();

private:
    Ui::SingleShiftWidget *ui;
    Positions m_positions;

signals:
    void singnalSendShift(SingleShift);

private slots:
    void slotCheckDates(const QDateTime&);
    void slotEnablingOK(int);

public slots:
    void slotSetSingleShift(SingleShift);
    void slotSetPositions(Positions);

    void slotSetShiftID(int);
    void slotChangeTextOKButton(const QString&);

};

#endif // SINGLESHIFTWIDGET_H
