//
// Created by septemberhx on 2020/6/9.
//

#ifndef DDE_ISTATE_MENUS_DDECPUPLUGIN_H
#define DDE_ISTATE_MENUS_DDECPUPLUGIN_H


#include <dde-dock/pluginsiteminterface.h>
#include "../DDEIstateSubPlugin.h"
#include "DDECpuItemWidget.h"
#include "../dde-istate-widgets/istatecpuwidget.h"
#include "../utils/process_entry.h"
#include <QLabel>
#include <QWidget>


class DDECpuPlugin : public QObject, public PluginsItemInterface, public DDEIstateSubPlugin {
    Q_OBJECT

    Q_INTERFACES(PluginsItemInterface)

public:
    explicit DDECpuPlugin(QObject *parent = 0);

    void addCpuUsage(qreal usage, cpu_usage avgUsage, QList<cpu_usage> cpuUsageList);
    void updateProcesses(QList<ProcessEntry> entryList);
    void updateUptime(qulonglong uptime);

    // 返回插件的名称，必须是唯一值，不可以和其它插件冲突
    const QString pluginName() const override;

    // 插件初始化函数
    void init(PluginProxyInterface *proxyInter) override;

    // 返回插件的 widget
    QWidget *itemWidget(const QString &itemKey) override;

    QWidget *itemTipsWidget(const QString &itemKey) override;

    QWidget *itemPopupApplet(const QString &itemKey) override;

    void pluginStateSwitched() override;
    const QString pluginDisplayName() const override;
    void pluginSettingsChanged() override;

    int itemSortKey(const QString &itemKey) Q_DECL_OVERRIDE;
    void setSortKey(const QString &itemKey, const int order) Q_DECL_OVERRIDE;

    void refreshData() override;

    void setRefreshInterval(int msec) override;

private:
    DDECpuItemWidget *m_pluginWidget;
    QLabel *m_tipWidget;
    IstateCpuWidget *m_popupWidget;
};


#endif //DDE_ISTATE_MENUS_DDECPUPLUGIN_H
