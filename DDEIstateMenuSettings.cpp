//
// Created by septemberhx on 2020/6/27.
//

#include "DDEIstateMenuSettings.h"

DDEIstateMenuSettings* DDEIstateMenuSettings::instance = nullptr;

DDEIstateMenuSettings *DDEIstateMenuSettings::inst() {
    if (instance == nullptr) {
        instance = new DDEIstateMenuSettings();
    }
    return instance;
}

DDEIstateMenuSettings::DDEIstateMenuSettings()
    : enableCpu(true)
    , enableCpuLabel(true)
    , enableDatetime(true)
    , enableDatetimeLabel(true)
    , enableDisk(true)
    , enableDiskLabel(true)
    , enableNetwork(true)
    , enableNetworkLabel(true)
    , enableRam(true)
    , enableRamLabel(true)
    , enableSensors(true)
    , enableSensorsLabel(true)
{

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
