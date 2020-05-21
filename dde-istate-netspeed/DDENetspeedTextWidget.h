//
// Created by septemberhx on 2020/5/21.
//

#ifndef DDE_ISTATE_MENUS_DDENETSPEEDTEXTWIDGET_H
#define DDE_ISTATE_MENUS_DDENETSPEEDTEXTWIDGET_H


#include <QLabel>
#include <QLocale>

class DDENetspeedTextWidget : public QLabel {
    Q_OBJECT

public:
    explicit DDENetspeedTextWidget(QWidget *parent = 0);
    QSize sizeHint() const override;
    void setUpAndDownBps(qreal uploadBps, qreal downloadBps);

private:
    void paintEvent(QPaintEvent *e) override;
    QString shortenDataSizeStr(QString dataSizeStr) const;
    QSize curSize() const;

    QString getUploadBpsStr();
    QString getDownloadBpsStr();

    mutable QFont m_font;
    mutable int m_offset;

    qreal downloadBps = 0;
    qreal uploadBps = 0;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);
};


#endif //DDE_ISTATE_MENUS_DDENETSPEEDTEXTWIDGET_H
