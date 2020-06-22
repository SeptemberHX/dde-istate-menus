//
// Created by septemberhx on 2020/6/22.
//

#ifndef DDE_ISTATE_MENUS_DDESENSORITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDESENSORITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class DDESensorItemWidget : public QWidget {

    Q_OBJECT

public:
    explicit DDESensorItemWidget(QWidget* parent = nullptr);

    void setTemp(qreal percent);

private:
    QHBoxLayout *m_layout;
    QLabel *m_plotWidget;
    QLabel *m_label;
};


#endif //DDE_ISTATE_MENUS_DDESENSORITEMWIDGET_H
