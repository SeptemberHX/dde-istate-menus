//
// Created by septemberhx on 2020/6/10.
//

#ifndef DDE_ISTATE_MENUS_DDECPUITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDECPUITEMWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include "DDECpuPlotItemWidget.h"

class DDECpuItemWidget : public QWidget {

    Q_OBJECT

public:
    explicit DDECpuItemWidget(QWidget *parent = nullptr);
    void addData(qreal usagePer);
    void changeIconTheme(bool dark);

private:
    QHBoxLayout *m_layout;
    DDECpuPlotItemWidget *m_plotWidget;
    QLabel *m_label;
};


#endif //DDE_ISTATE_MENUS_DDECPUITEMWIDGET_H
