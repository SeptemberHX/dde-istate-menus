#ifndef ISTATECPUWIDGET_H
#define ISTATECPUWIDGET_H

#include <QWidget>
#include <QColor>
#include <QtCharts>
#include <QChart>
#include <QBarSet>
#include <QLineSeries>
#include <QAreaSeries>
#include "../utils/system_stat.h"
#include "../utils/process_entry.h"

namespace Ui {
class IstateCpuWidget;
}

class IstateCpuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IstateCpuWidget(QWidget *parent = nullptr);
    void initCpuChart();
    void initCpuBarChart();
    void initLoadChart();
    ~IstateCpuWidget();

    void addCpuUsage(cpu_usage avgUsage, QList<cpu_usage> cpuUsageList);
    void updateProcesses(QList<ProcessEntry> entryList);
    void updateUptime(qulonglong uptime);
    void updateLoadAvg(qreal loadAvg1, qreal loadAvg5, qreal loadAvg15);

private:
    void redrawCpuCurve();
    void redrawCpuBarCurve();
    void redrawProcesses();
    void redrawUptime();
    void redrawLoadCurve();

protected:
    void showEvent(QShowEvent *event) override;

private:

    Ui::IstateCpuWidget *ui;

    QColor cpuIdleColor;
    QColor cpuUserColor;
    QColor cpuUserAreaColor;
    QColor cpuSystemColor;
    QColor cpuSystemAreaColor;
    QChart *m_cpuChart;

    int maxHistorySize = 60;

    QLineSeries *m_userSeries;
    QLineSeries *m_cpuZeroSeries;
    QAreaSeries *m_userAreaSeries;
    QList<cpu_usage> cpuUsageList;

    QLineSeries *m_systemSeries;
    QAreaSeries *m_systemAreaSeries;

    QChart *m_loadChart;
    QColor loadAvg15Color;
    QColor loadAvg1Color;
    QColor loadAvg5Color;
    QColor loadAvg1AreaColor;

    QLineSeries *m_loadAvg1Series;
    QAreaSeries *m_loadAvg1AreaSeries;
    QLineSeries *m_loadAvg5Series;
    QLineSeries *m_loadAvg15Series;
    QLineSeries *m_loadZeroSeries;

    QChart *m_cpuBarChart;
    QBarSet *m_cpuUserBarSet;
    QBarSet *m_cpuSystemBarSet;
    QBarSet *m_cpuIdleBarSet;
    QPercentBarSeries *m_cpuBarSeries;
    QStringList categories;
    QList<cpu_usage> cpuUsageBarList;

    QList<ProcessEntry> entries;
    qulonglong uptime;

    QList<qreal> loadAvg1List;
    QList<qreal> loadAvg5List;
    QList<qreal> loadAvg15List;
    int loadUnit;
};

#endif // ISTATECPUWIDGET_H
