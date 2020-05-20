#include "istatenetworkwidget.h"
#include "ui_istatenetworkwidget.h"

IstateNetworkWidget::IstateNetworkWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IstateNetworkWidget)
{
    ui->setupUi(this);

    this->m_chart = new QChart();
    this->m_inSeries = new QLineSeries();
    this->m_outSeries = new QLineSeries();
    this->m_seperatorSeries = new QLineSeries();

    this->m_inBottomSeries = new QLineSeries();
    this->m_outBottomSeries = new QLineSeries();

    this->m_inAreaSeries = new QAreaSeries(this->m_inSeries, this->m_inBottomSeries);
    this->m_inAreaSeries->setPen(Qt::NoPen);
    this->m_inAreaSeries->setBrush(QBrush("#0081FF"));
    this->m_chart->addSeries(this->m_inAreaSeries);

    this->m_outAreaSeries = new QAreaSeries(this->m_outSeries, this->m_outBottomSeries);
    this->m_outAreaSeries->setPen(Qt::NoPen);
    this->m_outAreaSeries->setBrush(QBrush(Qt::red));
    this->m_chart->addSeries(this->m_outAreaSeries);

    this->m_chart->addSeries(this->m_outSeries);
    this->m_outSeries->setName(tr("Peak↑"));                    // upload speed
    this->m_outSeries->setPen(QPen(QBrush("#EC9B9B"), 2));

    this->m_chart->addSeries(this->m_inSeries);                 // download speed
    this->m_inSeries->setName(tr("Peak↓"));
    this->m_inSeries->setPen(QPen(QBrush("#4DADFB"), 2));

    this->m_chart->addSeries(this->m_seperatorSeries);
    this->m_seperatorSeries->setPen(QPen(Qt::white, 1));
    for (int i = 0; i < this->maxHistorySize; ++i) {
        this->m_seperatorSeries->append(i, 0);
    }

    // enable the legend and move it to bottom
    this->m_chart->legend()->setVisible(true);
    this->m_chart->legend()->setAlignment(Qt::AlignBottom);

    // set the default range of the axes and hide the axes, grid line, and label
    this->m_chart->createDefaultAxes();
    this->m_chart->axisX()->setRange(0, this->maxHistorySize - 1);                     // record maxHistorySize timetick history. Fixed during running.
    this->m_chart->axisX()->setGridLineVisible(false);
    this->m_chart->axisX()->setLabelsVisible(false);
    this->m_chart->axisX()->setVisible(false);

    this->m_chart->axisY()->setRange(-(this->maxY + spacing), this->maxY + spacing);      // network transfer data size. Default range is [0, maxY) KB. Scaling at time: x1, x2, ...
    this->m_chart->axisY()->setGridLineVisible(false);
    this->m_chart->axisY()->setLabelsVisible(false);
    this->m_chart->axisY()->setVisible(false);

    // remove all the margins around the chart, curve, and legends
    this->m_chart->setMargins(QMargins(0, 0, 0, 0));
    this->m_chart->legend()->setContentsMargins(0, 0, 0, 0);
    this->m_chart->layout()->setContentsMargins(0, 0, 0, 0);
    this->m_chart->legend()->layout()->setContentsMargins(0, 0, 0, 0);

    // hide the legend of the seperator
    this->m_chart->legend()->markers(this->m_seperatorSeries)[0]->setVisible(false);
    this->m_chart->legend()->markers(this->m_inAreaSeries)[0]->setVisible(false);
    this->m_chart->legend()->markers(this->m_outAreaSeries)[0]->setVisible(false);

    this->m_chart->setBackgroundBrush(QBrush("#404244"));
    this->m_chart->setBackgroundRoundness(0);
    this->m_chart->legend()->setLabelColor(Qt::white);

    ui->chartView->setChart(this->m_chart);
    ui->chartView->setContentsMargins(0, 0, 0, 0);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

// for test
//    this->m_timer = new QTimer(this);
//    this->m_timer->setInterval(1000);
//    connect(this->m_timer, &QTimer::timeout, this, &IstateNetworkWidget::_addDataTest);
//    this->m_timer->start();
}

