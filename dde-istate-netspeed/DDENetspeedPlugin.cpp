//
// Created by septemberhx on 2020/5/19.
//

#include "DDENetspeedPlugin.h"
#include <QJsonObject>
#include "../utils/DDEUtils.h"

#define PLUGIN_STATE_KEY "enable"

DDENetspeedPlugin::DDENetspeedPlugin(QObject *parent) : QObject(parent) {

}

const QString DDENetspeedPlugin::pluginName() const {
    return QStringLiteral("dde_istate_netspeed");
}

void DDENetspeedPlugin::init(PluginProxyInterface *proxyInter) {
    this->lastInDataSize = 0;
    this->lastOutDataSize = 0;
    this->m_proxyInter = proxyInter;

    this->m_pluginWidget = new QLabel();
    this->m_tipsWidget = new QLabel();
    this->m_appletWidget = new IstateNetworkWidget();
    this->m_timer = new QTimer(this);
    this->m_timer->setInterval(this->intervalInMSec);
    this->m_appletWidget->setTimeInterval(this->intervalInMSec);
    connect(this->m_timer, &QTimer::timeout, this, &DDENetspeedPlugin::refreshData);
    this->m_timer->start();
}

QWidget *DDENetspeedPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    this->m_pluginWidget->setText("Plugin widget");
    return this->m_pluginWidget;
}

void DDENetspeedPlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, PLUGIN_STATE_KEY, pluginIsDisable());

    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

const QString DDENetspeedPlugin::pluginDisplayName() const {
    return QStringLiteral("DDE Istate Netspeed");
}

QWidget *DDENetspeedPlugin::itemTipsWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    this->m_tipsWidget->setText("Tips widget");
    DDEUtils::currNetInOutBytes();
    return m_tipsWidget;
}

QWidget *DDENetspeedPlugin::itemPopupApplet(const QString &itemKey) {
    Q_UNUSED(itemKey)

    return this->m_appletWidget;
}

int DDENetspeedPlugin::itemSortKey(const QString &itemKey) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    return m_proxyInter->getValue(this, key, 5).toInt();
}

void DDENetspeedPlugin::setSortKey(const QString &itemKey, const int order) {
    Q_UNUSED(itemKey)

    const QString key = QString("pos_%1").arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

void DDENetspeedPlugin::pluginSettingsChanged() {
    if (pluginIsDisable()) {
        m_proxyInter->itemRemoved(this, pluginName());
        return;
    }

    if (m_pluginWidget) {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

void DDENetspeedPlugin::refreshData() {
    QMap<QString, QPair<ulong, ulong>> dataMap = DDEUtils::currNetInOutBytes();
    QString netName = "wlp0s20f3";
    if (dataMap.contains(netName)) {
        if (this->lastInDataSize != 0) {
            this->m_appletWidget->appendSpeed(dataMap[netName].second - this->lastOutDataSize,
                                              dataMap[netName].first - this->lastInDataSize);
        }
        this->lastInDataSize = dataMap[netName].first;
        this->lastOutDataSize = dataMap[netName].second;
    }
}

void DDENetspeedPlugin::setRefreshInterval(int msec) {
    this->intervalInMSec = msec;
    this->m_timer->setInterval(msec);
    this->m_appletWidget->setTimeInterval(msec);
    this->m_timer->start();
}
