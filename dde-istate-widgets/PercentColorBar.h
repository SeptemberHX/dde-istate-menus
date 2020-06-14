//
// Created by septemberhx on 2020/6/14.
//

#ifndef DDE_ISTATE_MENUS_PERCENTCOLORBAR_H
#define DDE_ISTATE_MENUS_PERCENTCOLORBAR_H


#include <QLabel>
#include <QMap>

class PercentColorBar : public QLabel {

    Q_OBJECT

public:
    explicit PercentColorBar(QWidget *parent = nullptr);

    void setPercents(QMap<QString, qreal> percentMap, QMap<QString, QColor> colorMap, QList<QString> orderList);

protected:
    void paintEvent(QPaintEvent *event) override;

    QMap<QString, qreal> currPercentMap;
    QMap<QString, QColor> currColorMap;
    QList<QString> currOrderList;
    QColor defaultColor;
};


#endif //DDE_ISTATE_MENUS_PERCENTCOLORBAR_H
