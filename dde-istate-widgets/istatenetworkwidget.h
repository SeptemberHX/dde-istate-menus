#ifndef ISTATENETWORKWIDGET_H
#define ISTATENETWORKWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QLineSeries>
#include <QAreaSeries>
#include <QTimer>
#include "../utils/process_entry.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class IstateNetworkWidget;
}

class IstateNetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IstateNetworkWidget(QWidget *parent = nullptr);
    ~IstateNetworkWidget();
    void appendBps(qreal upSpeed, qreal downSpeed);
    void setCurveDevice(QString devName, QString ipv4);
    void updateStatistics(QMap<QString, QPair<ulong, ulong>> currStat, QMap<QString, QString> netIpv4Map);
    void updateProcesses(QList<ProcessEntry> entryList);
    void redrawCurve();
    void redrawProcessList();
    void redrawStatistics();

public slots:
    void _addDataTest();

private:
    QString shortenDataSizeStr(QString dataSizeStr, int minLength = 9);

    Ui::IstateNetworkWidget *ui;
    QChart *m_chart;
    QLineSeries *m_inSeries;
protected:
    void showEvent(QShowEvent *event) override;

private:
    QAreaSeries *m_inAreaSeries;

    QLineSeries *m_outSeries;
    QAreaSeries *m_outAreaSeries;
    QLineSeries *m_seperatorSeries;

    QLineSeries *m_inBottomSeries;
    QLineSeries *m_outBottomSeries;

    QList<qreal> downSpeedHistoryList;
    QList<qreal> uploadHistoryList;
    QList<ProcessEntry> entries;

    QMap<QString, QPair<ulong, ulong>> currStat;
    QMap<QString, QString> netIpv4Map;

    QTimer *m_timer;

    int _testCount = 0;
    int maxHistorySize = 180;
    int maxY = 100;
    double currMaxY = 100;
    double spacing = 0.15;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);
};

#endif // ISTATENETWORKWIDGET_H
