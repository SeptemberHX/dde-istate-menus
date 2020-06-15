//
// Created by septemberhx on 2020/6/14.
//

#include "PercentColorBar.h"
#include <QPainter>
#include <QColor>
#include <QMap>
#include <QPaintEvent>
#include <utility>

PercentColorBar::PercentColorBar(QWidget *parent)
    : QLabel(parent)
{
    this->defaultColor = QColor("#404244");
}

void PercentColorBar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal x0 = event->rect().left() + 1;
    qreal y0 = event->rect().top() + 1;
    qreal height = event->rect().height() - 2;
    for (const QString& itemKey : this->currOrderList) {
        painter.setPen(this->currColorMap[itemKey]);
        painter.setBrush(this->currColorMap[itemKey]);
        qreal width = (event->rect().width() - 2) * this->currPercentMap[itemKey] / 100.0;
        painter.drawRect(QRectF(x0, y0, width, height));
        x0 += width;
    }

    if (x0 < event->rect().right() - 1) {
        painter.setPen(this->defaultColor);
        painter.setBrush(this->defaultColor);
        qreal width = event->rect().width() - 1 - x0;
        painter.drawRect(QRectF(x0, y0, width, height));
    }

    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundRect(event->rect(), 1, 1);
}

void PercentColorBar::setPercents(QMap<QString, qreal> percentMap, QMap<QString, QColor> colorMap, QList<QString> orderList) {
    assert(percentMap.keys().size() == colorMap.keys().size());

    this->currPercentMap = std::move(percentMap);
    this->currColorMap = std::move(colorMap);
    this->currOrderList = std::move(orderList);
    this->repaint();
}
