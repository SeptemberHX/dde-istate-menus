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

private:
    void redrawCpuCurve();
    void redrawCpuBarCurve();
    void redrawProcesses();
    void redrawUptime();

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
    QColor loadIdleColor;
    QColor loadUserColor;
    QColor loadSystemColor;

    QLineSeries *m_loadUserSeries;
    QAreaSeries *m_loadUserAreaSeries;
    QLineSeries *m_loadSystemSeries;
    QLineSeries *m_loadIdleSeries;

    QChart *m_cpuBarChart;
    QBarSet *m_cpuUserBarSet;
    QBarSet *m_cpuSystemBarSet;
    QBarSet *m_cpuIdleBarSet;
    QPercentBarSeries *m_cpuBarSeries;
    QStringList categories;
    QList<cpu_usage> cpuUsageBarList;

    QList<ProcessEntry> entries;
    qulonglong uptime;
};

#endif // ISTATECPUWIDGET_H
