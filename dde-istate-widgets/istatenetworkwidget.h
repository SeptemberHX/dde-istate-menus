#ifndef ISTATENETWORKWIDGET_H
#define ISTATENETWORKWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QLineSeries>
#include <QAreaSeries>
#include <QTimer>

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
    void appendSpeed(qreal upSpeed, qreal downSpeed);
    void updateStatistics(QMap<QString, QPair<ulong, ulong>> currStat);
    void redrawCurve();
    void setTimeInterval(int msec);

public slots:
    void _addDataTest();

private:
    QString shortenDataSizeStr(QString dataSizeStr, int minLength = 9);

    Ui::IstateNetworkWidget *ui;
    QChart *m_chart;
    QLineSeries *m_inSeries;
    QAreaSeries *m_inAreaSeries;

    QLineSeries *m_outSeries;
    QAreaSeries *m_outAreaSeries;
    QLineSeries *m_seperatorSeries;

    QLineSeries *m_inBottomSeries;
    QLineSeries *m_outBottomSeries;

    QList<qreal> downSpeedHistoryList;
    QList<qreal> uploadHistoryList;

    QTimer *m_timer;

    int _testCount = 0;
    int maxHistorySize = 60;
    int maxY = 100;
    double currMaxY = 100;
    double spacing = 0.15;
    int timeInterval = 1000;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);
};

#endif // ISTATENETWORKWIDGET_H
