//
// Created by septemberhx on 2020/6/22.
//

#ifndef DDE_ISTATE_MENUS_DDESENSORITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDESENSORITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "DDESensorTempAndPowerItemWidget.h"

class DDESensorItemWidget : public QWidget {

    Q_OBJECT

public:
    explicit DDESensorItemWidget(QWidget* parent = nullptr);

    void setTemp(qreal percent);
    void setPower(qreal power);
    void changeIconTheme(bool dark);

private:
    QHBoxLayout *m_layout;
    DDESensorTempAndPowerItemWidget *m_plotWidget;
    QLabel *m_label;
};


#endif //DDE_ISTATE_MENUS_DDESENSORITEMWIDGET_H
