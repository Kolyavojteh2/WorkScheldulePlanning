#ifndef SHIFTCHARTS_H
#define SHIFTCHARTS_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QDateTime>
#include <QSet>
#include <QMenu>
#include <QInputDialog>
#include <QColorDialog>
#include <QIcon>
#include <QPixmap>
#include <QtCharts>

#include "../../src/XML_Parsing/SingleRequirement.h"
#include "../../src/XML_Parsing/Shifts.h"
#include "./charttimerangedialog.h"

class ShiftCharts : public QWidget
{
    Q_OBJECT
public:
    explicit ShiftCharts(QWidget *parent = nullptr);
    QString chartTitle() const;
    ~ShiftCharts();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void setupWidget();

private:
    const int STANDART_SIZE_OF_ICON = 32;

    QMap<QDateTime, QMap<QString, int>> m_chartData;
    QList<QString> m_positionNames;

    QDateTimeAxis *axisX;
    QValueAxis *axisY;

    QList<QLineSeries*> m_series;

    QChart *p_chart;
    QChartView *p_chartView;

    bool inRangeDateTime(const QDateTime& value,
                         const QDateTime& min,
                         const QDateTime& max);

    int maxNumberOfWorkersInChart();

    const QVector<Qt::GlobalColor> m_colorsForSeries = {
        Qt::black,
        Qt::red,
        Qt::green,
        Qt::blue,
        Qt::cyan,
        Qt::magenta,
        Qt::yellow,
        Qt::gray,
        Qt::lightGray,
        Qt::darkRed,
        Qt::darkGreen,
        Qt::darkBlue,
        Qt::darkCyan,
        Qt::darkMagenta,
        Qt::darkYellow,
        Qt::darkGray
    };

public slots:
    void setChartTitle(const QString&);
    void setLabelAngle(int);

    void setHorizontalTickCount(int);

    void setMinDateTime(const QDateTime&);
    void setMaxDateTime(const QDateTime&);
    void setRange(const QDateTime& start, const QDateTime& end);

    void setPositionNames(const QList<QString>);

    void setRequirementsChart(const Requirements&);
    void setShiftsChart(const Shifts&);
    void setDiffRequirementsAndShiftsChart(const Requirements&, const Shifts&);

    void setVisibleSeries(const QString& nameSeries, bool visibility);
    void setSeriesColor(const QString& nameSeries, const QColor& color);

signals:
    void changedLabelAngle(int);
    void changedHorizontalTickCount(int);
    void changedVisibleSeries(const QString& nameSeries, bool visibility);
    void changedColorSeries(const QString& nameSeries, const QColor& color);
    void changedRange(const QDateTime& start, const QDateTime& end);

private slots:
    void contextMenuChangeRange();
    void contextMenuChangeAngle();
    void contextMenuChangeTitle();
    void contextMenuChangeTickCount();
    void contextMenuChangeVisibleSeries(bool);
    void contextMenuChangeSeriesColor();

};

#endif // SHIFTCHARTS_H
