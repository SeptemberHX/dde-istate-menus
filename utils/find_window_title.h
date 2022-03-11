/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *               2011 ~ 2018 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FINDWINDOWTITLE_H
#define FINDWINDOWTITLE_H

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <QMap>
#include <QObject>

class FindWindowTitle : QObject
{
    Q_OBJECT

public:
    FindWindowTitle();
    ~FindWindowTitle();
    QString getWindowName(xcb_window_t window);
    QString getWindowFlatpakAppid(xcb_window_t window);
    int getWindowPid(xcb_window_t window);
    QPixmap getWindowIcon(xcb_window_t win, int iconSize);

    QList<int> getWindowPids();
    QString getWindowTitle(int pid);
    void updateWindowInfos();
    xcb_window_t getWindow(int pid);

private:
    xcb_get_property_reply_t* getProperty(xcb_window_t window, QString propertyName, xcb_atom_t type);
    xcb_atom_t getAtom(QString name);
    QStringList getWindowTypes(xcb_window_t window);
    QString getAtomName(xcb_atom_t atom);

    xcb_window_t rootWindow;
    QMap<int, xcb_window_t> m_windowTitles;
};

#endif
