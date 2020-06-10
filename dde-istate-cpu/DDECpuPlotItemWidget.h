//
// Created by septemberhx on 2020/6/9.
//

#ifndef DDE_ISTATE_MENUS_DDECPUPLOTITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDECPUPLOTITEMWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QChartView>
#include <QHBoxLayout>


class DDECpuPlotItemWidget : public QLabel {

    Q_OBJECT

public:
    explicit DDECpuPlotItemWidget(QWidget *parent = nullptr);
    void addData(qreal usagePer);

    QSize sizeHint() const override;

private:
    int historySize;
    int maxValue;
    QList<qreal> valueList;

protected:
    void paintEvent(QPaintEvent *event) override;
};


#endif //DDE_ISTATE_MENUS_DDECPUPLOTITEMWIDGET_H
