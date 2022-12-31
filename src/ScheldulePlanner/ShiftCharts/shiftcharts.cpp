#include "shiftcharts.h"

#include <QVBoxLayout>


ShiftCharts::ShiftCharts(QWidget *parent) : QWidget(parent)
{
    setupWidget();
}

QString ShiftCharts::chartTitle() const
{
    return p_chart->title();
}

ShiftCharts::~ShiftCharts()
{
    for (auto it : m_series)
        delete it;

    m_series.clear();
}

void ShiftCharts::setupWidget()
{
    p_chart = new QChart;

    auto titleFont = p_chart->titleFont();
    titleFont.setBold(true);
    titleFont.setPixelSize(21);
    p_chart->setTitleFont(titleFont);

    axisX = new QDateTimeAxis;
    axisX->setTitleText(tr("Time"));
    axisX->setFormat(QString("dd.MM hh:mm"));
    p_chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis;
    axisY->setTitleText(tr("Count of workers"));
    p_chart->addAxis(axisY, Qt::AlignLeft);

    p_chartView = new QChartView(p_chart);
    p_chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *p_layout = new QVBoxLayout;
    p_layout->addWidget(p_chartView);
    this->setLayout(p_layout);

    this->setGeometry(0, 0, 1000, 300);

    connect(this, SIGNAL(changedRange(const QDateTime&, const QDateTime&)),
            this, SLOT(setRange(const QDateTime&, const QDateTime&)));
    connect(this, SIGNAL(changedHorizontalTickCount(int)),
            this, SLOT(setHorizontalTickCount(int)));
    connect(this, SIGNAL(changedLabelAngle(int)),
            this, SLOT(setLabelAngle(int)));
    connect(this, SIGNAL(changedVisibleSeries(const QString&, bool)),
            this, SLOT(setVisibleSeries(const QString&, bool)));
    connect(this, SIGNAL(changedColorSeries(const QString&, const QColor&)),
            this, SLOT(setSeriesColor(const QString&, const QColor&)));
}

void ShiftCharts::setChartTitle(const QString& str)
{
    p_chart->setTitle(str);
    this->setWindowTitle(str);
}

void ShiftCharts::setLabelAngle(int angle)
{
    axisX->setLabelsAngle(angle);
}

void ShiftCharts::setMinDateTime(const QDateTime& dateTime)
{
    p_chart->axes(Qt::Orientation::Horizontal).first()->setMin(dateTime);
}

void ShiftCharts::setMaxDateTime(const QDateTime& dateTime)
{
    p_chart->axes(Qt::Orientation::Horizontal).first()->setMax(dateTime);
}

void ShiftCharts::setPositionNames(const QList<QString> list)
{
    m_positionNames = list;
}

void ShiftCharts::setRequirementsChart(const Requirements& requirements)
{
    const int secondsPerPeriod = 60;

    QSet<QString> positions;

    for (QDateTime it_time = axisX->min(); it_time != axisX->max(); it_time = it_time.addSecs(secondsPerPeriod))
    {
        QMap<QString, int> valuesPerTime;
        for (auto it_requirement = requirements.begin(); it_requirement != requirements.end(); it_requirement++)
        {
            if (inRangeDateTime(it_time, QDateTime::fromString(it_requirement->StartDate), QDateTime::fromString(it_requirement->EndDate)))
            {
                valuesPerTime[it_requirement.value().Position] += it_requirement.value().NumberWorkers;
                positions.insert(it_requirement.value().Position);
            }
        }

        m_chartData.insert(it_time, valuesPerTime);
    }

    m_positionNames = positions.toList();

    m_series.clear();
    p_chart->removeAllSeries();

    int counterSeries = 0;
    for (auto it_position : m_positionNames)
    {
        QLineSeries* series = new QLineSeries;
        series->setName(it_position);
        series->setColor(m_colorsForSeries[counterSeries % m_colorsForSeries.size()]);

        for (auto it_data = m_chartData.begin(); it_data != m_chartData.end(); it_data++)
        {
            series->append(it_data.key().toMSecsSinceEpoch(), it_data.value().value(it_position));
        }

        m_series.push_back(series);
    }

    for (auto it : m_series)
    {
        p_chart->addSeries(it);
        it->attachAxis(axisX);
        it->attachAxis(axisY);
    }

    axisY->setMin(0);
    axisY->setMax(maxNumberOfWorkersInChart() + 1);
    axisY->setTickCount(axisY->max() + 1);
}

