//
// Created by septemberhx on 2020/5/19.
//

#include "DDEUtils.h"
#include "system_stat.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>

/**
 * Get current in/out data size in bytes of the network in bytes
 * @return Map[interface name, <in data size, out data size>]
 */
QMap<QString, QPair<ulong, ulong>> DDEUtils::currNetInOutBytes() {
    QMap<QString, QPair<ulong, ulong>> resultMap;

    NetIFStat ifstat {};
    NetIFStatMap ifstatMap {};
    if (SystemStat::readNetIfStats(ifstat, ifstatMap)) {
        for (QString interfaceName : ifstatMap.keys()) {
//            qDebug() << "==========>>> " << interfaceName << ifstatMap[interfaceName]->rx_bytes << ifstatMap[interfaceName]->tx_bytes;
            resultMap.insert(interfaceName, QPair<ulong, ulong>(ifstatMap[interfaceName]->rx_bytes, ifstatMap[interfaceName]->tx_bytes));
        }
    }

// ------- abandon. use the code from deepin-system-monitor
//
//    QFile file("/proc/net/dev");
//    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        file.readLine();  // abandon the header line
//        file.readLine();
//
//        while(!file.atEnd())
//        {
//            QTextStream dataStream(file.readLine());
//            QString devName, inDataSize, outDataSize, _;
//            dataStream >> devName >> inDataSize >> _ >> _ >> _ >> _ >> _ >> _ >> _ >> outDataSize;
//            devName = devName.remove(':');
//            resultMap.insert(devName, QPair<ulong, ulong>(inDataSize.toULong(), outDataSize.toULong()));
//            qDebug() << devName << inDataSize << outDataSize;
//        }
//        file.close();
//    }

    return resultMap;
}
