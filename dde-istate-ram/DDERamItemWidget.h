//
// Created by septemberhx on 2020/6/21.
//

#ifndef DDE_ISTATE_MENUS_DDERAMITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDERAMITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "DDERamBarItemWidget.h"

class DDERamItemWidget : public QWidget {

    Q_OBJECT

public:
    explicit DDERamItemWidget(QWidget *parent = nullptr);
    void setPercent(qreal percent);
    void changeIconTheme(bool dark);

private:
    QHBoxLayout *m_layout;
    DDERamBarItemWidget *m_plotWidget;
    QLabel *m_label;
};


#endif //DDE_ISTATE_MENUS_DDERAMITEMWIDGET_H
