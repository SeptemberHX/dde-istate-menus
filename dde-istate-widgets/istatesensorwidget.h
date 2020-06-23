#ifndef ISTATESENSORWIDGET_H
#define ISTATESENSORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "../dde-istate-sensor/DDESensorTempItemWidget.h"
#include "../utils/system_stat.h"
#include "../utils/rapl_read.h"

namespace Ui {
class IstateSensorWidget;
}

class IstateSensorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IstateSensorWidget(QWidget *parent = nullptr);
    ~IstateSensorWidget();
    void updateTempInfos(QList<TempInfo> infoList);
    void updatePowerConsumption(PowerConsumption pc);

private:
    void redrawTemp();
    void redrawPowerC();

protected:
    void showEvent(QShowEvent *event) override;

private:

    Ui::IstateSensorWidget *ui;
    QVBoxLayout *m_tempLayout;

    QList<TempInfo> infoList;
    PowerConsumption pc;
};

#endif // ISTATESENSORWIDGET_H
