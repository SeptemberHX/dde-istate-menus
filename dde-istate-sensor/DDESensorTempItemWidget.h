//
// Created by septemberhx on 2020/6/22.
//

#ifndef DDE_ISTATE_MENUS_DDESENSORTEMPITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDESENSORTEMPITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include "../dde-istate-widgets/PercentColorBar.h"
#include <QHBoxLayout>
#include <QColor>

class DDESensorTempItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit DDESensorTempItemWidget(QWidget *parent = nullptr);
    void setText(const QString& text);
    void setTemp(qreal temp);

private:
    QColor tempColor;
    QColor bgColor;
    QMap<QString, QColor> colorMap;
    QList<QString> orderList;

    QHBoxLayout *m_layout;
    QLabel *m_label;
    QLabel *m_tempLabel;
    PercentColorBar *m_perColorBar;
};


#endif //DDE_ISTATE_MENUS_DDESENSORTEMPITEMWIDGET_H
