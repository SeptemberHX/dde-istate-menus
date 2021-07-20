#include "istatecpuwidget.h"
#include "ui_istatecpuwidget.h"

#include <iostream>
#include <QDebug>

IstateCpuWidget::IstateCpuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IstateCpuWidget)
{
    ui->setupUi(this);

    this->cpuIdleColor = QColor("#404244");
    this->cpuUserColor = QColor("#17a5ec");
    this->cpuUserAreaColor = QColor("#2bbafc");
    this->cpuSystemColor = QColor("#ef1818");
    this->cpuSystemAreaColor = QColor("#ff2a2a");

    this->loadAvg15Color = QColor("#9C9CA0");
    this->loadAvg1Color = this->cpuUserColor;
    this->loadAvg5Color = this->cpuSystemAreaColor;
    this->loadAvg1AreaColor = this->cpuUserAreaColor;

    ui->cpuIdleColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->cpuIdleColor.name()));
    ui->cpuSystemColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->cpuSystemColor.name()));
    ui->cpuUserColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->cpuUserColor.name()));

    ui->loadAvg1ColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->loadAvg1Color.name()));
    ui->loadAvg5ColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->loadAvg5Color.name()));
    ui->loadAvg15ColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->loadAvg15Color.name()));

    this->loadUnit = 5;

    initCpuChart();
    initCpuBarChart();
    initLoadChart();
}

IstateCpuWidget::~IstateCpuWidget()
{
    delete ui;
}

void IstateCpuWidget::initCpuChart() {
    this->m_cpuChart = new QChart();

    this->m_userSeries = new QLineSeries();
    this->m_userSeries->setPen(QPen(this->cpuUserColor, 1));
    this->m_userSeries->setBrush(Qt::NoBrush);

    this->m_systemSeries = new QLineSeries();
    this->m_systemSeries->setPen(QPen(this->cpuSystemColor, 1));
    this->m_systemSeries->setBrush(Qt::NoBrush);

    this->m_cpuZeroSeries = new QLineSeries();
    this->m_cpuZeroSeries->setPen(Qt::NoPen);
    this->m_cpuZeroSeries->setBrush(Qt::NoBrush);
    this->m_cpuChart->addSeries(this->m_cpuZeroSeries);

    for (int i = 0; i < this->maxHistorySize; ++i) {
        this->m_cpuZeroSeries->append(i, 0);
    }

    this->m_userAreaSeries = new QAreaSeries(this->m_userSeries, this->m_cpuZeroSeries);
    this->m_userAreaSeries->setPen(QPen(Qt::transparent));
    this->m_userAreaSeries->setBrush(this->cpuUserAreaColor);
    this->m_cpuChart->addSeries(this->m_userAreaSeries);

    this->m_systemAreaSeries = new QAreaSeries(this->m_systemSeries, this->m_userSeries);
    this->m_systemAreaSeries->setPen(QPen(Qt::transparent));
    this->m_systemAreaSeries->setBrush(this->cpuSystemAreaColor);
    this->m_cpuChart->addSeries(this->m_systemAreaSeries);

    this->m_cpuChart->addSeries(this->m_userSeries);
    this->m_cpuChart->addSeries(this->m_systemSeries);

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
    this->m_cpuChart->setBackgroundRoundness(5);
    ui->cpuChartView->setStyleSheet("* { background: transparent; }");

    ui->cpuChartView->setChart(this->m_cpuChart);
    ui->cpuChartView->setContentsMargins(0, 0, 0, 0);
    ui->cpuChartView->setRenderHint(QPainter::Antialiasing);
}