void ShiftCharts::setShiftsChart(const Shifts& shifts)
{
    const int secondsPerPeriod = 60;

    QSet<QString> positions;

    for (QDateTime it_time = axisX->min(); it_time != axisX->max(); it_time = it_time.addSecs(secondsPerPeriod))
    {
        QMap<QString, int> valuesPerTime;
        for (auto it_singleShift = shifts.begin(); it_singleShift != shifts.end(); it_singleShift++)
        {
            if (inRangeDateTime(it_time, QDateTime::fromString(it_singleShift->StartDate), QDateTime::fromString(it_singleShift->EndDate)))
            {
                valuesPerTime[it_singleShift.value().Position] += 1/*it_singleShift.value().NumberWorkers*/;
                positions.insert(it_singleShift.value().Position);
            }
        }

        m_chartData.insert(it_time, valuesPerTime);
    }

    m_positionNames = positions.toList();

    m_series.clear();
    p_chart->removeAllSeries();

    int counterSeries = 0;
    for (auto it_position : m_positionNames)
    {
        QLineSeries* series = new QLineSeries;
        series->setName(it_position);
        series->setColor(m_colorsForSeries[counterSeries % m_colorsForSeries.size()]);

        for (auto it_data = m_chartData.begin(); it_data != m_chartData.end(); it_data++)
        {
            series->append(it_data.key().toMSecsSinceEpoch(), it_data.value().value(it_position));
        }

        m_series.push_back(series);
    }

    for (auto it : m_series)
    {
        p_chart->addSeries(it);
        it->attachAxis(axisX);
        it->attachAxis(axisY);
    }

    axisY->setMin(0);
    axisY->setMax(maxNumberOfWorkersInChart() + 1);
    axisY->setTickCount(axisY->max() + 1);
}

void ShiftCharts::setDiffRequirementsAndShiftsChart(const Requirements& requirements, const Shifts& shifts)
{
    const int secondsPerPeriod = 60;

    QSet<QString> positions;

    for (QDateTime it_time = axisX->min(); it_time != axisX->max(); it_time = it_time.addSecs(secondsPerPeriod))
    {
        QMap<QString, int> valuesPerTime;
        for (auto it_requirement = requirements.begin(); it_requirement != requirements.end(); it_requirement++)
        {
            if (inRangeDateTime(it_time, QDateTime::fromString(it_requirement->StartDate), QDateTime::fromString(it_requirement->EndDate)))
            {
                valuesPerTime[it_requirement.value().Position] += it_requirement.value().NumberWorkers;
                positions.insert(it_requirement.value().Position);
            }
        }

        m_chartData.insert(it_time, valuesPerTime);
    }

    for (QDateTime it_time = axisX->min(); it_time != axisX->max(); it_time = it_time.addSecs(secondsPerPeriod))
    {
        QMap<QString, int> valuesPerTime;
        for (auto it_singleShift = shifts.begin(); it_singleShift != shifts.end(); it_singleShift++)
        {
            if (inRangeDateTime(it_time, QDateTime::fromString(it_singleShift->StartDate), QDateTime::fromString(it_singleShift->EndDate)))
            {
                m_chartData[it_time][it_singleShift.value().Position] -= 1;
                //valuesPerTime[it_singleShift.value().Position] -= 1/*it_singleShift.value().NumberWorkers*/;
                positions.insert(it_singleShift.value().Position);
            }
        }

        //m_chartData.insert(it_time, valuesPerTime);
    }


///////////////////////////////
    m_positionNames = positions.toList();

    m_series.clear();
    p_chart->removeAllSeries();

    int counterSeries = 0;
    for (auto it_position : m_positionNames)
    {
        QLineSeries* series = new QLineSeries;
        series->setName(it_position);
        series->setColor(m_colorsForSeries[counterSeries % m_colorsForSeries.size()]);

        for (auto it_data = m_chartData.begin(); it_data != m_chartData.end(); it_data++)
        {
            series->append(it_data.key().toMSecsSinceEpoch(), it_data.value().value(it_position));
        }

        m_series.push_back(series);
    }

    for (auto it : m_series)
    {
        p_chart->addSeries(it);
        it->attachAxis(axisX);
        it->attachAxis(axisY);
    }

    axisY->setMin(0);
    axisY->setMax(maxNumberOfWorkersInChart() + 1);
    axisY->setTickCount(axisY->max() + 1);
}

