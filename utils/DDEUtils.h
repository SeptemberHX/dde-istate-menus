//
// Created by septemberhx on 2020/5/19.
//

#ifndef DDE_ISTATE_MENUS_DDEUTILS_H
#define DDE_ISTATE_MENUS_DDEUTILS_H


#include <QPair>
#include <QString>
#include <QMap>

class DDEUtils {

public:
    static QMap<QString, QPair<ulong, ulong>> currNetInOutBytes();
    static QMap<QString, QString> netIpv4Map();
};


#endif //DDE_ISTATE_MENUS_DDEUTILS_H
