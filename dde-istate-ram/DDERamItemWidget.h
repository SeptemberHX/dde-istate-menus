//
// Created by septemberhx on 2020/6/13.
//

#ifndef DDE_ISTATE_MENUS_DDERAMITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDERAMITEMWIDGET_H


#include <QLabel>

class DDERamItemWidget : public QLabel {

    Q_OBJECT

public:
    explicit DDERamItemWidget(QWidget *parent = nullptr);

    void setPercent(qreal percent);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal percent;  // [0, 100]
    quint8 maxValue;
    quint8 width;
};


#endif //DDE_ISTATE_MENUS_DDERAMITEMWIDGET_H
