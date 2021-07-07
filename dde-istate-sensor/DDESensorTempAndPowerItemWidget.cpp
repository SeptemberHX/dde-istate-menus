//
// Created by septemberhx on 2020/6/24.
//

#include "DDESensorTempAndPowerItemWidget.h"
#include <QPainter>
#include <QDebug>
#include <DFontSizeManager>
#include <QFontDatabase>

DWIDGET_USE_NAMESPACE

DDESensorTempAndPowerItemWidget::DDESensorTempAndPowerItemWidget(QWidget *parent)
    : QLabel(parent)
    , temperature(0)
    , powerConsumption(0)
{
    this->setFixedSize(34, 24);
    this->fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Bold.ttf");
}

void DDESensorTempAndPowerItemWidget::setTempAndPower(qreal percent, qreal powerConsumption) {
    this->temperature = percent;
    this->powerConsumption = powerConsumption;
    this->repaint();
}

void DDESensorTempAndPowerItemWidget::paintEvent(QPaintEvent *event) {
    QString tempStr = QString("%1°").arg(QString::number(this->temperature, 'f', 1));
    QString powerConStr = QString("%1W").arg(QString::number(this->powerConsumption, 'f', 1));

    QFont font = QFont(QFontDatabase::applicationFontFamilies(fontId).at(0));
    font.setBold(true);
    font.setPixelSize(14);;
    while (QFontMetrics(font).boundingRect(tempStr).size().height() +
           QFontMetrics(font).boundingRect(powerConStr).size().height() > height() + 2) {
        font.setPixelSize(font.pixelSize() - 1);
    }

    QRect tempRect = QRect(rect().topLeft() + QPoint(0, 0), QSize(rect().width(), rect().height() / 2));
    QRect powerRect = QRect(tempRect.bottomLeft() + QPoint(0, 0), QSize(rect().width(), rect().height() / 2));

    QPainter painter(this);
    painter.setFont(font);
    painter.setPen(QPen(palette().brightText(), 1));
    painter.drawText(tempRect, tempStr);
    painter.drawText(powerRect, powerConStr);
}

void DDESensorTempAndPowerItemWidget::setTemp(qreal temp) {
    this->temperature = temp;
    this->repaint();
}

void DDESensorTempAndPowerItemWidget::setPower(qreal power) {
    this->powerConsumption = power;
    this->repaint();
}
