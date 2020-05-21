//
// Created by septemberhx on 2020/5/21.
//

#ifndef DDE_ISTATE_MENUS_DDENETSPEEDTEXTWIDGET2_H
#define DDE_ISTATE_MENUS_DDENETSPEEDTEXTWIDGET2_H

#include <QWidget>
#include <QLabel>
#include <QLocale>
#include <QVBoxLayout>

class DDENetspeedTextWidget2 : public QWidget {
    Q_OBJECT

public:
    explicit DDENetspeedTextWidget2(QWidget *parent = 0);
    void setUpAndDownBps(qreal uploadBps, qreal downloadBps);

private:
    QLabel *m_upLabel;
    QLabel *m_downLabel;

    QString getUploadBpsStr();
    QString getDownloadBpsStr();
    QString shortenDataSizeStr(QString dataSizeStr);

    mutable QFont m_font;

    qreal downloadBps = 0;
    qreal uploadBps = 0;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);
    QVBoxLayout *m_layout;
};


#endif //DDE_ISTATE_MENUS_DDENETSPEEDTEXTWIDGET2_H
