#ifndef DURATIONOFWORKSHIFTDIALOG_H
#define DURATIONOFWORKSHIFTDIALOG_H

#include <QDialog>
#include "../../src/XML_Parsing/DurationsOfWorkShift.h"

namespace Ui {
class DurationOfWorkShiftDialog;
}

class DurationOfWorkShiftDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DurationOfWorkShiftDialog(QWidget *parent = nullptr);
    ~DurationOfWorkShiftDialog();

private:
    Ui::DurationOfWorkShiftDialog *ui;

signals:
    void EnteredDurations(const DurationsOfWorkShift&);

private slots:
    void checkTimes();
    void prepareBeforeSendData();
};

#endif // DURATIONOFWORKSHIFTDIALOG_H