int ShiftCharts::maxNumberOfWorkersInChart()
{
    int maxNumber = 0;
    for (auto it = m_chartData.begin(); it != m_chartData.end(); it++)
    {
        for (auto it_pos = it.value().begin(); it_pos != it.value().end(); it_pos++)
        {
            if (maxNumber < it_pos.value())
                maxNumber = it_pos.value();
        }
    }

    return maxNumber;
}

bool ShiftCharts::inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max)
{
    if (value >= min && value < max)
        return true;
    return false;
}

void ShiftCharts::setHorizontalTickCount(int n)
{
    axisX->setTickCount(n);
}

void ShiftCharts::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *p_contextMenu = new QMenu;

    // Date range
    QAction *p_dateRange = new QAction(tr("Сhange date &range..."));
    connect(p_dateRange, SIGNAL(triggered()),
            this, SLOT(contextMenuChangeRange()));
    p_contextMenu->addAction(p_dateRange);

    // Angle label
    QAction *p_labelAngle = new QAction(tr("Change the &angle of the text..."));
    connect(p_labelAngle, SIGNAL(triggered()),
            this, SLOT(contextMenuChangeAngle()));
    p_contextMenu->addAction(p_labelAngle);

    // Angle label
    QAction *p_changeTitle = new QAction(tr("Change the &title of chart..."));
    connect(p_changeTitle, SIGNAL(triggered()),
            this, SLOT(contextMenuChangeTitle()));
    p_contextMenu->addAction(p_changeTitle);

    // Tick count
    QAction *p_changeTickCount = new QAction(tr("Change the t&ick count..."));
    connect(p_changeTickCount, SIGNAL(triggered()),
            this, SLOT(contextMenuChangeTickCount()));
    p_contextMenu->addAction(p_changeTickCount);

    // Choise series
    QMenu *p_seriesMenu = new QMenu(tr("Choose visible series"));
    for (auto it : m_series)
    {
        QAction *p_seriesAction = new QAction(it->name());
        p_seriesAction->setCheckable(true);
        p_seriesAction->setChecked(it->isVisible());

        connect(p_seriesAction, SIGNAL(triggered(bool)),
                this, SLOT(contextMenuChangeVisibleSeries(bool)));
        p_seriesMenu->addAction(p_seriesAction);
    }
    if (m_series.isEmpty())
        p_seriesMenu->setEnabled(false);
    p_contextMenu->addMenu(p_seriesMenu);

    // Choise series color
    QMenu *p_seriesColorMenu = new QMenu(tr("Choose the color of the series"));
    for (auto it : m_series)
    {
        QAction *p_seriesAction = new QAction(it->name());
        QPixmap iconColor(STANDART_SIZE_OF_ICON, STANDART_SIZE_OF_ICON);
        iconColor.fill(it->color());
        p_seriesAction->setIcon(QIcon(iconColor));

        connect(p_seriesAction, SIGNAL(triggered()),
                this, SLOT(contextMenuChangeSeriesColor()));
        p_seriesColorMenu->addAction(p_seriesAction);
    }
    if (m_series.isEmpty())
        p_seriesColorMenu->setEnabled(false);
    p_contextMenu->addMenu(p_seriesColorMenu);

    p_contextMenu->exec(event->globalPos());
}

