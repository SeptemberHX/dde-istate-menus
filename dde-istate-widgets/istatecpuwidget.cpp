#include "istatecpuwidget.h"
#include "ui_istatecpuwidget.h"

IstateCpuWidget::IstateCpuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IstateCpuWidget)
{
    ui->setupUi(this);

    this->cpuIdleColor = QColor("#404244");
    this->cpuUserColor = QColor("#5BBDF8");
    this->cpuUserAreaColor = QColor("#811D18");
    this->cpuSystemColor = QColor("#EA654D");
    this->cpuSystemAreaColor = QColor("#296085");

    this->loadIdleColor = QColor("#9C9CA0");
    this->loadUserColor = this->cpuUserColor;
    this->loadSystemColor = this->cpuSystemColor;

    ui->cpuIdleColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->cpuIdleColor.name()));
    ui->cpuSystemColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->cpuSystemColor.name()));
    ui->cpuUserColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->cpuUserColor.name()));

    initCpuChart();
    initLoadChart();
}

IstateCpuWidget::~IstateCpuWidget()
{
    delete ui;
}

void IstateCpuWidget::initCpuChart() {
    this->m_cpuChart = new QChart();

    this->m_userSeries = new QLineSeries();
    this->m_userSeries->setPen(QPen(this->cpuUserColor, 2));
    this->m_userSeries->setBrush(Qt::NoBrush);
    this->m_cpuChart->addSeries(this->m_userSeries);

    this->m_systemSeries = new QLineSeries();
    this->m_systemSeries->setPen(QPen(this->cpuSystemColor, 2));
    this->m_systemSeries->setBrush(Qt::NoBrush);
    this->m_cpuChart->addSeries(this->m_systemSeries);

    this->m_cpuZeroSeries = new QLineSeries();
    this->m_cpuZeroSeries->setPen(Qt::NoPen);
    this->m_cpuZeroSeries->setBrush(Qt::NoBrush);
    this->m_cpuChart->addSeries(this->m_cpuZeroSeries);

    // enable the legend and move it to bottom
    this->m_cpuChart->legend()->setVisible(false);

    // set the default range of the axes and hide the axes, grid line, and label
    this->m_cpuChart->createDefaultAxes();
    this->m_cpuChart->axisX()->setRange(0, this->maxHistorySize - 1);
    this->m_cpuChart->axisX()->setGridLineVisible(false);
    this->m_cpuChart->axisX()->setLabelsVisible(false);
    this->m_cpuChart->axisX()->setVisible(false);

    this->m_cpuChart->axisY()->setRange(0, 100);
    this->m_cpuChart->axisY()->setGridLineVisible(false);
    this->m_cpuChart->axisY()->setLabelsVisible(false);
    this->m_cpuChart->axisY()->setVisible(false);

    this->m_cpuChart->setMargins(QMargins(0, 0, 0, 0));
    this->m_cpuChart->layout()->setContentsMargins(0, 0, 0, 0);
    this->m_cpuChart->setBackgroundBrush(QBrush(this->cpuIdleColor));
    this->m_cpuChart->setBackgroundRoundness(0);

    ui->cpuChartView->setChart(this->m_cpuChart);
    ui->cpuChartView->setContentsMargins(0, 0, 0, 0);
    ui->cpuChartView->setRenderHint(QPainter::Antialiasing);
}

void IstateCpuWidget::initLoadChart() {
    this->m_loadChart = new QChart();

    this->m_loadUserSeries = new QLineSeries();
    this->m_loadUserSeries->setPen(QPen(this->loadUserColor, 2));
    this->m_loadUserSeries->setBrush(Qt::NoBrush);
    this->m_loadChart->addSeries(this->m_loadUserSeries);

    this->m_loadSystemSeries = new QLineSeries();
    this->m_loadSystemSeries->setPen(QPen(this->loadSystemColor, 2));
    this->m_loadSystemSeries->setBrush(Qt::NoBrush);
    this->m_loadChart->addSeries(this->m_loadSystemSeries);

    this->m_loadIdleSeries = new QLineSeries();
    this->m_loadIdleSeries->setPen(QPen(this->loadIdleColor, 2));
    this->m_loadIdleSeries->setBrush(Qt::NoBrush);
    this->m_loadChart->addSeries(this->m_loadIdleSeries);

    this->m_loadChart->legend()->setVisible(false);

    // set the default range of the axes and hide the axes, grid line, and label
    this->m_loadChart->createDefaultAxes();
    this->m_loadChart->axisX()->setRange(0, this->maxHistorySize - 1);
    this->m_loadChart->axisX()->setGridLineVisible(false);
    this->m_loadChart->axisX()->setLabelsVisible(false);
    this->m_loadChart->axisX()->setVisible(false);

    this->m_loadChart->axisY()->setRange(0, 10);
    this->m_loadChart->axisY()->setGridLineVisible(false);
    this->m_loadChart->axisY()->setLabelsVisible(false);
    this->m_loadChart->axisY()->setVisible(false);

    this->m_loadChart->setMargins(QMargins(0, 0, 0, 0));
    this->m_loadChart->layout()->setContentsMargins(0, 0, 0, 0);
    this->m_loadChart->setBackgroundBrush(QBrush(this->cpuIdleColor));
    this->m_loadChart->setBackgroundRoundness(0);

    ui->loadChartView->setChart(this->m_loadChart);
    ui->loadChartView->setContentsMargins(0, 0, 0, 0);
    ui->loadChartView->setRenderHint(QPainter::Antialiasing);

    ui->peakLoadLabel->setStyleSheet(QString("QLabel { background-color: %1; color: white; }").arg(this->cpuIdleColor.name()));
}

void IstateCpuWidget::addCpuUsage(cpu_usage avgUsage, QList<cpu_usage> cpuUsageList) {
    this->cpuUsageList.append(avgUsage);
    if (this->cpuUsageList.size() > this->maxHistorySize) {
        this->cpuUsageList = this->cpuUsageList.mid(this->cpuUsageList.size() - this->maxHistorySize);
    }
    this->redrawCpuCurve();
}

void IstateCpuWidget::redrawCpuCurve() {
    if (this->isHidden()) return;  // reduce useless cpu usage

    this->m_userSeries->clear();
    this->m_systemSeries->clear();

    int i;
    for (i = 0; i < this->maxHistorySize - this->cpuUsageList.size(); ++i) {
        this->m_userSeries->append(i, 0);
        this->m_systemSeries->append(i, 0);
    }
    for (int j = 0; j < this->cpuUsageList.size(); ++j) {
        this->m_userSeries->append(i + j, 100 * this->cpuUsageList[j].user);
        this->m_systemSeries->append(i + j, 100 * (this->cpuUsageList[j].system + this->cpuUsageList[j].user));
    }

    ui->cpuUserLabel->setText(QString("%1%").arg(QString::number(this->cpuUsageList.last().user * 100, 'f', 1)));
    ui->cpuSystemLabel->setText(QString("%1%").arg(QString::number(this->cpuUsageList.last().system * 100, 'f', 1)));
    ui->cpuIdleLabel->setText(QString("%1%").arg(QString::number(this->cpuUsageList.last().idle * 100, 'f', 1)));
}

void IstateCpuWidget::showEvent(QShowEvent *event) {
    this->redrawCpuCurve();
    QWidget::showEvent(event);
}
