//
// Created by septemberhx on 2020/6/24.
//

#ifndef DDE_ISTATE_MENUS_DDESENSORTEMPANDPOWERITEMWIDGET_H
#define DDE_ISTATE_MENUS_DDESENSORTEMPANDPOWERITEMWIDGET_H

#include <QLabel>

class DDESensorTempAndPowerItemWidget : public QLabel {
    Q_OBJECT

public:
    explicit DDESensorTempAndPowerItemWidget(QWidget *parent = nullptr);
    void setTempAndPower(qreal percent, qreal powerConsumption);
    void setTemp(qreal temp);
    void setPower(qreal power);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal temperature;
    qreal powerConsumption;
};


#endif //DDE_ISTATE_MENUS_DDESENSORTEMPANDPOWERITEMWIDGET_H
