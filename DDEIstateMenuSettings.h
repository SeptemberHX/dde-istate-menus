//
// Created by septemberhx on 2020/6/27.
//

#ifndef DDE_ISTATE_MENUS_DDEISTATEMENUSETTINGS_H
#define DDE_ISTATE_MENUS_DDEISTATEMENUSETTINGS_H


class DDEIstateMenuSettings {

public:
    static DDEIstateMenuSettings* inst();

    bool isEnableSensors() const;

    bool isEnableSensorsLabel() const;

    bool isEnableRam() const;

    bool isEnableRamLabel() const;

    bool isEnableCpu() const;

    bool isEnableCpuLabel() const;

    bool isEnableNetwork() const;

    bool isEnableNetworkLabel() const;

    bool isEnableDatetime() const;

    bool isEnableDatetimeLabel() const;

    bool isEnableDisk() const;

    bool isEnableDiskLabel() const;

private:
    static DDEIstateMenuSettings *instance;

    bool enableSensors;
    bool enableSensorsLabel;

    bool enableRam;
    bool enableRamLabel;

    bool enableCpu;
    bool enableCpuLabel;

    bool enableNetwork;
    bool enableNetworkLabel;

    bool enableDatetime;
    bool enableDatetimeLabel;

    bool enableDisk;
    bool enableDiskLabel;

private:
    explicit DDEIstateMenuSettings();
};


#endif //DDE_ISTATE_MENUS_DDEISTATEMENUSETTINGS_H
