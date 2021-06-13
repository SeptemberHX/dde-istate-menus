//
// Created by septemberhx on 2020/5/13.
//

#include "DDEIstateMenuPlugin.h"
#include "utils/network_traffic_filter.h"
#include "DDEIstateMenuSettings.h"
#include <QJsonObject>
#include <QDebug>

#define PLUGIN_STATE_KEY "enable"

DDEIstateMenuPlugin::DDEIstateMenuPlugin(QObject *parent) : QObject(parent) {
    connect(StatsCollector::instance(), &StatsCollector::cpuStatInfoUpdated, this, &DDEIstateMenuPlugin::updateCpuUsage);
    connect(StatsCollector::instance(), &StatsCollector::processListUpdated, this, &DDEIstateMenuPlugin::updateProcessList);
    connect(StatsCollector::instance(), &StatsCollector::uptimeInfoUpdated, this, [this](qulonglong uptime) {
        if (DDEIstateMenuSettings::inst()->isEnableCpu()) {
            this->cpuPlugin->updateUptime(uptime);
        }
    });
    connect(StatsCollector::instance(), &StatsCollector::loadAvgInfoUpdated, this, [this](qreal loadAvg1, qreal loadAvg5, qreal loadAvg15) {
        if (DDEIstateMenuSettings::inst()->isEnableCpu()) {
            this->cpuPlugin->updateLoadAvg(loadAvg1, loadAvg5, loadAvg15);
        }
    });
    connect(StatsCollector::instance(), &StatsCollector::memStatInfoUpdated,
            this,[this](qulonglong usedMemory, qulonglong totalMemory, qulonglong usedSwap, qulonglong totalSwap, mem_stat memStat) {
        if (DDEIstateMenuSettings::inst()->isEnableRam()) {
            this->ramPlugin->updateRamInfo(usedMemory * 100.0 / totalMemory, memStat);
        }
    });
    connect(StatsCollector::instance(), &StatsCollector::tempInfoUpdated, this, &DDEIstateMenuPlugin::updateTempInfo);
    connect(StatsCollector::instance(), &StatsCollector::powerInfoUpdated, this, [this](QList<PowerConsumption> pcList) {
        if (!pcList.isEmpty() && DDEIstateMenuSettings::inst()->isEnableSensors()) {
            PowerConsumption pc = pcList.first();
            PowerConsumption targetPc {};
            targetPc.cores = pc.cores - this->prevPc.cores;
            targetPc.psys = pc.psys - this->prevPc.psys;
            targetPc.dram = pc.dram - this->prevPc.dram;
            targetPc.onCoreGpu = pc.onCoreGpu - this->prevPc.onCoreGpu;
            targetPc.package = pc.package - this->prevPc.package;
            targetPc.timestamp = pc.timestamp - this->prevPc.timestamp;
            this->prevPc = pc;
            this->sensorPlugin->updatePowerConsumption(targetPc);
        }
    });
}

const QString DDEIstateMenuPlugin::pluginName() const {
    return QStringLiteral("dde_istate_menus");
}