IstateNetworkWidget::~IstateNetworkWidget()
{
    delete ui;
}

void IstateNetworkWidget::appendSpeed(qreal upSpeed, qreal downSpeed)
{
    this->downSpeedHistoryList.append(downSpeed / this->timeInterval * 1000);
    this->uploadHistoryList.append(upSpeed / this->timeInterval * 1000);

    if (this->downSpeedHistoryList.size() > this->maxHistorySize) {
        this->downSpeedHistoryList = this->downSpeedHistoryList.mid(this->downSpeedHistoryList.size() - this->maxHistorySize);
    }
    if (this->uploadHistoryList.size() > this->maxHistorySize) {
        this->uploadHistoryList = this->uploadHistoryList.mid(this->uploadHistoryList.size() - this->maxHistorySize);
    }
    this->redrawCurve();
}

void IstateNetworkWidget::redrawCurve()
{
    this->m_inSeries->clear();
    this->m_outSeries->clear();
    this->m_inBottomSeries->clear();
    this->m_outBottomSeries->clear();

    // scale the y axis if needed
    qreal uploadPeak = *(std::max_element(this->uploadHistoryList.begin(), this->uploadHistoryList.end()));
    qreal downloadPeak = *(std::max_element(this->downSpeedHistoryList.begin(), this->downSpeedHistoryList.end()));
    qreal t_maxY = std::ceil(std::max(uploadPeak, downloadPeak) / 1000 / this->maxY) * this->maxY;
    this->currMaxY = t_maxY;
    this->m_chart->axisY()->setRange(-(t_maxY * (1 + this->spacing)), t_maxY * (1 + this->spacing));

    // format and show peak speed
    QString uploadPeakStr = this->engLocale.formattedDataSize(uploadPeak, 1, QLocale::DataSizeSIFormat);
    QString downloadPeakStr = this->engLocale.formattedDataSize(downloadPeak, 1, QLocale::DataSizeSIFormat);
    uploadPeakStr = this->shortenDataSizeStr(uploadPeakStr);
    downloadPeakStr = this->shortenDataSizeStr(downloadPeakStr);
    this->m_inSeries->setName(QString("%1↓  %2/s").arg(tr("Peak"), downloadPeakStr));
    this->m_outSeries->setName(QString("%1↑  %2/s").arg(tr("Peak"), uploadPeakStr));

    // format and show current speed
    QString currUploadSpeedStr = this->engLocale.formattedDataSize(this->uploadHistoryList.last(), 1, QLocale::DataSizeSIFormat);
    QString currDownloadSpeedStr = this->engLocale.formattedDataSize(this->downSpeedHistoryList.last(), 1, QLocale::DataSizeSIFormat);
    currUploadSpeedStr = this->shortenDataSizeStr(currUploadSpeedStr);
    currDownloadSpeedStr = this->shortenDataSizeStr(currDownloadSpeedStr);
    ui->currUploadSpeedLabel->setText(QString("%1/s").arg(currUploadSpeedStr));
    ui->currDownloadSpeedLabel->setText(QString("%1/s").arg(currDownloadSpeedStr));

    // draw the curve
    int startX = this->maxHistorySize - this->downSpeedHistoryList.size();
    double t_spacing = this->spacing * this->currMaxY;
    for (int i = 0; startX + i < this->maxHistorySize; ++i) {
        this->m_inSeries->append(startX + i, -this->downSpeedHistoryList[i] / 1000 - t_spacing);  // convert from bytes to kB
        this->m_outSeries->append(startX + i, this->uploadHistoryList[i] / 1000 + t_spacing);
        this->m_inBottomSeries->append(startX + i, -(this->currMaxY * (this->spacing - 0.05)));
        this->m_outBottomSeries->append(startX + i, this->currMaxY * (this->spacing - 0.05));
    }
}

void IstateNetworkWidget::setTimeInterval(int msec)
{
    this->timeInterval = msec;
}

