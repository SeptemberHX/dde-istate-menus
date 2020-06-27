//
// Created by septemberhx on 2020/6/13.
//

#ifndef DDE_ISTATE_MENUS_DDERAMPLUGIN_H
#define DDE_ISTATE_MENUS_DDERAMPLUGIN_H


#include <dde-dock/pluginsiteminterface.h>
#include <QLabel>
#include <QWidget>

#include "../DDEIstateSubPlugin.h"
#include "../dde-istate-widgets/istateramwidget.h"
#include "DDERamItemWidget.h"
#include "../utils/system_stat.h"

class DDERamPlugin : public QObject, public PluginsItemInterface, public DDEIstateSubPlugin {
    Q_OBJECT

    Q_INTERFACES(PluginsItemInterface)

public:
    explicit DDERamPlugin(QObject *parent = nullptr);

    void updateRamInfo(qreal usedPercent, mem_stat memStat);
    void updateProcesses(QList<ProcessEntry> processEntries);

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

    void reloadSettings() override;

private:
    DDERamItemWidget *m_pluginWidget;
    QLabel *m_tipWidget;
    IstateRamWidget *m_popupWidget;
    QLocale engLocale = QLocale(QLocale::English, QLocale::UnitedStates);
};


#endif //DDE_ISTATE_MENUS_DDERAMPLUGIN_H