void DDEIstateMenuPlugin::init(PluginProxyInterface *proxyInter) {
    this->m_proxyInter = proxyInter;

    this->p_itemWidget = new QWidget();
    this->itemLayout = new QHBoxLayout(this->p_itemWidget);
    this->itemLayout->setMargin(0);

    this->netspeedPlugin = new DDENetspeedPlugin();
    this->netspeedPlugin->init(this->m_proxyInter);
    this->netspeedPlugin->itemWidget("")->installEventFilter(this);

    this->datetimePlugin = new DatetimePlugin();
    this->datetimePlugin->init(this->m_proxyInter);
    this->datetimePlugin->itemWidget("")->installEventFilter(this);

    this->cpuPlugin = new DDECpuPlugin();
    this->cpuPlugin->init(this->m_proxyInter);
    this->cpuPlugin->itemWidget("")->installEventFilter(this);

    this->ramPlugin = new DDERamPlugin();
    this->ramPlugin->init(this->m_proxyInter);
    this->ramPlugin->itemWidget("")->installEventFilter(this);

    this->sensorPlugin = new DDESensorPlugin();
    this->sensorPlugin->init(this->m_proxyInter);
    this->sensorPlugin->itemWidget("")->installEventFilter(this);

    if (!NetworkTrafficFilter::hasInstance) {
        NetworkTrafficFilter::hasInstance = true;
        std::thread nethogs_monitor_thread(&NetworkTrafficFilter::nethogsMonitorThreadProc);
        nethogs_monitor_thread.detach();
    }

    if (!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, this->pluginName());
        // force refresh status to make sure they keep the same as the config file
        m_proxyInter->saveValue(this, PLUGIN_STATE_KEY, true);
        m_proxyInter->saveValue(this->netspeedPlugin, PLUGIN_STATE_KEY, true);
        m_proxyInter->saveValue(this->datetimePlugin, PLUGIN_STATE_KEY, true);
        m_proxyInter->saveValue(this->cpuPlugin, PLUGIN_STATE_KEY, true);
        m_proxyInter->saveValue(this->ramPlugin, PLUGIN_STATE_KEY, true);
        m_proxyInter->saveValue(this->sensorPlugin, PLUGIN_STATE_KEY, true);
        this->reloadSettings();
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
    m_proxyInter->saveValue(this->datetimePlugin, PLUGIN_STATE_KEY, pluginIsDisable());
    m_proxyInter->saveValue(this->cpuPlugin, PLUGIN_STATE_KEY, pluginIsDisable());
    m_proxyInter->saveValue(this->ramPlugin, PLUGIN_STATE_KEY, pluginIsDisable());
    m_proxyInter->saveValue(this->sensorPlugin, PLUGIN_STATE_KEY, pluginIsDisable());

    if (pluginIsDisable()) {
//        m_proxyInter->itemRemoved(this, pluginName());
        m_proxyInter->itemRemoved(this->datetimePlugin, this->datetimePlugin->pluginName());
        m_proxyInter->itemRemoved(this->netspeedPlugin, this->netspeedPlugin->pluginName());
        m_proxyInter->itemRemoved(this->cpuPlugin, this->cpuPlugin->pluginName());
        m_proxyInter->itemRemoved(this->ramPlugin, this->ramPlugin->pluginName());
        m_proxyInter->itemRemoved(this->sensorPlugin, this->ramPlugin->pluginName());
        return;
    }

//    m_proxyInter->itemAdded(this, pluginName());
//    this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->datetimePlugin, this->datetimePlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->cpuPlugin, this->cpuPlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->ramPlugin, this->ramPlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->sensorPlugin, this->sensorPlugin->pluginName());
    this->reloadSettings();
}

const QString DDEIstateMenuPlugin::pluginDisplayName() const {
    return QStringLiteral("DDE Istate Menus");
}

void DDEIstateMenuPlugin::pluginSettingsChanged() {
    if (pluginIsDisable()) {
//        m_proxyInter->itemRemoved(this, pluginName());
        m_proxyInter->itemRemoved(this->datetimePlugin, this->datetimePlugin->pluginName());
        m_proxyInter->itemRemoved(this->netspeedPlugin, this->netspeedPlugin->pluginName());
        m_proxyInter->itemRemoved(this->cpuPlugin, this->cpuPlugin->pluginName());
        m_proxyInter->itemRemoved(this->ramPlugin, this->ramPlugin->pluginName());
        m_proxyInter->itemRemoved(this->sensorPlugin, this->ramPlugin->pluginName());
        return;
    }

//    m_proxyInter->itemAdded(this, pluginName());
//    this->m_proxyInter->itemAdded(this->datetimePlugin, this->datetimePlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->cpuPlugin, this->cpuPlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->ramPlugin, this->ramPlugin->pluginName());
//    this->m_proxyInter->itemAdded(this->sensorPlugin, this->sensorPlugin->pluginName());
    this->reloadSettings();
}

QWidget *DDEIstateMenuPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return p_itemWidget;
}

void DDEIstateMenuPlugin::updateProcessList(QList<ProcessEntry> procList) {
    if (DDEIstateMenuSettings::inst()->isEnableNetwork()) {
        this->netspeedPlugin->updateProcesses(procList);
    }

    if (DDEIstateMenuSettings::inst()->isEnableCpu()) {
        this->cpuPlugin->updateProcesses(procList);
    }

    if (DDEIstateMenuSettings::inst()->isEnableRam()) {
        this->ramPlugin->updateProcesses(procList);
    }
}

void DDEIstateMenuPlugin::updateCpuUsage(qreal cpuPercent, const QList<double> cpuPercents, cpu_usage separatorUsage, QList<cpu_usage> cpuUsageList)
{
    if (DDEIstateMenuSettings::inst()->isEnableCpu()) {
        this->cpuPlugin->addCpuUsage(cpuPercent, separatorUsage, cpuUsageList);
    }
}