void IstateNetworkWidget::_addDataTest()
{
    this->appendSpeed(10 * this->_testCount, 10 * this->_testCount * 1000);
    ++this->_testCount;
}

QString IstateNetworkWidget::shortenDataSizeStr(QString dataSizeStr, int minLength)
{
    int i = -1;
    for (int j = 0; j < dataSizeStr.length(); ++j) {
        if (dataSizeStr[j].isLetter()) {
            i = j;
            break;
        }
    }

    QString tmpStr = dataSizeStr;
    if (i >= 0 && i + 1 < dataSizeStr.size()) {
        tmpStr = dataSizeStr.mid(0, i + 1);
    }

    while (tmpStr.length() < minLength) {
        tmpStr = tmpStr.insert(0, ' ');
    }
    return tmpStr;
}

void IstateNetworkWidget::updateStatistics(QMap<QString, QPair<ulong, ulong>> currStat) {
    QList<QString> interfaceList(currStat.keys());
    std::sort(interfaceList.begin(), interfaceList.end(), [currStat](const QString &s1, const QString &s2) {
        return currStat[s1].first + currStat[s1].second > currStat[s2].first + currStat[s2].second;
    });

    int rank = 0;
    for (int r = 1; r < ui->statisticsGridLayout->rowCount(); ++r) {
        for (int c = 0; c < ui->statisticsGridLayout->columnCount(); ++c) {
            auto *qLabel = dynamic_cast<QLabel*>(ui->statisticsGridLayout->itemAtPosition(r, c)->widget());
            qLabel->clear();
        }

        if (rank < interfaceList.size()) {
            QString interfaceName = interfaceList.at(rank);
            auto *qLabel = dynamic_cast<QLabel*>(ui->statisticsGridLayout->itemAtPosition(r, 0)->widget());
            qLabel->setText(interfaceName);

            qLabel = dynamic_cast<QLabel*>(ui->statisticsGridLayout->itemAtPosition(r, 1)->widget());
            QString uploadStr = this->engLocale.formattedDataSize(currStat[interfaceName].second, 2, QLocale::DataSizeSIFormat);
            qLabel->setText(uploadStr);

            qLabel = dynamic_cast<QLabel*>(ui->statisticsGridLayout->itemAtPosition(r, 2)->widget());
            QString downloadStr = this->engLocale.formattedDataSize(currStat[interfaceName].first, 2, QLocale::DataSizeSIFormat);
            qLabel->setText(downloadStr);

            ++rank;
        }
    }
}

void IstateNetworkWidget::updateProcesses(QMap<QString, QPair<ulong, ulong>> currStat) {
    QList<QString> processList(currStat.keys());
    std::sort(processList.begin(), processList.end(), [currStat](const QString &s1, const QString &s2) {
        return currStat[s1].second > currStat[s2].second;
    });

    int rank = 0;
    for (int r = 1; r < ui->processesGridLayout->rowCount(); ++r) {
        for (int c = 0; c < ui->processesGridLayout->columnCount(); ++c) {
            auto *qLabel = dynamic_cast<QLabel*>(ui->processesGridLayout->itemAtPosition(r, c)->widget());
            qLabel->clear();
        }

        if (rank < processList.size()) {
            QString processName = processList.at(rank);
            auto *qLabel = dynamic_cast<QLabel*>(ui->processesGridLayout->itemAtPosition(r, 0)->widget());
            qLabel->setText(processName);

            qLabel = dynamic_cast<QLabel*>(ui->processesGridLayout->itemAtPosition(r, 1)->widget());
            QString uploadStr = this->engLocale.formattedDataSize(currStat[processName].second, 2, QLocale::DataSizeSIFormat);
            qLabel->setText(uploadStr + "/s");

            qLabel = dynamic_cast<QLabel*>(ui->processesGridLayout->itemAtPosition(r, 2)->widget());
            QString downloadStr = this->engLocale.formattedDataSize(currStat[processName].first, 2, QLocale::DataSizeSIFormat);
            qLabel->setText(downloadStr + "/s");

            ++rank;
        }
    }
}
