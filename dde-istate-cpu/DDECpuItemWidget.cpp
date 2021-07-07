//
// Created by septemberhx on 2020/6/10.
//

#include "DDECpuItemWidget.h"
#include <QtSvg/QSvgRenderer>

DDECpuItemWidget::DDECpuItemWidget(QWidget *parent)
    : QWidget(parent)
{
    this->m_layout = new QHBoxLayout(this);

    this->m_label = new QLabel(this);
    this->m_layout->addWidget(this->m_label);

    this->m_plotWidget = new DDECpuPlotItemWidget(this);
    this->m_layout->addWidget(this->m_plotWidget);

    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setSpacing(2);
    this->m_layout->setMargin(0);

    this->m_label->setAlignment(Qt::AlignCenter);
    auto image = QImage(":/icons/cpu-dark.svg");
    image.setDevicePixelRatio(this->devicePixelRatioF());
    image = image.scaled(10 * this->devicePixelRatio(), 24 * this->devicePixelRatio(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->m_label->setFixedSize(10, 24);
    this->m_label->setPixmap(QPixmap::fromImage(image));
}

void DDECpuItemWidget::addData(qreal usagePer) {
    this->m_plotWidget->addData(usagePer);
}
