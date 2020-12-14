//
// Created by septemberhx on 2020/6/27.
//

#include "DDEIstateMenuSettings.h"
#include <QSettings>

DDEIstateMenuSettings* DDEIstateMenuSettings::instance = nullptr;

DDEIstateMenuSettings *DDEIstateMenuSettings::inst() {
    if (instance == nullptr) {
        instance = new DDEIstateMenuSettings();
    }
    return instance;
}

DDEIstateMenuSettings::DDEIstateMenuSettings() {
    QSettings settings("dde-istate-menus", "dde-istate-menus");
    this->enableCpu = settings.value("plugins/cpu", true).toBool();
    this->enableCpuLabel = settings.value("plugins/cpuLabel", true).toBool();
    this->enableDatetime = settings.value("plugins/datetime", true).toBool();
    this->enableDatetimeLabel = settings.value("plugins/datetimeLabel", true).toBool();
    this->enableDisk = settings.value("plugins/disk", true).toBool();
    this->enableDiskLabel = settings.value("plugins/diskLabel", true).toBool();
    this->enableNetwork = settings.value("plugins/network", true).toBool();
    this->enableNetworkLabel = settings.value("plugins/networkLabel", true).toBool();
    this->enableRam = settings.value("plugins/ram", true).toBool();
    this->enableRamLabel = settings.value("plugins/ramLabel", true).toBool();
    this->enableSensors = settings.value("plugins/sensors", true).toBool();
    this->enableSensorsLabel = settings.value("plugins/sensorsLabel", true).toBool();
}

bool DDEIstateMenuSettings::isEnableSensors() const {
    return enableSensors;
}

bool DDEIstateMenuSettings::isEnableSensorsLabel() const {
    return enableSensorsLabel;
}

bool DDEIstateMenuSettings::isEnableRam() const {
    return enableRam;
}

bool DDEIstateMenuSettings::isEnableRamLabel() const {
    return enableRamLabel;
}

bool DDEIstateMenuSettings::isEnableCpu() const {
    return enableCpu;
}

bool DDEIstateMenuSettings::isEnableCpuLabel() const {
    return enableCpuLabel;
}

bool DDEIstateMenuSettings::isEnableNetwork() const {
    return enableNetwork;
}

bool DDEIstateMenuSettings::isEnableNetworkLabel() const {
    return enableNetworkLabel;
}

bool DDEIstateMenuSettings::isEnableDatetime() const {
    return enableDatetime;
}

bool DDEIstateMenuSettings::isEnableDatetimeLabel() const {
    return enableDatetimeLabel;
}

bool DDEIstateMenuSettings::isEnableDisk() const {
    return enableDisk;
}

bool DDEIstateMenuSettings::isEnableDiskLabel() const {
    return enableDiskLabel;
}