void ShiftCharts::contextMenuChangeSeriesColor()
{
    QAction *series = dynamic_cast<QAction *>(sender());

    QColor oldColor(Qt::GlobalColor::black);
    QLineSeries* p_series = nullptr;
    for (auto it : m_series)
    {
        if (it->name() == series->text())
        {
            oldColor = it->color();
            p_series = it;
            break;
        }
    }

    if (p_series == nullptr)
        return;

    QColor newColor = QColorDialog::getColor(oldColor,
                                             this,
                                             tr("Enter the color of the series %1").arg(series->text()));

    emit changedColorSeries(p_series->name(), newColor);
    //p_series->setColor(newColor);
}

void ShiftCharts::setSeriesColor(const QString& nameSeries, const QColor& color)
{
    QLineSeries* p_series = nullptr;
    for (auto it : m_series)
    {
        if (it->name() == nameSeries)
        {
            p_series = it;
            break;
        }
    }

    if (p_series == nullptr)
        return;

    p_series->setColor(color);
}

void ShiftCharts::contextMenuChangeVisibleSeries(bool visible)
{
    auto seriesAction = dynamic_cast<QAction *>(sender());
    QLineSeries *series = nullptr;
    for (auto it : m_series)
    {
        if (it->name() == seriesAction->text())
        {
            series = it;
            break;
        }
    }

    if (!series)
        return;

    emit changedVisibleSeries(series->name(), visible);
    //series->setVisible(visible);
}

void ShiftCharts::setVisibleSeries(const QString& nameSeries, bool visibility)
{
    QLineSeries *series = nullptr;
    for (auto it : m_series)
    {
        if (it->name() == nameSeries)
        {
            series = it;
            break;
        }
    }

    if (!series)
        return;

    series->setVisible(visibility);
}

void ShiftCharts::contextMenuChangeTitle()
{
    bool m_ok;
    QString str = QInputDialog::getText(this,
                                      tr("The title of the chart"),
                                      tr("Enter the title:"),
                                      QLineEdit::EchoMode::Normal,
                                      "",
                                      &m_ok);

    if (!m_ok)
        return;

    setChartTitle(str);
}

void ShiftCharts::contextMenuChangeRange()
{
    QDateTime startRange = axisX->min();
    QDateTime endRange = axisX->max();
    ChartTimeRangeDialog* dialog = new ChartTimeRangeDialog(startRange, endRange, this);
    dialog->show();

    connect(dialog, SIGNAL(sendRange(const QDateTime&, const QDateTime&)),
            this, SIGNAL(changedRange(const QDateTime&, const QDateTime&)));

    //connect(dialog, SIGNAL(sendStartDate(const QDateTime&)),
    //        this, SLOT(setMinDateTime(const QDateTime&)));
    //connect(dialog, SIGNAL(sendEndDate(const QDateTime&)),
    //        this, SLOT(setMaxDateTime(const QDateTime&)));
}

void ShiftCharts::contextMenuChangeAngle()
{
    bool m_ok;
    int angle = QInputDialog::getInt(this,
                                     tr("The angle of the text"),
                                     tr("Set the angle of the text:"),
                                     0,
                                     -179,
                                     180,
                                     1,
                                     &m_ok);

    if (!m_ok)
        return;

    emit changedLabelAngle(angle);
}

void ShiftCharts::contextMenuChangeTickCount()
{
    bool m_ok;
    int counts = QInputDialog::getInt(this,
                                     tr("Tick count"),
                                     tr("Enter the tick count of the time axis:"),
                                     axisX->tickCount(),
                                     0,
                                     65535,
                                     1,
                                     &m_ok);

    if (!m_ok)
        return;

    emit changedHorizontalTickCount(counts);
}

void ShiftCharts::setRange(const QDateTime& start, const QDateTime& end)
{
    setMinDateTime(start);
    setMaxDateTime(end);
}

