//
// Created by septemberhx on 2020/5/21.
//

#include <constants.h>
#include "DDENetspeedTextWidget2.h"

QString DDENetspeedTextWidget2::shortenDataSizeStr(QString dataSizeStr)
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

void DDENetspeedTextWidget2::setUpAndDownBps(qreal uploadBps, qreal downloadBps) {
    this->uploadBps = uploadBps;
    this->downloadBps = downloadBps;
    this->m_upLabel->setText(this->getUploadBpsStr());
    this->m_downLabel->setText(this->getDownloadBpsStr());
    this->m_upLabel->adjustSize();
    this->m_downLabel->adjustSize();
}

DDENetspeedTextWidget2::DDENetspeedTextWidget2(QWidget *parent) : QWidget(parent) {

    setMinimumSize(PLUGIN_BACKGROUND_MIN_SIZE, PLUGIN_BACKGROUND_MIN_SIZE);

    this->m_upLabel = new QLabel(this);
    this->m_downLabel = new QLabel(this);
    this->m_upLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->m_upLabel->setMargin(0);
    this->m_downLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    this->m_downLabel->setMargin(0);
    this->m_upLabel->setContentsMargins(0,0,0,0);
    this->m_downLabel->setContentsMargins(0, 0, 0, 0);

    this->m_upLabel->setFrameShape(QFrame::Panel);
    this->m_downLabel->setFrameShape(QFrame::Panel);

    this->m_layout = new QVBoxLayout(this);
    this->setLayout(this->m_layout);
    this->m_layout->setMargin(0);
    this->m_layout->setSpacing(0);
    this->m_layout->setContentsMargins(0, 0, 0, 0);

    this->m_layout->addWidget(this->m_upLabel);
    this->m_layout->addWidget(this->m_downLabel);
    this->m_layout->setMargin(5);

    this->m_upLabel->setStyleSheet("QLabel { color : dark; }");
    this->m_downLabel->setStyleSheet("QLabel { color : dark; }");
}

QString DDENetspeedTextWidget2::getUploadBpsStr() {
    QString uploadStr = this->engLocale.formattedDataSize(this->uploadBps, 1, QLocale::DataSizeSIFormat);
    uploadStr = this->shortenDataSizeStr(uploadStr);
    return QString("↑ ") + uploadStr;
}

QString DDENetspeedTextWidget2::getDownloadBpsStr() {
    QString downloadStr = this->engLocale.formattedDataSize(this->downloadBps, 1, QLocale::DataSizeSIFormat);
    downloadStr = this->shortenDataSizeStr(downloadStr);
    return QString("↓ ") + downloadStr;
}
