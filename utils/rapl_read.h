//
// Created by septemberhx on 2020/6/23.
//

#ifndef DDE_ISTATE_MENUS_RAPL_READ_H
#define DDE_ISTATE_MENUS_RAPL_READ_H

#include <QList>

struct PowerConsumption {
    double package;
    double cores;
    double onCoreGpu;
    double dram;
    double psys;
    qlonglong timestamp;
};

bool readPowerConsumption(QList<PowerConsumption>& pc);

Q_DECLARE_METATYPE(PowerConsumption)

#endif //DDE_ISTATE_MENUS_RAPL_READ_H
