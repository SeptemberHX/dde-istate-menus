#ifndef ISTATESENSORWIDGET_H
#define ISTATESENSORWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "../dde-istate-sensor/DDESensorTempItemWidget.h"
#include "../utils/system_stat.h"

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

private:
    void redrawTemp();

protected:
    void showEvent(QShowEvent *event) override;

private:

    Ui::IstateSensorWidget *ui;
    QVBoxLayout *m_tempLayout;

    QList<TempInfo> infoList;
};

#endif // ISTATESENSORWIDGET_H
