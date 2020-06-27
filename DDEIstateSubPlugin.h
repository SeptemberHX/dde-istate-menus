//
// Created by septemberhx on 2020/5/19.
//

#ifndef DDE_ISTATE_MENUS_DDEISTATESUBPLUGIN_H
#define DDE_ISTATE_MENUS_DDEISTATESUBPLUGIN_H


class DDEIstateSubPlugin {

public:
    virtual void refreshData() = 0;
    virtual void setRefreshInterval(int msec) = 0;
    virtual void reloadSettings() = 0;
};


#endif //DDE_ISTATE_MENUS_DDEISTATESUBPLUGIN_H
