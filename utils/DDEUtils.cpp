//
// Created by septemberhx on 2020/5/19.
//

#include "DDEUtils.h"
#include "system_stat.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

QMap<QString, QString> DDEUtils::netIpv4Map() {
    QMap<QString, QString> resultMap;
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
//            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            resultMap.insert(QString(ifa->ifa_name), QString(addressBuffer));
        }
//        else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
//            // is a valid IP6 Address
//            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
//            char addressBuffer[INET6_ADDRSTRLEN];
//            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
//            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
//        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return resultMap;
}
