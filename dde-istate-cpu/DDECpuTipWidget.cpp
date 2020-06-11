//
// Created by septemberhx on 2020/6/11.
//

#include "DDECpuTipWidget.h"

DDECpuTipWidget::DDECpuTipWidget(QWidget *parent)
    : QLabel(parent)
{
    this->setFixedSize(85, 50);
}

void DDECpuTipWidget::redrawTip() {
    if (this->isHidden()) return;

    this->setText(QString("CPU : %1%\nLoad: %2%")
        .arg(QString::number((1 - this->cpuUsage.idle) * 100, 'f', 1))
        .arg(QString::number(this->loadAvg1, 'f', 1))
    );
}

void DDECpuTipWidget::updateCpuUsage(cpu_usage usage) {
    this->cpuUsage = usage;
    this->redrawTip();
}

void DDECpuTipWidget::updateLoad(qreal avg1, qreal avg5, qreal avg15) {
    this->loadAvg1 = avg1;
    this->loadAvg5 = avg5;
    this->loadAvg15 = avg15;
    this->redrawTip();
}

void DDECpuTipWidget::showEvent(QShowEvent *event) {
    this->redrawTip();
    QWidget::showEvent(event);
}
