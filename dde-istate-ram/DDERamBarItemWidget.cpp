//
// Created by septemberhx on 2020/6/13.
//

#include "DDERamBarItemWidget.h"
#include <QPainter>
#include <QPaintEvent>

DDERamBarItemWidget::DDERamBarItemWidget(QWidget *parent)
    : QLabel(parent)
    , percent(0)
    , maxValue(24)
    , width(10)
{
    this->setFixedSize(width + 2, maxValue + 2);
}

void DDERamBarItemWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QColor("#32B1FF"));
    painter.setBrush(QColor("#32B1FF"));

    qreal y0 = event->rect().bottomLeft().y();
    qreal x0 = event->rect().bottomLeft().x();
    qreal height = this->percent / 100 * this->maxValue;
    painter.drawRect(x0, y0 - height, width, height);

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundRect(event->rect(), 50, 50);
}

void DDERamBarItemWidget::setPercent(qreal percent) {
    if (qAbs(percent / 100 * this->maxValue - this->percent / 100 * this->maxValue) < 1) {  // abandon
        return;
    }
    DDERamBarItemWidget::percent = percent;
    this->repaint();
}
