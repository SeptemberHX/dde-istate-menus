//
// Created by septemberhx on 2020/5/13.
//

#ifndef DDE_PSTATE_DDEPSTATEPLUGIN_H
#define DDE_PSTATE_DDEPSTATEPLUGIN_H

#include "DDEIstateMenuPlugin.h"
#include "dde-istate-netspeed/DDENetspeedPlugin.h"
#include "dde-istate-datetime/datetimeplugin.h"
#include "dde-istate-ram/DDERamPlugin.h"
#include "dde-istate-cpu/DDECpuPlugin.h"
#include "dde-istate-sensor/DDESensorPlugin.h"
#include "utils/stats_collector.h"
#include <dde-dock/pluginsiteminterface.h>
#include <QObject>
#include <QLabel>
#include <QThread>
#include <QWidget>
#include <QHBoxLayout>
#include <QMap>

class DDEIstateMenuPlugin : public QObject, public PluginsItemInterface {
    Q_OBJECT

    // 声明实现了的接口
    Q_INTERFACES(PluginsItemInterface)
    // 插件元数据
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "dde_istate_menus.json")

public:
    explicit DDEIstateMenuPlugin(QObject *parent = nullptr);

    // 返回插件的名称，必须是唯一值，不可以和其它插件冲突
    const QString pluginName() const override;

    // 插件初始化函数
    void init(PluginProxyInterface *proxyInter) override;
    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemPopupApplet(const QString &itemKey) override;

    bool pluginIsAllowDisable() override;
    bool pluginIsDisable() override;

    void pluginStateSwitched() override;

    const QString pluginDisplayName() const override;
    void pluginSettingsChanged() override;

    void reloadSettings();

    bool eventFilter(QObject *watched, QEvent *event) override;

//    int itemSortKey(const QString &itemKey) Q_DECL_OVERRIDE;
//    void setSortKey(const QString &itemKey, const int order) Q_DECL_OVERRIDE;

private slots:
    void updateProcessList(const QList<ProcessEntry> procList);
    void updateCpuUsage(qreal cpuPercent, const QList<double> cpuPercents, cpu_usage separatorUsage, QList<cpu_usage> cpuUsageList);
    void updateTempInfo(QList<TempInfo> tempInfoList);

private:
    DDENetspeedPlugin *netspeedPlugin;
    DatetimePlugin *datetimePlugin;
    DDECpuPlugin *cpuPlugin;
    DDERamPlugin *ramPlugin;
    DDESensorPlugin *sensorPlugin;

    QWidget *p_itemWidget;
    QWidget *popupWidget;
    QHBoxLayout *itemLayout;

    PowerConsumption prevPc {};
};


#endif //DDE_PSTATE_DDEPSTATEPLUGIN_H
