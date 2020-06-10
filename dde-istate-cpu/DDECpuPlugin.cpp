//
// Created by septemberhx on 2020/6/9.
//

#include "DDECpuPlugin.h"

#define PLUGIN_STATE_KEY "enable"

DDECpuPlugin::DDECpuPlugin(QObject *parent) : QObject(parent)
{
    this->m_tipWidget = new QLabel();
    this->m_popupWidget = new IstateCpuWidget();
    this->m_pluginWidget = new DDECpuItemWidget();
}


void DDECpuPlugin::init(PluginProxyInterface *proxyInter) {
    m_proxyInter = proxyInter;

    if (pluginIsDisable()) {
        return;
    }
}

// --------------------------- CHANGE NAME --------------------------------

const QString DDECpuPlugin::pluginDisplayName() const {
    return QStringLiteral("DDE Istate CPU");
}

const QString DDECpuPlugin::pluginName() const {
    return QStringLiteral("dde-istate-cpu");
}

// --------------------------- NO NEED TO CHANGE --------------------------------

int DDECpuPlugin::itemSortKey(const QString &itemKey) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    return m_proxyInter->getValue(this, key, 5).toInt();
}

void DDECpuPlugin::setSortKey(const QString &itemKey, const int order) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

QWidget *DDECpuPlugin::itemPopupApplet(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_popupWidget;
}

QWidget *DDECpuPlugin::itemTipsWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_tipWidget;
}

QWidget *DDECpuPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->m_pluginWidget;
}

void DDECpuPlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, PLUGIN_STATE_KEY, pluginIsDisable());

    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDECpuPlugin::pluginSettingsChanged() {
    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDECpuPlugin::refreshData() {

}

void DDECpuPlugin::setRefreshInterval(int msec) {

}

void DDECpuPlugin::addCpuUsage(qreal usage, cpu_usage avgUsage, QList<cpu_usage> cpuUsageList) {
    this->m_pluginWidget->addData(usage);
    this->m_popupWidget->addCpuUsage(avgUsage, cpuUsageList);
}

void DDECpuPlugin::updateProcesses(QList<ProcessEntry> entryList) {
    this->m_popupWidget->updateProcesses(entryList);
}

void DDECpuPlugin::updateUptime(qulonglong uptime) {
    this->m_popupWidget->updateUptime(uptime);
}
