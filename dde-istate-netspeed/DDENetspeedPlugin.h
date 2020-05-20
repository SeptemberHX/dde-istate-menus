//
// Created by septemberhx on 2020/5/19.
//

#ifndef DDE_ISTATE_MENUS_DDENETSPEEDPLUGIN_H
#define DDE_ISTATE_MENUS_DDENETSPEEDPLUGIN_H

#include "../DDEIstateSubPlugin.h"
#include "../dde-istate-widgets/istatenetworkwidget.h"
#include <dde-dock/pluginsiteminterface.h>
#include <QObject>
#include <QLabel>
#include <QTimer>


class DDENetspeedPlugin : public QObject, public PluginsItemInterface, public DDEIstateSubPlugin {
    Q_OBJECT

    // 声明实现了的接口
    Q_INTERFACES(PluginsItemInterface)
    // 插件元数据
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "dde_istate_netspeed.json")

public:
    explicit DDENetspeedPlugin(QObject *parent = nullptr);

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
    QLabel *m_pluginWidget;
    IstateNetworkWidget *m_appletWidget;
    QLabel *m_tipsWidget;
    QTimer *m_timer;
    uint lastInDataSize;
    uint lastOutDataSize;
    int intervalInMSec = 2000;
};


#endif //DDE_ISTATE_MENUS_DDENETSPEEDPLUGIN_H
