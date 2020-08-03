//
// Created by septemberhx on 2020/5/21.
//

#include "DDENetspeedTextWidget.h"
#include <DGuiApplicationHelper>
#include <DStyle>
#include <constants.h>
#include <QPainter>
#include <DFontSizeManager>

#define PLUGIN_STATE_KEY    "enable"
#define TIME_FONT DFontSizeManager::instance()->t4()

DWIDGET_USE_NAMESPACE

DDENetspeedTextWidget::DDENetspeedTextWidget(QWidget *parent)
        : QWidget(parent) {
    setMinimumSize(PLUGIN_BACKGROUND_MIN_SIZE, PLUGIN_BACKGROUND_MIN_SIZE);
    this->setFixedHeight(24);
}

QSize DDENetspeedTextWidget::sizeHint() const {
    return this->curSize();
}

void DDENetspeedTextWidget::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    this->curSize();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(this->m_font);
    painter.setPen(QPen(palette().brightText(), 1));

    QRect uploadRect = rect();
    QRect downloadRect = rect();

    uploadRect.setBottom(rect().center().y() + 2);
    downloadRect.setTop(uploadRect.bottom() - 2);

    painter.drawText(uploadRect, Qt::AlignBottom | Qt::AlignLeft, getUploadBpsStr());
    painter.drawText(downloadRect, Qt::AlignTop | Qt::AlignLeft, getDownloadBpsStr());
}

QString DDENetspeedTextWidget::shortenDataSizeStr(QString dataSizeStr, int fixLength) const {
    int i = -1;
    for (int j = 0; j < dataSizeStr.length(); ++j) {
        if (dataSizeStr[j].isLetter()) {
            i = j;
            break;
        }
    }

    QString tmpStr = dataSizeStr;
    if (i >= 0 && i + 1 < dataSizeStr.size()) {
        tmpStr = dataSizeStr.mid(0, i + 1);
    }

    while (tmpStr.length() < fixLength) {
        tmpStr = tmpStr.insert(0, ' ');
    }

    return tmpStr;
}

void DDENetspeedTextWidget::setUpAndDownBps(qreal uploadBps, qreal downloadBps) {
    this->uploadBps = uploadBps;
    this->downloadBps = downloadBps;
    this->repaint();
}

QSize DDENetspeedTextWidget::curSize() const {
    QLocale tmpLocal = QLocale(QLocale::English, QLocale::UnitedStates);

    this->m_font = TIME_FONT;
    QFontMetrics fm(this->m_font);
    QString uploadStr = QString("↑ 1000.0B");
    QString downloadStr = QString("↑ 1000.0B");

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();

    if (position == Dock::Top || position == Dock::Bottom) {
        while (QFontMetrics(m_font).boundingRect(uploadStr).size().height() +
               QFontMetrics(m_font).boundingRect(downloadStr).size().height() > height() + 2) {
            m_font.setPixelSize(m_font.pixelSize() - 1);
        }
    } else {
        while (std::max(QFontMetrics(m_font).boundingRect(uploadStr).size().width(),
                        QFontMetrics(m_font).boundingRect(downloadStr).size().width()) > width()) {
            m_font.setPixelSize(m_font.pixelSize() - 1);
        }
    }
    return QSize(std::max(QFontMetrics(m_font).boundingRect(uploadStr).size().width(),
                          QFontMetrics(m_font).boundingRect(downloadStr).size().width()),
                 QFontMetrics(m_font).boundingRect(uploadStr).size().height() +
                 QFontMetrics(m_font).boundingRect(downloadStr).size().height());
}

QString DDENetspeedTextWidget::getUploadBpsStr() {
    QString uploadStr = this->engLocale.formattedDataSize(this->uploadBps, 1, QLocale::DataSizeSIFormat);
    uploadStr = this->shortenDataSizeStr(uploadStr);
    return QString("↑ ") + uploadStr;
}

QString DDENetspeedTextWidget::getDownloadBpsStr() {
    QString downloadStr = this->engLocale.formattedDataSize(this->downloadBps, 1, QLocale::DataSizeSIFormat);
    downloadStr = this->shortenDataSizeStr(downloadStr);
    return QString("↓ ") + downloadStr;
}
