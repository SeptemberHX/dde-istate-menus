//
// Created by septemberhx on 2020/6/22.
//

#ifndef DDE_ISTATE_MENUS_DDESENSORPLUGIN_H
#define DDE_ISTATE_MENUS_DDESENSORPLUGIN_H

#include <dde-dock/pluginsiteminterface.h>
#include "../DDEIstateSubPlugin.h"
#include "../dde-istate-widgets/istatesensorwidget.h"
#include "DDESensorItemWidget.h"

class DDESensorPlugin : public QObject, public PluginsItemInterface, public DDEIstateSubPlugin {
    Q_OBJECT

    Q_INTERFACES(PluginsItemInterface)

public:
    explicit DDESensorPlugin(QObject *parent = 0);
    void setCpuTemp(qreal temp);
    void setPower(qreal power);
    void updateTempInfos(QList<TempInfo> infoList);
    void updatePowerConsumption(PowerConsumption pc);

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
    DDESensorItemWidget *m_pluginWidget;
    QLabel *m_tipWidget;
    IstateSensorWidget *m_popupWidget;
};

#endif //DDE_ISTATE_MENUS_DDESENSORPLUGIN_H
