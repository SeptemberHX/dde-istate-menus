//
// Created by septemberhx on 2020/6/22.
//

#include "DDESensorTempItemWidget.h"
#include <cmath>

#define TEMP_NAME "temp"
#define TEMP_LEFT "temp_left"

DDESensorTempItemWidget::DDESensorTempItemWidget(QWidget *parent)
    : QWidget(parent)
{
    this->tempColor = QColor("#5BBDF8");
    this->bgColor = QColor("#404244");

    this->colorMap.insert(TEMP_NAME, this->tempColor);
    this->colorMap.insert(TEMP_LEFT, this->bgColor);

    this->orderList << TEMP_NAME << TEMP_LEFT;

    this->m_layout = new QHBoxLayout(this);
    this->m_layout->setContentsMargins(0, 0, 0, 0);

    this->m_label = new QLabel(this);
    this->m_layout->addWidget(this->m_label);

    this->m_tempLabel = new QLabel(this);
    this->m_tempLabel->setAlignment(Qt::AlignRight);
    this->m_layout->addWidget(this->m_tempLabel);

    this->m_perColorBar = new PercentColorBar(this);
    this->m_layout->addWidget(this->m_perColorBar);

    this->setFixedHeight(20);
    this->m_perColorBar->setFixedHeight(10);
    this->m_label->setFixedWidth(100);
}

void DDESensorTempItemWidget::setText(const QString &text) {
    this->m_label->setText(text);
}

void DDESensorTempItemWidget::setTemp(qreal temp) {
    this->m_tempLabel->setText(QString("%1°").arg(QString::number(int(ceil(temp)))));
    QMap<QString, qreal> percentMap;
    percentMap.insert(TEMP_NAME, temp);
    percentMap.insert(TEMP_LEFT, 100 - temp);
    this->m_perColorBar->setPercents(percentMap, this->colorMap, this->orderList);
}
