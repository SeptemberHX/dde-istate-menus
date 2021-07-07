/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "datetimewidget.h"
#include "constants.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <QFontDatabase>

#define TIME_FONT DFontSizeManager::instance()->t4()
#define DATE_FONT DFontSizeManager::instance()->t10()

DWIDGET_USE_NAMESPACE

DatetimeWidget::DatetimeWidget(QWidget *parent)
    : QWidget(parent)
    , showDate(false)
{
    setMinimumSize(PLUGIN_BACKGROUND_MIN_SIZE, PLUGIN_BACKGROUND_MIN_SIZE);
    this->m_dateFont = QFont(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Regular.ttf")).at(0));
    this->m_timeFont = QFont(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Regular.ttf")).at(0));
}

void DatetimeWidget::set24HourFormat(const bool value)
{
    if (m_24HourFormat == value) {
        return;
    }

    m_24HourFormat = value;
    update();

    adjustSize();
    adjustSize();
    if (isVisible()) {
        emit requestUpdateGeometry();
    }
}

QSize DatetimeWidget::curTimeSize() const
{
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();

    this->m_timeFont.setPixelSize(20);
    this->m_dateFont.setPixelSize(20);
    QFontMetrics fm(m_timeFont);
    QString format;
    if (m_24HourFormat)
        format = "hh:mm";
    else {
        if (position == Dock::Top || position == Dock::Bottom)
            format = "hh:mm AP";
        else
            format = "hh:mm\nAP";
    }

    QString timeString = QDateTime::currentDateTime().toString(format);
    QSize timeSize = fm.boundingRect(timeString).size();
    if (timeString.contains("\n")) {
         QStringList SL = timeString.split("\n");
         timeSize = QSize(fm.boundingRect(SL.at(0)).width(), fm.boundingRect(SL.at(0)).height() + fm.boundingRect(SL.at(1)).height());
    }

    QSize dateSize = QFontMetrics(m_dateFont).boundingRect("0000/00/00").size();

    if (this->showDate) {
        if (position == Dock::Bottom || position == Dock::Top) {
            while (QFontMetrics(m_timeFont).boundingRect(timeString).size().height() +
                   QFontMetrics(m_dateFont).boundingRect("0000/00/00").size().height() > height()) {
                m_timeFont.setPixelSize(m_timeFont.pixelSize() - 1);
                timeSize.setWidth(QFontMetrics(m_timeFont).boundingRect(timeString).size().width());
                if (m_timeFont.pixelSize() - m_dateFont.pixelSize() == 1) {
                    m_dateFont.setPixelSize(m_dateFont.pixelSize() - 1);
                    dateSize.setWidth(QFontMetrics(m_dateFont).boundingRect("0000/00/00").size().width());
                }
            }
            return QSize(std::max(timeSize.width(), dateSize.width()) + 2, height());
        } else {
            while (std::max(QFontMetrics(m_timeFont).boundingRect(timeString).size().width(),
                            QFontMetrics(m_dateFont).boundingRect("0000/00/00").size().width()) > (width() - 4)) {
                m_timeFont.setPixelSize(m_timeFont.pixelSize() - 1);
                if (m_24HourFormat) {
                    timeSize.setHeight(QFontMetrics(m_timeFont).boundingRect(timeString).size().height());
                } else {
                    timeSize.setHeight(QFontMetrics(m_timeFont).boundingRect(timeString).size().height() * 2);
                }
                if (m_timeFont.pixelSize() - m_dateFont.pixelSize() == 1) {
                    m_dateFont.setPixelSize(m_dateFont.pixelSize() - 1);
                    dateSize.setWidth(QFontMetrics(m_dateFont).boundingRect("0000/00/00").size().height());
                }
            }
            m_timeOffset = (timeSize.height() - dateSize.height()) / 2;
            return QSize(width(), timeSize.height() + dateSize.height());
        }
    } else {
        if (position == Dock::Bottom || position == Dock::Top) {
            while (QFontMetrics(m_timeFont).boundingRect(timeString).size().height() > height()) {
                m_timeFont.setPixelSize(m_timeFont.pixelSize() - 1);
                timeSize.setWidth(QFontMetrics(m_timeFont).boundingRect(timeString).size().width());
            }
            return QSize(std::max(timeSize.width(), timeSize.width()) + 2, height());
        } else {
            while (QFontMetrics(m_timeFont).boundingRect(timeString).size().width() > (width() - 4)) {
                m_timeFont.setPixelSize(m_timeFont.pixelSize() - 1);
                if (m_24HourFormat) {
                    timeSize.setHeight(QFontMetrics(m_timeFont).boundingRect(timeString).size().height());
                } else {
                    timeSize.setHeight(QFontMetrics(m_timeFont).boundingRect(timeString).size().height() * 2);
                }
            }
            return QSize(width(), timeSize.height());
        }
    }
}

QSize DatetimeWidget::sizeHint() const
{
    return curTimeSize();
}

void DatetimeWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    const QDateTime current = QDateTime::currentDateTime();

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QString format;
    if (m_24HourFormat)
        format = "hh:mm";
    else {
        if (position == Dock::Top || position == Dock::Bottom)
            format = "hh:mm AP";
        else
            format = "hh:mm\nAP";
    }

    painter.setFont(m_timeFont);
    painter.setPen(QPen(palette().brightText(), 1));

    QRect timeRect = rect();
    QRect dateRect = rect();

    if (this->showDate) {
        if (position == Dock::Top || position == Dock::Bottom) {
            timeRect.setBottom(rect().center().y() + 6);
            dateRect.setTop(timeRect.bottom() - 4);
        } else {
            timeRect.setBottom(rect().center().y() + m_timeOffset);
            dateRect.setTop(timeRect.bottom());
        }
        painter.drawText(timeRect, Qt::AlignBottom | Qt::AlignHCenter, current.toString(format));
        format = "yyyy/MM/dd";
        painter.setFont(m_dateFont);
        painter.drawText(dateRect, Qt::AlignTop | Qt::AlignHCenter, current.toString(format));
    } else {
        painter.drawText(timeRect, Qt::AlignVCenter | Qt::AlignHCenter, current.toString(format));
    }
}
