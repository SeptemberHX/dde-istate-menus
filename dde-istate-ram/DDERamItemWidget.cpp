//
// Created by septemberhx on 2020/6/21.
//

#include "DDERamItemWidget.h"

void DDERamItemWidget::setPercent(qreal percent) {
    this->m_plotWidget->setPercent(percent);
}

DDERamItemWidget::DDERamItemWidget(QWidget *parent)
    : QWidget(parent)
{
    this->m_layout = new QHBoxLayout(this);

    this->m_label = new QLabel(this);
    this->m_layout->addWidget(this->m_label);

    this->m_plotWidget = new DDERamBarItemWidget(this);
    this->m_layout->addWidget(this->m_plotWidget);

    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setSpacing(2);
    this->m_layout->setMargin(0);

    this->m_label->setAlignment(Qt::AlignCenter);
    auto image = QImage(":/icons/ram-dark.svg");
    image.setDevicePixelRatio(this->devicePixelRatioF());
    image = image.scaled(10 * this->devicePixelRatio(), 22 * this->devicePixelRatio(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->m_label->setFixedSize(10, 22);
    this->m_label->setPixmap(QPixmap::fromImage(image));
}

void DDERamItemWidget::changeIconTheme(bool dark) {
    QImage image;
    if (dark) {
        image = QImage(":/icons/ram-light.svg");
    } else {
        image = QImage(":/icons/ram-dark.svg");
    }
    image.setDevicePixelRatio(this->devicePixelRatioF());
    image = image.scaled(10 * this->devicePixelRatio(), 22 * this->devicePixelRatio(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->m_label->setPixmap(QPixmap::fromImage(image));
}