void IstateCpuWidget::initLoadChart() {
    this->m_loadChart = new QChart();

    this->m_loadAvg1Series = new QLineSeries();
    this->m_loadAvg1Series->setPen(QPen(this->loadAvg1Color, 1));
    this->m_loadAvg1Series->setBrush(Qt::NoBrush);

    this->m_loadAvg5Series = new QLineSeries();
    this->m_loadAvg5Series->setPen(QPen(this->loadAvg5Color, 1));
    this->m_loadAvg5Series->setBrush(Qt::NoBrush);

    this->m_loadAvg15Series = new QLineSeries();
    this->m_loadAvg15Series->setPen(QPen(this->loadAvg15Color, 1));
    this->m_loadAvg15Series->setBrush(Qt::NoBrush);

    this->m_loadZeroSeries = new QLineSeries();
    this->m_loadZeroSeries->setPen(Qt::NoPen);
    this->m_loadZeroSeries->setBrush(Qt::NoBrush);

    for (int i = 0; i < this->maxHistorySize; ++i) {
        this->m_loadZeroSeries->append(i, 0);
    }

    this->m_loadAvg1AreaSeries = new QAreaSeries(this->m_loadAvg1Series, this->m_loadZeroSeries);
    this->m_loadAvg1AreaSeries->setPen(Qt::NoPen);
    this->m_loadAvg1AreaSeries->setBrush(this->loadAvg1AreaColor);

    this->m_loadChart->addSeries(this->m_loadAvg1AreaSeries);
    this->m_loadChart->addSeries(this->m_loadAvg1Series);
    this->m_loadChart->addSeries(this->m_loadAvg5Series);
    this->m_loadChart->addSeries(this->m_loadAvg15Series);
    this->m_loadChart->addSeries(this->m_loadZeroSeries);

    this->m_loadChart->legend()->setVisible(false);

    // set the default range of the axes and hide the axes, grid line, and label
    this->m_loadChart->createDefaultAxes();
    this->m_loadChart->axisX()->setRange(0, this->maxHistorySize - 1);
    this->m_loadChart->axisX()->setGridLineVisible(false);
    this->m_loadChart->axisX()->setLabelsVisible(false);
    this->m_loadChart->axisX()->setVisible(false);

    this->m_loadChart->axisY()->setRange(0, this->loadUnit);
    this->m_loadChart->axisY()->setGridLineVisible(false);
    this->m_loadChart->axisY()->setLabelsVisible(false);
    this->m_loadChart->axisY()->setVisible(false);

    this->m_loadChart->setMargins(QMargins(0, 0, 0, 0));
    this->m_loadChart->layout()->setContentsMargins(0, 0, 0, 0);
    this->m_loadChart->setBackgroundBrush(QBrush(this->cpuIdleColor));
    this->m_loadChart->setBackgroundRoundness(5);

    ui->loadChartView->setStyleSheet("* { background: transparent; }");
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

    this->cpuUsageBarList = cpuUsageList;
    this->redrawCpuCurve();
    this->redrawCpuBarCurve();
}

void IstateCpuWidget::redrawCpuCurve() {
    if (this->isHidden()) return;  // reduce useless cpu usage

    std::cout << "======> Trying to redraw cpu usage curve ..." << std::endl;
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
    std::cout << "======> Redraw cpu usage curve finished" << std::endl;
}

void IstateCpuWidget::showEvent(QShowEvent *event) {
    this->redrawCpuCurve();
    this->redrawCpuBarCurve();
    this->redrawProcesses();
    this->redrawUptime();
    this->redrawLoadCurve();
    QWidget::showEvent(event);
}

