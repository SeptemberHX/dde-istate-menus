//
// Created by septemberhx on 2020/6/9.
//

#include "DDECpuPlotItemWidget.h"
#include <QPainter>

DDECpuPlotItemWidget::DDECpuPlotItemWidget(QWidget *parent)
    : QLabel(parent)
    , historySize(35)
    , maxValue(20)      // 1, 2, ..., maxValue
{
    this->setFixedSize(historySize + 2, maxValue + 2);

    for (int i = 0; i < historySize; ++i) {
        this->valueList.append(0);
    }

    this->repaint();
}

QSize DDECpuPlotItemWidget::sizeHint() const {
    return QSize(historySize + 2, maxValue + 2);
}

void DDECpuPlotItemWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int x0 = event->rect().bottomLeft().x() + 1;
    int y0 = event->rect().bottomLeft().y() - 1;
    QPointF startPoint(x0, y0);
    QPainterPath painterPath(startPoint);
    for (int i = 0; i < this->valueList.size(); ++i) {
        painterPath.lineTo(x0 + i, y0 - this->valueList.at(i) + 1);
    }
    painterPath.lineTo(event->rect().bottomRight() - QPoint(1, 0));
    painterPath.closeSubpath();

    painter.setPen(QPen(QColor("#1E9CEF"), 1));
    painter.setBrush(QColor("#32B1FF"));
    painter.drawPath(painterPath);

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundRect(event->rect());
}

void DDECpuPlotItemWidget::addData(qreal usagePer) {
    qreal newValue = usagePer / 100 * maxValue;
    this->valueList.append(newValue);
    if (this->valueList.size() > historySize) {
        this->valueList = this->valueList.mid(this->valueList.size() - historySize);
    }
    this->repaint();
}
