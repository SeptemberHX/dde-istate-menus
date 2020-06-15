#include "istateramwidget.h"
#include "ui_istateramwidget.h"
#include <QIcon>

#define RAM_USED "Used"
#define RAM_BUFFERS "Buffers"
#define RAM_CACHED "Cached"
#define RAM_FREE "Free"

IstateRamWidget::IstateRamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IstateRamWidget),
    currMemStat({})
{
    ui->setupUi(this);

    this->memUsedColor = QColor("#6CCFF6");
    this->memBuffersColor = QColor("#ED5047");
    this->memCachedColor = QColor("#825ACA");
    this->memFreeColor = QColor("#122534");

    this->ramColorMap.insert(RAM_USED, this->memUsedColor);
    this->ramColorMap.insert(RAM_BUFFERS, this->memBuffersColor);
    this->ramColorMap.insert(RAM_CACHED, this->memCachedColor);
    this->ramColorMap.insert(RAM_FREE, this->memFreeColor);

    this->ramItemList << RAM_USED << RAM_BUFFERS << RAM_CACHED << RAM_FREE;

    this->initRamWidget();
}

IstateRamWidget::~IstateRamWidget()
{
    delete ui;
}

void IstateRamWidget::initRamWidget() {
    ui->ramUsedColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->memUsedColor.name()));
    ui->ramBuffersColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->memBuffersColor.name()));
    ui->ramCachedColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->memCachedColor.name()));
    ui->ramFreeColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->memFreeColor.name()));
}

void IstateRamWidget::updateMemStat(mem_stat memStat) {
    this->currMemStat = memStat;
    this->redrawRamUsage();
}

void IstateRamWidget::redrawRamUsage() {
    if (this->isHidden()) return;

    QMap<QString, qreal> percentMap;
    qulonglong ramTotal = this->currMemStat.mem_total_kb;
    qulonglong ramFree = this->currMemStat.mem_free_kb;
    qulonglong ramBuffers = this->currMemStat.buffers_kb;
    qulonglong ramCached = this->currMemStat.cached_kb + this->currMemStat.sreclaimable_kb;
    qulonglong ramUsed = ramTotal - ramFree - ramBuffers - ramCached;

    percentMap.insert(RAM_USED, ramTotal ? ramUsed * 100.0 / ramTotal : 0);
    percentMap.insert(RAM_BUFFERS, ramTotal ? ramBuffers * 100.0 / ramTotal : 0);
    percentMap.insert(RAM_CACHED, ramTotal ? ramCached * 100.0 / ramTotal : 0);
    percentMap.insert(RAM_FREE, ramTotal ? ramFree * 100.0 / ramTotal : 0);
    ui->ramColorProBar->setPercents(percentMap, this->ramColorMap, this->ramItemList);

    QString ramUsedStr = this->engLocale.formattedDataSize(ramUsed * 1024, 2, QLocale::DataSizeTraditionalFormat);
    QString ramBuffersStr = this->engLocale.formattedDataSize(ramBuffers * 1024, 2, QLocale::DataSizeTraditionalFormat);
    QString ramCachedStr = this->engLocale.formattedDataSize(ramCached * 1024, 2, QLocale::DataSizeTraditionalFormat);
    QString ramFreeStr = this->engLocale.formattedDataSize(ramFree * 1024, 2, QLocale::DataSizeTraditionalFormat);
    ui->ramUsedLabel->setText(ramUsedStr);
    ui->ramBuffersLabel->setText(ramBuffersStr);
    ui->ramCachedLabel->setText(ramCachedStr);
    ui->ramFreeLabel->setText(ramFreeStr);
}

void IstateRamWidget::showEvent(QShowEvent *event) {
    this->redrawRamUsage();
    this->redrawProcesses();
    QWidget::showEvent(event);
}

void IstateRamWidget::updateProcessList(QList<ProcessEntry> processEntries) {
    std::sort(processEntries.begin(), processEntries.end(), [](ProcessEntry p1, ProcessEntry p2) {
        return p1.getMemory() > p2.getMemory();
    });
    this->entries = processEntries.mid(0, 5);
    this->redrawProcesses();
}

void IstateRamWidget::redrawProcesses() {
    if (this->isHidden()) return;

    int r;
    for (r = 0; r < ui->processGridLayout->rowCount() && r < this->entries.size(); ++r) {
        QLabel *label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 0)->widget());
        label->setPixmap(this->entries[r].getIcon().pixmap(label->size()));

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 1)->widget());
        label->setText(this->entries[r].getName());

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 2)->widget());
        label->setText(this->engLocale.formattedDataSize(this->entries[r].getMemory() * 1024, 2, QLocale::DataSizeTraditionalFormat));
    }

    for (; r < ui->processGridLayout->rowCount(); ++r) {
        QLabel *label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 0)->widget());
        label->clear();

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 1)->widget());
        label->clear();

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 2)->widget());
        label->clear();
    }
}
