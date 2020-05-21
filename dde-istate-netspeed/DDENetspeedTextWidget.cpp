//
// Created by septemberhx on 2020/5/21.
//

#include "DDENetspeedTextWidget.h"
#include <DGuiApplicationHelper>
#include <DStyle>
#include <constants.h>
#include <QPainter>
#include <DFontSizeManager>


DWIDGET_USE_NAMESPACE

DDENetspeedTextWidget::DDENetspeedTextWidget(QWidget *parent)
    : QLabel(parent)
{
    this->setMinimumSize(PLUGIN_BACKGROUND_MIN_SIZE, PLUGIN_BACKGROUND_MIN_SIZE);
    this->setFixedSize(40, 30);
    this->curSize();
    this->m_font = DFontSizeManager::instance()->t10();
}

QSize DDENetspeedTextWidget::sizeHint() const {
    return QSize(40, 30);
}

void DDENetspeedTextWidget::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setFont(this->m_font);
    painter.setPen(QPen(palette().brightText(), 1));

    QRect uploadRect = rect();
    QRect downloadRect = rect();

    uploadRect.setBottom(rect().center().y() + 4);
    downloadRect.setTop(uploadRect.bottom() - 4);

    painter.drawText(uploadRect, Qt::AlignBottom | Qt::AlignHCenter, getUploadBpsStr());
    painter.drawText(downloadRect, Qt::AlignTop | Qt::AlignHCenter, getDownloadBpsStr());
}

QString DDENetspeedTextWidget::shortenDataSizeStr(QString dataSizeStr) const
{
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

    return tmpStr;
}

void DDENetspeedTextWidget::setUpAndDownBps(qreal uploadBps, qreal downloadBps) {
    this->uploadBps = uploadBps;
    this->downloadBps = downloadBps;
    this->repaint();
}

QSize DDENetspeedTextWidget::curSize() const {
    QLocale tmpLocal = QLocale(QLocale::English, QLocale::UnitedStates);

    this->m_font = DFontSizeManager::instance()->t10();
    QFontMetrics fm(this->m_font);

    QString uploadStr = tmpLocal.formattedDataSize(this->uploadBps, 1, QLocale::DataSizeSIFormat);
    uploadStr = QString("↑ ") + shortenDataSizeStr(uploadStr);

    QString downloadStr = tmpLocal.formattedDataSize(this->uploadBps, 1, QLocale::DataSizeSIFormat);
    downloadStr = QString("↑ ") + shortenDataSizeStr(downloadStr);

    while (QFontMetrics(m_font).boundingRect(uploadStr).size().height() +
           QFontMetrics(m_font).boundingRect(downloadStr).size().height() > height() ||
           std::max(QFontMetrics(m_font).boundingRect(uploadStr).size().width(),
                    QFontMetrics(m_font).boundingRect(downloadStr).size().width()) > width()) {
        m_font.setPixelSize(m_font.pixelSize() - 1);
    }
    return this->size();
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