void IstateCpuWidget::initCpuBarChart() {
    this->m_cpuBarChart = new QChart();

    this->m_cpuUserBarSet = new QBarSet("User");
    this->m_cpuUserBarSet->setPen(QPen(this->cpuUserColor, 0.5));
    this->m_cpuUserBarSet->setBrush(this->cpuUserAreaColor);

    this->m_cpuSystemBarSet = new QBarSet("System");
    this->m_cpuSystemBarSet->setPen(QPen(this->cpuSystemColor, 0.5));
    this->m_cpuSystemBarSet->setBrush(this->cpuSystemAreaColor);

    this->m_cpuIdleBarSet = new QBarSet("Idle");
    this->m_cpuIdleBarSet->setPen(Qt::NoPen);
    this->m_cpuIdleBarSet->setBrush(Qt::NoBrush);

    this->m_cpuBarSeries = new QPercentBarSeries(this->m_cpuBarChart);
    this->m_cpuBarSeries->append(this->m_cpuUserBarSet);
    this->m_cpuBarSeries->append(this->m_cpuSystemBarSet);
    this->m_cpuBarSeries->append(this->m_cpuIdleBarSet);
    this->m_cpuBarChart->addSeries(this->m_cpuBarSeries);

    QValueAxis *axisY = new QValueAxis();
    axisY->hide();
    axisY->setGridLineVisible(false);
    this->m_cpuBarChart->addAxis(axisY, Qt::AlignLeft);
    this->m_cpuBarSeries->attachAxis(axisY);

    this->m_cpuBarChart->legend()->setVisible(false);

    this->m_cpuBarChart->setMargins(QMargins(0, 0, 0, 0));
    this->m_cpuBarChart->layout()->setContentsMargins(0, 0, 0, 0);
    this->m_cpuBarChart->setBackgroundBrush(QBrush(this->cpuIdleColor));
    this->m_cpuBarChart->setBackgroundRoundness(5);

    ui->cpuBarChartView->setStyleSheet("* { background: transparent; }");
    ui->cpuBarChartView->setChart(this->m_cpuBarChart);
    ui->cpuBarChartView->setContentsMargins(0, 0, 0, 0);
    ui->cpuBarChartView->setRenderHint(QPainter::Antialiasing);
}

void IstateCpuWidget::redrawCpuBarCurve() {
    if (this->isHidden()) return;

    std::cout << "======> Trying to redraw cpu bar curve ..." << std::endl;

    if (this->categories.size() != this->cpuUsageBarList.size()) {
        this->categories.clear();
        for (int i  = 0; i < this->cpuUsageBarList.size(); ++i) {
            categories << QString::number(i);
        }
        ui->cpuBarChartView->setFixedWidth(6 * this->cpuUsageBarList.size());

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->hide();
        axisX->append(categories);
        this->m_cpuBarChart->addAxis(axisX, Qt::AlignBottom);
        this->m_cpuBarSeries->attachAxis(axisX);
        this->m_cpuBarSeries->setBarWidth(1);

    }

    this->m_cpuUserBarSet->remove(0, this->cpuUsageBarList.size());
    this->m_cpuSystemBarSet->remove(0, this->cpuUsageBarList.size());
    this->m_cpuIdleBarSet->remove(0, this->cpuUsageBarList.size());

    for (auto & i : this->cpuUsageBarList) {
        this->m_cpuUserBarSet->append(i.user);
        this->m_cpuSystemBarSet->append(i.system);
        this->m_cpuIdleBarSet->append(i.idle);
    }

    std::cout << "======> Redraw cpu bar curve finished" << std::endl;
}

void IstateCpuWidget::updateProcesses(QList<ProcessEntry> entryList) {
    std::sort(entryList.begin(), entryList.end(), [](const ProcessEntry &e1, const ProcessEntry &e2) {
        return e1.getCPU() > e2.getCPU();
    });
    this->entries = entryList.mid(0, 5);
    this->redrawProcesses();
}

void IstateCpuWidget::redrawProcesses() {
    if (this->isHidden()) return;
    std::cout << "======> Trying to redraw cpu processes ..." << std::endl;

    int r;
    for (r = 0; r < ui->processGridLayout->rowCount() && r < this->entries.size(); ++r) {
        QLabel *label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 0)->widget());
        auto pixmap = this->entries[r].getIcon().pixmap(label->sizeHint() * this->devicePixelRatioF());
        pixmap.setDevicePixelRatio(this->devicePixelRatioF());
        label->setPixmap(pixmap);

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 2)->widget());
        label->setText(this->entries[r].getName());

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 3)->widget());
        label->setText(QString("%1%").arg(QString::number(this->entries[r].getCPU(), 'f', 1)));
    }

    for (; r < ui->processGridLayout->rowCount(); ++r) {
        QLabel *label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 0)->widget());
        label->clear();

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 2)->widget());
        label->clear();

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 3)->widget());
        label->clear();
    }
    std::cout << "======> Redraw cpu processes finished" << std::endl;
}

