//
// Created by septemberhx on 2020/6/22.
//

#include "DDESensorItemWidget.h"
#include <cmath>

DDESensorItemWidget::DDESensorItemWidget(QWidget *parent)
    : QWidget(parent)
{
    this->m_layout = new QHBoxLayout(this);

    this->m_label = new QLabel(this);
    this->m_layout->addWidget(this->m_label);

    this->m_plotWidget = new DDESensorTempAndPowerItemWidget(this);
    this->m_layout->addWidget(this->m_plotWidget);

    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setSpacing(2);
    this->m_layout->setMargin(0);

    this->m_label->setAlignment(Qt::AlignCenter);
    auto image = QImage(":/icons/sensor-dark.svg");
    image.setDevicePixelRatio(this->devicePixelRatioF());
    image = image.scaled(10 * this->devicePixelRatio(), 28 * this->devicePixelRatio(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->m_label->setFixedSize(10, 28);
    this->m_label->setPixmap(QPixmap::fromImage(image));
}

void DDESensorItemWidget::setPower(qreal power) {
    this->m_plotWidget->setPower(power);
}

void DDESensorItemWidget::setTemp(qreal percent) {
    this->m_plotWidget->setTemp(percent);
}
