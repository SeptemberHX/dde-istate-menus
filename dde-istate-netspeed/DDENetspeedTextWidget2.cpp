//
// Created by septemberhx on 2020/5/21.
//

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
}

DDENetspeedTextWidget2::DDENetspeedTextWidget2(QWidget *parent) : QWidget(parent) {
    this->m_upLabel = new QLabel(this);
    this->m_downLabel = new QLabel(this);
    this->m_upLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    this->m_downLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    this->m_layout = new QVBoxLayout(this);
    this->setLayout(this->m_layout);
    this->m_layout->setMargin(0);
    this->m_layout->setSpacing(0);

    this->m_layout->addWidget(this->m_upLabel);
    this->m_layout->addWidget(this->m_downLabel);
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
