//
// Created by septemberhx on 2020/6/11.
//

#ifndef DDE_ISTATE_MENUS_DDECPUTIPWIDGET_H
#define DDE_ISTATE_MENUS_DDECPUTIPWIDGET_H

#include <QLabel>
#include "../utils/system_stat.h"

class DDECpuTipWidget : public QLabel {

    Q_OBJECT

public:
    explicit DDECpuTipWidget(QWidget *parent = nullptr);
    void updateCpuUsage(cpu_usage usage);
    void updateLoad(qreal avg1, qreal avg5, qreal avg15);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void redrawTip();

private:
    cpu_usage cpuUsage;
    qreal loadAvg1;
    qreal loadAvg5;
    qreal loadAvg15;
};


#endif //DDE_ISTATE_MENUS_DDECPUTIPWIDGET_H
