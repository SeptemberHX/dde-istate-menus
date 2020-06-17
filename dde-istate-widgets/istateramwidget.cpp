#include "istateramwidget.h"
#include "ui_istateramwidget.h"
#include <QIcon>

#define RAM_USED "Used"
#define RAM_BUFFERS "Buffers"
#define RAM_CACHED "Cached"
#define RAM_FREE "Free"

#define SWAP_USED "SwapUsed"
#define SWAP_FREE "SwapFree"

#define ACTIVE_ANON "ActiveAnon"
#define INACTIVE_ANON "InactiveAnon"
#define ACTIVE_FILE "ActiveFile"
#define INACTIVE_FILE "InactiveFile"

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

    this->swapUsedColor = this->memUsedColor;
    this->swapFreeColor = this->memFreeColor;

    this->activeAnonColor = this->memUsedColor;
    this->inactiveAnonColor = this->memFreeColor;
    this->activeFileColor = this->memCachedColor;
    this->inactiveFileColor = QColor("#404244");

    this->ramColorMap.insert(RAM_USED, this->memUsedColor);
    this->ramColorMap.insert(RAM_BUFFERS, this->memBuffersColor);
    this->ramColorMap.insert(RAM_CACHED, this->memCachedColor);
    this->ramColorMap.insert(RAM_FREE, this->memFreeColor);

    this->swapColorMap.insert(SWAP_USED, this->swapUsedColor);
    this->swapColorMap.insert(SWAP_FREE, this->swapFreeColor);

    this->activeColorMap.insert(ACTIVE_ANON, this->activeAnonColor);
    this->activeColorMap.insert(INACTIVE_ANON, this->inactiveAnonColor);
    this->activeColorMap.insert(ACTIVE_FILE, this->activeFileColor);
    this->activeColorMap.insert(INACTIVE_FILE, this->inactiveFileColor);

    this->ramItemList << RAM_USED << RAM_BUFFERS << RAM_CACHED << RAM_FREE;
    this->swapItemList << SWAP_USED << SWAP_FREE;
    this->activeItemList << ACTIVE_ANON << INACTIVE_ANON << ACTIVE_FILE << INACTIVE_FILE;

    this->initRamWidget();
    this->initActiveWidget();
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
    this->redrawSwapUsage();
    this->redrawActiveUsage();
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
    this->redrawRamUsage();
    this->redrawActiveUsage();
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

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 2)->widget());
        label->setText(this->entries[r].getName());

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 3)->widget());
        label->setText(this->engLocale.formattedDataSize(this->entries[r].getMemory() * 1024, 2, QLocale::DataSizeTraditionalFormat));
    }

    for (; r < ui->processGridLayout->rowCount(); ++r) {
        QLabel *label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 0)->widget());
        label->clear();

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 2)->widget());
        label->clear();

        label = dynamic_cast<QLabel*>(ui->processGridLayout->itemAtPosition(r, 3)->widget());
        label->clear();
    }
}

void IstateRamWidget::redrawSwapUsage() {
    if (this->isHidden()) return;

    qulonglong swapFree = this->currMemStat.swap_free_kb;
    qulonglong swapTotal = this->currMemStat.swap_total_kb;
    qulonglong swapUsed = swapTotal - swapFree;

    QMap<QString, qreal> percentMap;
    percentMap.insert(SWAP_USED, swapTotal ? swapUsed * 100.0 / swapTotal : 0);
    percentMap.insert(SWAP_FREE, swapTotal ? swapFree * 100.0 / swapTotal : 0);
    ui->swapBarLabel->setPercents(percentMap, this->swapColorMap, this->swapItemList);

    ui->swapLabel->setText(QString("%1 of %2")
        .arg(this->engLocale.formattedDataSize(swapUsed * 1024, 2, QLocale::DataSizeTraditionalFormat))
        .arg(this->engLocale.formattedDataSize(swapTotal * 1024, 2, QLocale::DataSizeTraditionalFormat))
    );
}

void IstateRamWidget::initActiveWidget() {
    ui->activeAnonColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->activeAnonColor.name()));
    ui->inactiveAnonColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->inactiveAnonColor.name()));
    ui->activeFileColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->activeFileColor.name()));
    ui->inactiveFileColorLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(this->inactiveFileColor.name()));
}

void IstateRamWidget::redrawActiveUsage() {
    if (this->isHidden()) return;

    qulonglong activeTotal = this->currMemStat.active_kb;
    qulonglong inactiveTotal = this->currMemStat.inactive_kb;
    qulonglong total = activeTotal + inactiveTotal;
    qulonglong activeAnon = this->currMemStat.active_anon_kb;
    qulonglong inactiveAnon = this->currMemStat.inactive_anon_kb;
    qulonglong activeFile = this->currMemStat.active_file_kb;
    qulonglong inactiveFile = this->currMemStat.inactive_file_kb;

    QMap<QString, qreal> percentMap;
    percentMap.insert(ACTIVE_ANON, total ? activeAnon * 100.0 / total : 0);
    percentMap.insert(INACTIVE_ANON, total ? inactiveAnon * 100.0 / total : 0);
    percentMap.insert(ACTIVE_FILE, total ? activeFile * 100.0 / total : 0);
    percentMap.insert(INACTIVE_FILE, total ? inactiveFile * 100.0 / total : 0);
    ui->activeBarLabel->setPercents(percentMap, this->activeColorMap, this->activeItemList);
    ui->activeAnonLabel->setText(this->engLocale.formattedDataSize(activeAnon * 1024, 2, QLocale::DataSizeTraditionalFormat));
    ui->inactiveAnonLabel->setText(this->engLocale.formattedDataSize(inactiveAnon * 1024, 2, QLocale::DataSizeTraditionalFormat));
    ui->activeFileLabel->setText(this->engLocale.formattedDataSize(activeFile * 1024, 2, QLocale::DataSizeTraditionalFormat));
    ui->inactiveFileLabel->setText(this->engLocale.formattedDataSize(inactiveFile * 1024, 2, QLocale::DataSizeTraditionalFormat));
}
