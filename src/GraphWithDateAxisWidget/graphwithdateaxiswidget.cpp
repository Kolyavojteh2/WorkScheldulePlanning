#include "graphwithdateaxiswidget.h"
#include "ui_graphwithdateaxiswidget.h"
#include <QLibrary>
#include "../../src/XML_Parsing/xml_parsing.h"
#include <QVBoxLayout>
#include <QSet>
#include <QRandomGenerator>

GraphWithDateAxisWidget::GraphWithDateAxisWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphWithDateAxisWidget)
{
    ui->setupUi(this);
    setupWidget();
}

GraphWithDateAxisWidget::~GraphWithDateAxisWidget()
{
    delete ui;
}

void GraphWithDateAxisWidget::setupWidget()
{
    p_chart = new QChart;
    //_chart->legend()->hide();
    p_chart->setTitle("Test");

    axisX = new QDateTimeAxis;
    axisX->setTickCount(13);
    axisX->setFormat(/*"dd.MM.yyyy*/ QString("dd.MM hh:mm"));
    axisX->setTitleText("Date");
    axisX->setLabelsAngle(-60);
    p_chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis;
    //axisY->setLabelFormat("%i");
    axisY->setTitleText("Count");
    axisY->setTickCount(axisY->max());
    p_chart->addAxis(axisY, Qt::AlignLeft);

    p_chartView = new QChartView(p_chart);
    p_chartView->setRenderHint(QPainter::Antialiasing);
    //p_chartView->setRubberBand(QChartView::HorizontalRubberBand);

    this->setCentralWidget(p_chartView);

    //slotUpdateChart();

    connect(ui->p_button_updateGraph, SIGNAL(clicked()),
            this, SLOT(slotUpdateChart()));

    connect(ui->p_list_PositionsOnChart, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(slotChangeVisibleSeries(QListWidgetItem*)));
    connect(ui->p_button_CheckAll, SIGNAL(clicked()),
            this, SLOT(slotCheckAllPositions()));
    connect(ui->p_button_UncheckAll, SIGNAL(clicked()),
            this, SLOT(slotUncheckAllPositions()));

    //this->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    //connect(p_chartView, SIGNAL(customContextMenuRequested(const QPoint &)),
    //        this, SLOT(slotContextMenu(const QPoint &)));
}

void GraphWithDateAxisWidget::slotCheckAllPositions()
{
    for (auto it : p_series)
    {
        it->setVisible(true);
    }
    for (int i = 0; i < ui->p_list_PositionsOnChart->count(); i++)
    {
        ui->p_list_PositionsOnChart->item(i)->setCheckState(Qt::CheckState::Checked);
    }
}

void GraphWithDateAxisWidget::slotUncheckAllPositions()
{
    for (auto it : p_series)
    {
        it->setVisible(false);
    }
    for (int i = 0; i < ui->p_list_PositionsOnChart->count(); i++)
    {
        ui->p_list_PositionsOnChart->item(i)->setCheckState(Qt::CheckState::Unchecked);
    }
}

void GraphWithDateAxisWidget::slotChangeVisibleSeries(QListWidgetItem* item)
{
    for (auto it : p_series)
    {
        if (it->name() == item->text())
        {
            it->setVisible(item->checkState());
        }
    }
}

void GraphWithDateAxisWidget::createSeries()
{
    p_series.clear();

    for (auto it_position = m_positionNames.begin(); it_position != m_positionNames.end(); it_position++)
    {
        QLineSeries* series = new QLineSeries;
        series->setName(*it_position);

        for (auto it_data = m_chartData.begin(); it_data != m_chartData.end(); it_data++)
        {
            series->append(it_data.key().toMSecsSinceEpoch(), it_data.value().value(*it_position));
        }

        // Присвоєння випадкового кольору для лінії
        QRandomGenerator rng(QTime::currentTime().msec());
        series->setColor(QColor(rng.generate() % 256, rng.generate() % 256, rng.generate() % 256));

        p_series.push_back(series);
    }
}

int GraphWithDateAxisWidget::maxNumberOfWorkersOnPosition()
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