void DDEIstateMenuPlugin::updateTempInfo(QList<TempInfo> tempInfoList) {
    if (!DDEIstateMenuSettings::inst()->isEnableSensors()) return;

    std::sort(tempInfoList.begin(), tempInfoList.end(), [](const TempInfo& t1, const TempInfo& t2) {
        return t1.deviceName < t2.deviceName;
    });

    qreal cpuTemp = 0;
    for (const auto& info : tempInfoList) {
        if (info.deviceName.startsWith("Package") || info.deviceName.startsWith("Physical")) {
            cpuTemp = info.deviceTemp;
        }
    }
    this->sensorPlugin->setCpuTemp(cpuTemp);
    this->sensorPlugin->updateTempInfos(tempInfoList);
}

void DDEIstateMenuPlugin::reloadSettings() {
    int c = this->itemLayout->count();
    for (int i = 0; i < c; ++i) {
        this->itemLayout->removeItem(this->itemLayout->itemAt(0));
    }

    if (DDEIstateMenuSettings::inst()->isEnableCpu() != this->cpuPlugin->pluginIsDisable()) {
        if (!this->cpuPlugin->pluginIsDisable()) {
//            this->m_proxyInter->itemAdded(this->cpuPlugin, this->cpuPlugin->pluginName());
//        } else {
//            m_proxyInter->itemRemoved(this->cpuPlugin, this->cpuPlugin->pluginName());
            this->itemLayout->addWidget(this->cpuPlugin->itemWidget(""));
        }
    }
    this->cpuPlugin->reloadSettings();

    if (DDEIstateMenuSettings::inst()->isEnableRam() != this->ramPlugin->pluginIsDisable()) {
        if (!this->ramPlugin->pluginIsDisable()) {
//            this->m_proxyInter->itemAdded(this->ramPlugin, this->ramPlugin->pluginName());
//        } else {
//            m_proxyInter->itemRemoved(this->ramPlugin, this->ramPlugin->pluginName());
            this->itemLayout->addWidget(this->ramPlugin->itemWidget(""));
        }
    }
    this->ramPlugin->reloadSettings();

    if (DDEIstateMenuSettings::inst()->isEnableNetwork() != this->netspeedPlugin->pluginIsDisable()) {
        if (!this->netspeedPlugin->pluginIsDisable()) {
//            this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
//        } else {
//            m_proxyInter->itemRemoved(this->netspeedPlugin, this->netspeedPlugin->pluginName());
            this->itemLayout->addWidget(this->netspeedPlugin->itemWidget(""));
        }
    }
    this->netspeedPlugin->reloadSettings();

    if (DDEIstateMenuSettings::inst()->isEnableSensors() != this->sensorPlugin->pluginIsDisable()) {
        if (!this->sensorPlugin->pluginIsDisable()) {
//            this->m_proxyInter->itemAdded(this->sensorPlugin, this->sensorPlugin->pluginName());
//        } else {
//            m_proxyInter->itemRemoved(this->sensorPlugin, this->sensorPlugin->pluginName());
            this->itemLayout->addWidget(this->sensorPlugin->itemWidget(""));
        }
    }
    this->sensorPlugin->reloadSettings();

    if (DDEIstateMenuSettings::inst()->isEnableDatetime() != this->datetimePlugin->pluginIsDisable()) {
        if (!this->datetimePlugin->pluginIsDisable()) {
//            this->m_proxyInter->itemAdded(this->datetimePlugin, this->datetimePlugin->pluginName());
//        } else {
//            m_proxyInter->itemRemoved(this->datetimePlugin, this->datetimePlugin->pluginName());
            this->itemLayout->addWidget(this->datetimePlugin->itemWidget(""));
        }
    }
    this->datetimePlugin->reloadSettings();
}

bool DDEIstateMenuPlugin::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Enter) {
        auto *subPlugin = dynamic_cast<QWidget*>(watched);
        if (subPlugin == this->cpuPlugin->itemWidget("")) {
            this->popupWidget = this->cpuPlugin->itemPopupApplet("");
        } else if (subPlugin == this->ramPlugin->itemWidget("")) {
            this->popupWidget = this->ramPlugin->itemPopupApplet("");
        } else if (subPlugin == this->sensorPlugin->itemWidget("")) {
            this->popupWidget = this->sensorPlugin->itemPopupApplet("");
        } else if (subPlugin == this->netspeedPlugin->itemWidget("")) {
            this->popupWidget = this->netspeedPlugin->itemPopupApplet("");
        } else if (subPlugin == this->datetimePlugin->itemWidget("")) {
            this->popupWidget = this->datetimePlugin->itemPopupApplet("");
        }
    }

    return QObject::eventFilter(watched, event);
}

QWidget *DDEIstateMenuPlugin::itemPopupApplet(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return this->popupWidget;
}