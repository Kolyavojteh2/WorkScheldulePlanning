#ifndef CHARTTIMERANGEDIALOG_H
#define CHARTTIMERANGEDIALOG_H

#include <QDialog>
#include "../../src/XML_Parsing/ProjectSetup.h"

namespace Ui {
class ChartTimeRangeDialog;
}

class ChartTimeRangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChartTimeRangeDialog(const QDateTime& start,
                                  const QDateTime& end,
                                  QWidget *parent = nullptr);
    ~ChartTimeRangeDialog();

private:
    Ui::ChartTimeRangeDialog *ui;

    ProjectSetup m_config;

signals:
    void sendStartDate(const QDateTime&);
    void sendEndDate(const QDateTime&);
    void sendRange(const QDateTime&, const QDateTime&);

private slots:
    void changedDateTime(const QDateTime &);
    void sendData();
};

#endif // CHARTTIMERANGEDIALOG_H
