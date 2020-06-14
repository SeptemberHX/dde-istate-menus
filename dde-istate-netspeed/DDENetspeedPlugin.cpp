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

    this->m_speedTextWidget = new DDENetspeedTextWidget();
    this->m_speedTextWidget2 = new DDENetspeedTextWidget2();
    this->m_pluginWidget = new QLabel();
    this->m_tipsWidget = new QLabel();
    this->m_appletWidget = new IstateNetworkWidget();
    this->m_timer = new QTimer(this);
    this->m_timer->setInterval(this->intervalInMSec);
    connect(this->m_timer, &QTimer::timeout, this, &DDENetspeedPlugin::refreshData);
    this->m_timer->start();
}

QWidget *DDENetspeedPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    this->m_pluginWidget->setText("Plugin widget");
    return this->m_speedTextWidget;
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
    QMap<QString, QString> netIpv4Map = DDEUtils::netIpv4Map();

//    if (this->defaultNetDeviceName.isEmpty()) {
        this->determineNetDevice(dataMap);
//    }

    if (!this->defaultNetDeviceName.isEmpty()) {
        if (dataMap.contains(this->defaultNetDeviceName)) {
            qreal uploadBps = (dataMap[this->defaultNetDeviceName].second - this->lastOutDataSize) / (this->intervalInMSec / 1000.0);
            qreal downloadBps = (dataMap[this->defaultNetDeviceName].first - this->lastInDataSize) / (this->intervalInMSec / 1000.0);
            if (this->lastInDataSize != 0) {
                this->m_appletWidget->appendBps(uploadBps, downloadBps);
            }
            this->m_speedTextWidget->setUpAndDownBps(uploadBps, downloadBps);
            this->m_speedTextWidget2->setUpAndDownBps(uploadBps, downloadBps);

            this->lastInDataSize = dataMap[this->defaultNetDeviceName].first;
            this->lastOutDataSize = dataMap[this->defaultNetDeviceName].second;

            QString ipv4 = "-";
            if (netIpv4Map.contains(this->defaultNetDeviceName)) {
                ipv4 = netIpv4Map[this->defaultNetDeviceName];
            }
            this->m_appletWidget->setCurveDevice(this->defaultNetDeviceName, ipv4);
        }
    } else {
        this->m_appletWidget->appendBps(0, 0);
        this->m_appletWidget->setCurveDevice("-", "-");
    }
    this->m_appletWidget->updateStatistics(dataMap, netIpv4Map);
}

void DDENetspeedPlugin::setRefreshInterval(int msec) {
    this->intervalInMSec = msec;
    this->m_timer->setInterval(msec);
    this->m_timer->start();
}

void DDENetspeedPlugin::updateProcesses(QList<ProcessEntry> entryList) {
    this->m_appletWidget->updateProcesses(entryList);
}

void DDENetspeedPlugin::determineNetDevice(QMap<QString, QPair<ulong, ulong>> dataMap) {
    QList<QString> interfaceList(dataMap.keys());
    std::sort(interfaceList.begin(), interfaceList.end(), [dataMap](const QString &s1, const QString &s2) {
        return dataMap[s1].first + dataMap[s1].second > dataMap[s2].first + dataMap[s2].second;
    });

    for (const QString& devName : interfaceList) {
        if (devName.startsWith("wlan") || devName.startsWith("wlp")
            || devName.startsWith("eth") || devName.startsWith("en")) {
            this->defaultNetDeviceName = devName;
            break;
        }
    }
}