void IstateCpuWidget::updateUptime(qulonglong uptime) {
    this->uptime = uptime;
    this->redrawUptime();
}

void IstateCpuWidget::redrawUptime() {
    if (this->isHidden()) return;

    std::cout << "======> Trying to redraw uptime ..." << std::endl;

    int minutes = this->uptime / 60;
    int nowMinutes = minutes % 60;
    int hours = minutes / 60;
    int nowHours = hours % 24;
    int days = hours / 24;

    ui->uptimeLabel->setText(QString("%1 days, %2 hours, %3 minutes").arg(days).arg(nowHours).arg(nowMinutes));

    std::cout << "======> Redraw uptime finished" << std::endl;
}

void IstateCpuWidget::updateLoadAvg(qreal loadAvg1, qreal loadAvg5, qreal loadAvg15) {
    this->loadAvg1List.append(loadAvg1);
    if (this->loadAvg1List.size() > this->maxHistorySize) {
        this->loadAvg1List = this->loadAvg1List.mid(this->loadAvg1List.size() - this->maxHistorySize);
    }

    this->loadAvg5List.append(loadAvg5);
    if (this->loadAvg5List.size() > this->maxHistorySize) {
        this->loadAvg5List = this->loadAvg5List.mid(this->loadAvg5List.size() - this->maxHistorySize);
    }

    this->loadAvg15List.append(loadAvg15);
    if (this->loadAvg15List.size() > this->maxHistorySize) {
        this->loadAvg15List = this->loadAvg15List.mid(this->loadAvg15List.size() - this->maxHistorySize);
    }

    this->redrawLoadCurve();
}

void IstateCpuWidget::redrawLoadCurve() {
    if (this->isHidden()) return;

    std::cout << "======> Trying to redraw system load curve ..." << std::endl;

    this->m_loadAvg1Series->clear();
    this->m_loadAvg5Series->clear();
    this->m_loadAvg15Series->clear();

    if (this->loadAvg1List.isEmpty() || this->loadAvg5List.isEmpty() || this->loadAvg15List.isEmpty()) {
        std::cout << "======> Redraw system load curve finished due to empty list" << std::endl;
        return;
    }

    qreal maxAvg1 = *std::max_element(this->loadAvg1List.begin(), this->loadAvg1List.end());
    qreal maxAvg5 = *std::max_element(this->loadAvg5List.begin(), this->loadAvg5List.end());
    qreal maxAvg15 = *std::max_element(this->loadAvg15List.begin(), this->loadAvg15List.end());

    qreal maxAvg = qMax(maxAvg1, qMax(maxAvg5, maxAvg15));

    this->m_loadChart->axisY()->setRange(0, (floor(maxAvg / this->loadUnit) + 1) * this->loadUnit);

    int i;
    for (i = 0; i < this->maxHistorySize - this->loadAvg1List.size(); ++i) {
        this->m_loadAvg1Series->append(i, 0);
        this->m_loadAvg5Series->append(i, 0);
        this->m_loadAvg15Series->append(i, 0);
    }

    for (int j = 0; j < this->loadAvg1List.size(); ++j) {
        this->m_loadAvg1Series->append(i + j, this->loadAvg1List[j]);
        this->m_loadAvg5Series->append(i + j, this->loadAvg5List[j]);
        this->m_loadAvg15Series->append(i + j, this->loadAvg15List[j]);
    }

    ui->peakLoadLabel->setText(QString("%1: %2").arg(tr("Peak load")).arg(QString::number(maxAvg1, 'f', 2)));
    ui->loadAvg1ValueLabel->setText(QString::number(this->loadAvg1List.last(), 'f', 2));
    ui->loadAvg5ValueLabel->setText(QString::number(this->loadAvg5List.last(), 'f', 2));
    ui->loadAvg15ValueLabel->setText(QString::number(this->loadAvg15List.last(), 'f', 2));

    std::cout << "======> Redraw system load curve finished" << std::endl;
}
