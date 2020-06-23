#include "istatesensorwidget.h"
#include "ui_istatesensorwidget.h"

IstateSensorWidget::IstateSensorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IstateSensorWidget)
{
    ui->setupUi(this);

    this->m_tempLayout = new QVBoxLayout(ui->tempWidget);
    this->m_tempLayout->setContentsMargins(0, 0, 0, 0);
    this->m_tempLayout->setSpacing(1);
}

IstateSensorWidget::~IstateSensorWidget()
{
    delete ui;
}

void IstateSensorWidget::updateTempInfos(QList<TempInfo> infoList) {
    this->infoList = infoList;
    std::sort(this->infoList.begin(), this->infoList.end(), [](const TempInfo& t1, const TempInfo& t2) {
        return t1.deviceName < t2.deviceName;
    });
    this->redrawTemp();
}

void IstateSensorWidget::redrawTemp() {
    if (this->isHidden()) return;

    int r;
    for (r = 0; r < this->m_tempLayout->count() && r < this->infoList.size(); ++r) {
        auto *itemWidget = qobject_cast<DDESensorTempItemWidget*>(this->m_tempLayout->itemAt(r)->widget());
        itemWidget->setText(this->infoList[r].deviceName);
        itemWidget->setTemp(this->infoList[r].deviceTemp);
        itemWidget->show();
    }

    for (; r < this->m_tempLayout->count(); ++r) {
        auto *itemWidget = qobject_cast<DDESensorTempItemWidget*>(this->m_tempLayout->itemAt(r)->widget());
        itemWidget->hide();
    }

    for (; r < this->infoList.size(); ++r) {
        auto *itemWidget = new DDESensorTempItemWidget(ui->tempWidget);
        itemWidget->setText(this->infoList[r].deviceName);
        itemWidget->setTemp(this->infoList[r].deviceTemp);
        this->m_tempLayout->addWidget(itemWidget);
    }
    ui->tempWidget->setFixedHeight(infoList.size() * 20 + infoList.size() - 1);
    ui->widget->setFixedHeight(100);
    ui->groupBox->setFixedHeight(ui->tempWidget->height() + 50 + ui->widget->height());
    this->setFixedHeight(ui->groupBox->height());
}

void IstateSensorWidget::showEvent(QShowEvent *event) {
    this->redrawTemp();
    this->redrawPowerC();
    QWidget::showEvent(event);
}

void IstateSensorWidget::updatePowerConsumption(PowerConsumption pc) {
    this->pc = pc;
    this->redrawPowerC();
}

void IstateSensorWidget::redrawPowerC() {
    if (this->isHidden()) return;

    ui->packageLabel->setText(QString("%1 W").arg(QString::number(pc.package * 1000 / pc.timestamp, 'f', 1)));
    ui->coresLabel->setText(QString("%1 W").arg(QString::number(pc.cores * 1000 / pc.timestamp, 'f', 1)));
    ui->onCoreGpuLabel->setText(QString("%1 W").arg(QString::number(pc.onCoreGpu * 1000 / pc.timestamp, 'f', 1)));
    ui->dramLabel->setText(QString("%1 W").arg(QString::number(pc.dram * 1000 / pc.timestamp, 'f', 1)));
    ui->psysLabel->setText(QString("%1 W").arg(QString::number(pc.psys * 1000 / pc.timestamp, 'f', 1)));
}
