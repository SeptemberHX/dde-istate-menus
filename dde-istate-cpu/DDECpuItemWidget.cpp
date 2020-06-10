//
// Created by septemberhx on 2020/6/10.
//

#include "DDECpuItemWidget.h"

DDECpuItemWidget::DDECpuItemWidget(QWidget *parent)
    : QWidget(parent)
{
    this->m_layout = new QHBoxLayout(this);

    this->m_label = new QLabel(this);
    this->m_layout->addWidget(this->m_label);

    this->m_plotWidget = new DDECpuPlotItemWidget(this);
    this->m_layout->addWidget(this->m_plotWidget);

    this->m_layout->setContentsMargins(0, 0, 0, 0);
    this->m_layout->setSpacing(0);
    this->m_layout->setMargin(0);

    this->m_label->setAlignment(Qt::AlignCenter);
    this->m_label->setPixmap(QPixmap::fromImage(QImage(":/icons/cpu.svg")).scaled(10, 24, Qt::KeepAspectRatio));
    this->m_label->hide();
}

void DDECpuItemWidget::addData(qreal usagePer) {
    this->m_plotWidget->addData(usagePer);
}
