#ifndef GRAPHWITHDATEAXISWIDGET_H
#define GRAPHWITHDATEAXISWIDGET_H

#include <QMainWindow>
#include <QtCharts>

#include <QMap>
#include <QString>
#include <QList>
#include <QMenu>

#include "../../src/XML_Parsing/SingleRequirement.h"
#include <QLineSeries>

QT_BEGIN_NAMESPACE
namespace Ui { class GraphWithDateAxisWidget; }
QT_END_NAMESPACE

class GraphWithDateAxisWidget : public QMainWindow
{
    Q_OBJECT

public:
    GraphWithDateAxisWidget(QWidget *parent = nullptr);
    ~GraphWithDateAxisWidget();

private:
    void setupWidget();

    void initChartData();
    void createSeries();

    bool inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max);
    int findLastRequirementID();
    int maxNumberOfWorkersOnPosition();
    QList<QString> positionsInRequirements();

private:
    Ui::GraphWithDateAxisWidget *ui;

    Requirements m_requirements;

    QMap<QDateTime, QMap<QString, int>> m_chartData;
    QDateTime m_minDate;
    QDateTime m_maxDate;

    QVector<QLineSeries*> p_series;
    QDateTimeAxis *axisX;
    QValueAxis *axisY;

    QList<QString> m_positionNames;
    QChart *p_chart;
    QChartView *p_chartView;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;


public slots:
    void slotSetMinDate(const QDateTime&);
    void slotSetMaxDate(const QDateTime&);

    void slotClearRequirements();
    void slotAddRequirementsFromFile(const QString&);

    void slotUpdateChart();

    void slotChangeVisibleSeries(QListWidgetItem*);
    void slotCheckAllPositions();
    void slotUncheckAllPositions();

    void slotContextMenu(const QPoint &);
};
#endif // GRAPHWITHDATEAXISWIDGET_H