void GraphWithDateAxisWidget::slotUpdateChart()
{
    p_chart->removeAllSeries();

    p_chart->axes(Qt::Orientation::Horizontal).first()->setMin(m_minDate);
    p_chart->axes(Qt::Orientation::Horizontal).first()->setMax(m_maxDate);

    initChartData();
    m_positionNames = positionsInRequirements();

    ui->p_list_PositionsOnChart->clear();
    for (auto it : m_positionNames)
    {
        QListWidgetItem *item = new QListWidgetItem(it);
        item->setFlags(item->flags() | Qt::ItemFlag::ItemIsUserCheckable);
        item->setCheckState(Qt::CheckState::Checked);
        ui->p_list_PositionsOnChart->addItem(item);
    }

    createSeries();

    for (auto it : p_series)
    {
        p_chart->addSeries(it);
        it->attachAxis(axisX);
        it->attachAxis(axisY);
    }

    axisY->setMin(0);
    axisY->setMax(maxNumberOfWorkersOnPosition() + 1);
    axisY->setTickCount(axisY->max() + 1);
}

void GraphWithDateAxisWidget::slotSetMinDate(const QDateTime& date)
{
    m_minDate = date;
}

void GraphWithDateAxisWidget::slotSetMaxDate(const QDateTime& date)
{
    m_maxDate = date;
}

bool GraphWithDateAxisWidget::inRangeDateTime(const QDateTime& value, const QDateTime& min, const QDateTime& max)
{
    if (value >= min && value < max)
        return true;
    return false;
}

void GraphWithDateAxisWidget::initChartData()
{
    int secondsPerPeriod = 60;

    for (QDateTime it_time = m_minDate; it_time <= m_maxDate; it_time = it_time.addSecs(secondsPerPeriod))
    {
        QMap<QString, int> valuesPerTime;
        for (auto it_requirement = m_requirements.begin(); it_requirement != m_requirements.end(); it_requirement++)
        {
            if (inRangeDateTime(it_time, QDateTime::fromString(it_requirement->StartDate), QDateTime::fromString(it_requirement->EndDate)))
            {
                if (valuesPerTime.find(it_requirement.value().Position) == valuesPerTime.end())
                {
                    valuesPerTime.insert(it_requirement.value().Position, it_requirement.value().NumberWorkers);
                }
                else
                {
                    valuesPerTime[it_requirement.value().Position] += it_requirement.value().NumberWorkers;
                }
            }
        }

        m_chartData.insert(it_time, valuesPerTime);
    }
}

void GraphWithDateAxisWidget::slotClearRequirements()
{
    m_requirements.clear();
    slotUpdateChart();
}

int GraphWithDateAxisWidget::findLastRequirementID()
{
    int result = 0;
    for (auto it = m_requirements.begin(); it != m_requirements.end(); it++)
    {
        if (result < it.value().ID)
            result = it.value().ID;
    }

    return result;
}

void GraphWithDateAxisWidget::slotAddRequirementsFromFile(const QString& filename)
{
    if (filename.isEmpty())
        return;

    // Завантаження даних про робітника із файлу
    typedef Requirements (*func_readRequirements)(const QString&);
    QLibrary xmlParser("XML_Parsing");
    func_readRequirements readRequirements = (func_readRequirements)xmlParser.resolve("read_RequirementsFromFile");

    Requirements fileRequirement = readRequirements(filename);

    for (auto it = fileRequirement.begin(); it != fileRequirement.end();)
    {
        if (m_requirements.find(QString::number(it.value().ID)) != m_requirements.end())
        {
            int newID = findLastRequirementID();
            m_requirements.insert(QString::number(newID), it.value());
            m_requirements[QString::number(newID)].ID = newID;

            auto delete_iterator = it;
            it++;
            fileRequirement.erase(delete_iterator);
        }
        else
        {
            m_requirements.insert(it.key(), it.value());

            auto delete_iterator = it;
            it++;
            fileRequirement.erase(delete_iterator);
        }
    }
}

QList<QString> GraphWithDateAxisWidget::positionsInRequirements()
{
   QSet<QString> positionsSet;

   for (auto it_requirements = m_requirements.begin(); it_requirements != m_requirements.end(); it_requirements++)
   {
       positionsSet.insert(it_requirements.value().Position);
       //for (auto it_values = it_requirements.value())
   }

   return positionsSet.toList();
}

void GraphWithDateAxisWidget::slotContextMenu(const QPoint& pos)
{
    QMenu *context = new QMenu("My", this);
    context->addAction(new QAction("Some", this));
    //context->exec(this->mapToGlobal(pos));
    context->popup(pos);
}

void GraphWithDateAxisWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *context = new QMenu("My", this);
    context->addAction(new QAction("Some", this));
    context->exec(this->mapToGlobal(event->pos()));
}

