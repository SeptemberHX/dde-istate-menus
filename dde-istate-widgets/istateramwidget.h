#ifndef ISTATERAMWIDGET_H
#define ISTATERAMWIDGET_H

#include <QWidget>
#include <QMap>
#include <QLocale>
#include "../utils/system_stat.h"

namespace Ui {
class IstateRamWidget;
}

class IstateRamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IstateRamWidget(QWidget *parent = nullptr);
    ~IstateRamWidget();

    void updateMemStat(mem_stat memStat);

    void initRamWidget();
    void redrawRamUsage();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::IstateRamWidget *ui;

    QColor memUsedColor;
    QColor memBuffersColor;
    QColor memCachedColor;
    QColor memFreeColor;

    QMap<QString, QColor> ramColorMap;
    QList<QString> ramItemList;

    mem_stat currMemStat;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);
};

#endif // ISTATERAMWIDGET_H
