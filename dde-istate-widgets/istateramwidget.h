#ifndef ISTATERAMWIDGET_H
#define ISTATERAMWIDGET_H

#include <QWidget>
#include <QMap>
#include <QLocale>
#include "../utils/system_stat.h"
#include "../utils/process_entry.h"

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
    void updateProcessList(QList<ProcessEntry> processEntries);
    void initRamWidget();
    void initActiveWidget();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void redrawRamUsage();
    void redrawProcesses();
    void redrawSwapUsage();
    void redrawActiveUsage();

    Ui::IstateRamWidget *ui;

    QColor memUsedColor;
    QColor memBuffersColor;
    QColor memCachedColor;
    QColor memFreeColor;

    QColor swapUsedColor;
    QColor swapFreeColor;

    QColor activeAnonColor;
    QColor inactiveAnonColor;
    QColor activeFileColor;
    QColor inactiveFileColor;

    QMap<QString, QColor> ramColorMap;
    QList<QString> ramItemList;

    QMap<QString, QColor> swapColorMap;
    QList<QString> swapItemList;

    QMap<QString, QColor> activeColorMap;
    QList<QString> activeItemList;

    mem_stat currMemStat;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);

    QList<ProcessEntry> entries;
};

#endif // ISTATERAMWIDGET_H
