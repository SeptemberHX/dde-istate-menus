//
// Created by septemberhx on 2020/5/13.
//

#include "DDEIstateMenuPlugin.h"
#include "utils/network_traffic_filter.h"
#include <QJsonObject>
#include <QDebug>

#define PLUGIN_STATE_KEY "enable"

DDEIstateMenuPlugin::DDEIstateMenuPlugin(QObject *parent) : QObject(parent) {
    m_statsCollector = new StatsCollector();
    m_statsCollector->moveToThread(&m_workerThread);

    connect(this->m_statsCollector, &StatsCollector::cpuStatInfoUpdated, this, &DDEIstateMenuPlugin::updateCpuUsage);
    connect(this->m_statsCollector, &StatsCollector::processListUpdated, this, &DDEIstateMenuPlugin::updateProcessList);
    connect(this->m_statsCollector, &StatsCollector::uptimeInfoUpdated, this, [this](qulonglong uptime) {
        this->cpuPlugin->updateUptime(uptime);
    });
    connect(this->m_statsCollector, &StatsCollector::loadAvgInfoUpdated, this, [this](qreal loadAvg1, qreal loadAvg5, qreal loadAvg15) {
        this->cpuPlugin->updateLoadAvg(loadAvg1, loadAvg5, loadAvg15);
    });
    connect(this->m_statsCollector, &StatsCollector::memStatInfoUpdated,
            this,[this](qulonglong usedMemory, qulonglong totalMemory, qulonglong usedSwap, qulonglong totalSwap, mem_stat memStat) {
        this->ramPlugin->updateRamInfo(usedMemory * 100.0 / totalMemory, memStat);
    });
    connect(this->m_statsCollector, &StatsCollector::tempInfoUpdated, this, &DDEIstateMenuPlugin::updateTempInfo);

    connect(&m_workerThread, &QThread::started, m_statsCollector, &StatsCollector::start);
    connect(&m_workerThread, &QThread::finished, m_statsCollector, &QObject::deleteLater);
    m_workerThread.start();
}

const QString DDEIstateMenuPlugin::pluginName() const {
    return QStringLiteral("dde_istate_menus");
}

void DDEIstateMenuPlugin::init(PluginProxyInterface *proxyInter) {
    this->m_proxyInter = proxyInter;

    this->netspeedPlugin = new DDENetspeedPlugin();
    this->netspeedPlugin->init(this->m_proxyInter);
    this->datetimePlugin = new DatetimePlugin();
    this->datetimePlugin->init(this->m_proxyInter);
    this->cpuPlugin = new DDECpuPlugin();
    this->cpuPlugin->init(this->m_proxyInter);
    this->ramPlugin = new DDERamPlugin();
    this->ramPlugin->init(this->m_proxyInter);
    this->sensorPlugin = new DDESensorPlugin();
    this->sensorPlugin->init(this->m_proxyInter);

    if (!NetworkTrafficFilter::hasInstance) {
        NetworkTrafficFilter::hasInstance = true;
        std::thread nethogs_monitor_thread(&NetworkTrafficFilter::nethogsMonitorThreadProc);
        nethogs_monitor_thread.detach();
    }

    if (!pluginIsDisable()) {
//        this->m_proxyInter->itemAdded(this, this->pluginName());
        this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
        this->m_proxyInter->itemAdded(this->datetimePlugin, this->datetimePlugin->pluginName());
        this->m_proxyInter->itemAdded(this->cpuPlugin, this->cpuPlugin->pluginName());
        this->m_proxyInter->itemAdded(this->ramPlugin, this->ramPlugin->pluginName());
        this->m_proxyInter->itemAdded(this->sensorPlugin, this->sensorPlugin->pluginName());
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
    this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
    this->m_proxyInter->itemAdded(this->datetimePlugin, this->datetimePlugin->pluginName());
    this->m_proxyInter->itemAdded(this->cpuPlugin, this->cpuPlugin->pluginName());
    this->m_proxyInter->itemAdded(this->ramPlugin, this->ramPlugin->pluginName());
    this->m_proxyInter->itemAdded(this->sensorPlugin, this->sensorPlugin->pluginName());
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
    this->m_proxyInter->itemAdded(this->datetimePlugin, this->datetimePlugin->pluginName());
    this->m_proxyInter->itemAdded(this->netspeedPlugin, this->netspeedPlugin->pluginName());
    this->m_proxyInter->itemAdded(this->cpuPlugin, this->cpuPlugin->pluginName());
    this->m_proxyInter->itemAdded(this->ramPlugin, this->ramPlugin->pluginName());
    this->m_proxyInter->itemAdded(this->sensorPlugin, this->sensorPlugin->pluginName());
}

QWidget *DDEIstateMenuPlugin::itemWidget(const QString &itemKey) {
    Q_UNUSED(itemKey)
    return nullptr;
}

void DDEIstateMenuPlugin::updateProcessList(QList<ProcessEntry> procList) {
    QSet<pid_t> procIdSet;
//    for (auto iter = procList.begin(); iter != procList.end(); ++iter) {
//        if (procIdSet.contains(iter->getPID())) {
//            procList.erase(iter);
//        } else {
//            procIdSet.insert(iter->getPID());
//        }
//    }

    if (this->netspeedPlugin != nullptr) {
        this->netspeedPlugin->updateProcesses(procList);
        this->cpuPlugin->updateProcesses(procList);
        this->ramPlugin->updateProcesses(procList);
    }
}

void DDEIstateMenuPlugin::updateCpuUsage(qreal cpuPercent, const QList<double> cpuPercents, cpu_usage separatorUsage, QList<cpu_usage> cpuUsageList)
{
    this->cpuPlugin->addCpuUsage(cpuPercent, separatorUsage, cpuUsageList);
}

void DDEIstateMenuPlugin::updateTempInfo(QList<TempInfo> tempInfoList) {
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
