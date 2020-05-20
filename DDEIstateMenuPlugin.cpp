//
// Created by septemberhx on 2020/5/13.
//

#include "DDEIstateMenuPlugin.h"
#include "utils/NetworkTrafficFilter.h"
#include <QJsonObject>

#define PLUGIN_STATE_KEY "enable"

DDEIstateMenuPlugin::DDEIstateMenuPlugin(QObject *parent) : QObject(parent) {

}

const QString DDEIstateMenuPlugin::pluginName() const {
    return QStringLiteral("dde_istate_menus");
}

void DDEIstateMenuPlugin::init(PluginProxyInterface *proxyInter) {
    this->m_proxyInter = proxyInter;
    this->netspeedPlugin = new DDENetspeedPlugin();
    this->netspeedPlugin->init(this->m_proxyInter);
    this->m_itemWidget = new QLabel();

    this->m_refreshTimer = new QTimer(this);
    this->m_refreshTimer->setInterval(2000);
    connect(this->m_refreshTimer, &QTimer::timeout, this, &DDEIstateMenuPlugin::fetchSystemData);
    this->m_refreshTimer->start();

    std::thread nethogs_monitor_thread(&NetworkTrafficFilter::nethogsMonitorThreadProc);
    nethogs_monitor_thread.detach();

    if (!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, this->pluginName());
        this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
    }
}

bool DDEIstateMenuPlugin::pluginIsAllowDisable() {
    return true;
}

bool DDEIstateMenuPlugin::pluginIsDisable() {
    return !(m_proxyInter->getValue(this, PLUGIN_STATE_KEY, true).toBool());
}

void DDEIstateMenuPlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, PLUGIN_STATE_KEY, pluginIsDisable());
    m_proxyInter->saveValue(this->netspeedPlugin, PLUGIN_STATE_KEY, pluginIsDisable());

    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        m_proxyInter->itemRemoved(this->netspeedPlugin, this->netspeedPlugin->pluginName());
        return;
    }

    m_proxyInter->itemAdded(this, pluginName());
    this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
}

const QString DDEIstateMenuPlugin::pluginDisplayName() const {
    return QStringLiteral("DDE Istate Menus");
}

int DDEIstateMenuPlugin::itemSortKey(const QString &itemKey) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    return m_proxyInter->getValue(this, key, 5).toInt();
}

void DDEIstateMenuPlugin::setSortKey(const QString &itemKey, const int order) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

void DDEIstateMenuPlugin::pluginSettingsChanged() {
    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    m_proxyInter->itemAdded(this, pluginName());
}

QWidget *DDEIstateMenuPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    this->m_itemWidget->setText("Item widget");
    return this->m_itemWidget;
}

void DDEIstateMenuPlugin::fetchSystemData() {
    NetworkTrafficFilter::Update update {};
    while (NetworkTrafficFilter::getRowUpdate(update)) {
        if (update.action != NETHOGS_APP_ACTION_REMOVE) {
            auto pid = update.record.pid;
            auto recvBps = qreal(update.record.recv_kbs) * 1024;
            auto sentBps = qreal(update.record.sent_kbs) * 1024;
            auto recvBytes = update.record.recv_bytes;
            auto sentBytes = update.record.sent_bytes;

//            qDebug() << "====>> pid:" << pid << recvBps << sentBps << recvBytes << sentBytes;
        }
    }

    ProcessStat::readProcStats(readProcStatsCallBack, this);
}

void DDEIstateMenuPlugin::readProcStatsCallBack(ProcStat &ps, void *context) {
    if (!context)
        return;

    auto *ctx = static_cast<DDEIstateMenuPlugin *>(context);
    if (!ctx)
        return;


}
