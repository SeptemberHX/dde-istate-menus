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

#include "find_window_title.h"
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <QDebug>
#include <QObject>
#include <QtX11Extras/QX11Info>
#include <QIcon>
#include "utils.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

FindWindowTitle::FindWindowTitle() {
    rootWindow = QX11Info::appRootWindow();
}

FindWindowTitle::~FindWindowTitle() {}

QList<int> FindWindowTitle::getWindowPids()
{
    return m_windowTitles.keys();
}

QString FindWindowTitle::getWindowTitle(int pid)
{
    if (m_windowTitles.contains(pid)) {
        return getWindowName(m_windowTitles.value(pid));
    } else {
        return {};
    }
}

void FindWindowTitle::updateWindowInfos()
{
    QList<xcb_window_t> windows;

    xcb_get_property_reply_t *listReply =
        getProperty(rootWindow, "_NET_CLIENT_LIST_STACKING", XCB_ATOM_WINDOW);

    if (listReply) {
        xcb_window_t *windowList = static_cast<xcb_window_t *>(xcb_get_property_value(listReply));
        uint windowListLength = listReply->length;

        for (uint i = 0; i < windowListLength; i++) {
            xcb_window_t window = windowList[i];

            foreach (QString type, getWindowTypes(window)) {
                if (type.contains("_NET_WM_WINDOW_TYPE_NORMAL") ||
                    type.contains("_NET_WM_WINDOW_TYPE_DIALOG")) {
                    windows.append(window);
                }
            }
        }

        free(listReply);

        m_windowTitles.clear();
        foreach (auto window, windows) {
            int pid = Utils::getWindowPid(this, window);

            if (!m_windowTitles.contains(pid)) {
                m_windowTitles[pid] = window;
            }
        }
    }
}

xcb_window_t FindWindowTitle::getWindow(int pid)
{
    if (m_windowTitles.contains(pid)) {
        return m_windowTitles.value(pid);
    } else {
        return UINT_MAX;
    }
}

QString FindWindowTitle::getWindowName(xcb_window_t window)
{
    if (window == rootWindow) {
        return tr("Desktop");
    } else {
        xcb_get_property_reply_t *reply = getProperty(window, "_NET_WM_NAME", getAtom("UTF8_STRING"));

        if(reply) {
            QString result = QString::fromUtf8(static_cast<char*>(xcb_get_property_value(reply)), xcb_get_property_value_length(reply));

            free(reply);

            return result;
        } else {
            return QString();
        }
    }
}

xcb_get_property_reply_t* FindWindowTitle::getProperty(xcb_window_t window, QString propertyName, xcb_atom_t type)
{
    xcb_get_property_cookie_t cookie = xcb_get_property(QX11Info::connection(), 0, window, getAtom(propertyName), type, 0, UINT32_MAX);
    return xcb_get_property_reply(QX11Info::connection(), cookie, NULL);
}

xcb_atom_t FindWindowTitle::getAtom(QString name)
{
    QByteArray rawName = name.toLatin1();
    xcb_atom_t result = XCB_ATOM_NONE;
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(QX11Info::connection(), 0, rawName.size(), rawName.data());
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(QX11Info::connection(), cookie, NULL);
    if(reply) {
        result = reply->atom;

        free(reply);
    }

    return result;
}

QStringList FindWindowTitle::getWindowTypes(xcb_window_t window)
{
    QStringList types;
    xcb_get_property_reply_t *reply = getProperty(window, "_NET_WM_WINDOW_TYPE", XCB_ATOM_ATOM);

    if(reply) {
        xcb_atom_t *typesA = static_cast<xcb_atom_t*>(xcb_get_property_value(reply));
        int typeNum = reply->length;

        for(int i = 0; i < typeNum; i++) {
            types.append(getAtomName(typesA[i]));
        }

        free(reply);
    }

    return types;
}

QString FindWindowTitle::getAtomName(xcb_atom_t atom)
{
    QString result;

    xcb_get_atom_name_cookie_t cookie = xcb_get_atom_name(QX11Info::connection(), atom);
    xcb_get_atom_name_reply_t *reply = xcb_get_atom_name_reply(QX11Info::connection(), cookie, NULL);

    if (reply) {
        result = QString::fromLatin1(xcb_get_atom_name_name(reply), xcb_get_atom_name_name_length(reply));
        free(reply);
    }

    return result;
}

QString FindWindowTitle::getWindowFlatpakAppid(xcb_window_t window)
{
    xcb_get_property_reply_t *reply = getProperty(window, "FLATPAK_APPID", getAtom("STRING"));

    if(reply) {
        QList<QByteArray> rawClasses = QByteArray(static_cast<char*>(xcb_get_property_value(reply)),
                                                  xcb_get_property_value_length(reply)).split('\0');

        free(reply);

        return QString::fromLatin1(rawClasses[0]);
    } else {
        return QString();
    }
}

int FindWindowTitle::getWindowPid(xcb_window_t window)
{
    xcb_get_property_reply_t *reply = getProperty(window, "_NET_WM_PID", XCB_ATOM_CARDINAL);
    int pid = 0;

    if (reply) {
        pid = *((int *) xcb_get_property_value(reply));

        free(reply);
    }

    return pid;
}

QPixmap FindWindowTitle::getWindowIcon(xcb_window_t win, int iconSize)
{
    QIcon defaultExecutableIcon = QIcon::fromTheme("application-x-executable");
    QPixmap defaultPixmap = defaultExecutableIcon.pixmap(iconSize, iconSize);

    if (win > 0) {
        int format;
        ulong type, nitems, extra;
        ulong* data = 0;

        XGetWindowProperty(QX11Info::display(), win, getAtom("_NET_WM_ICON"),
                           0, LONG_MAX, False, AnyPropertyType,
                           &type, &format, &nitems, &extra,
                           (uchar**)&data);
        if (!data) {
            return defaultPixmap;
        }

        QImage img (data[0], data[1], QImage::Format_ARGB32);
        for (int i=0; i<img.byteCount()/4; ++i) {
            ((uint*)img.bits())[i] = data[i+2];
        }

        // Need add options Qt::KeepAspectRatio and Qt::SmoothTransformation to keep window icon scaled smoothly.
        QPixmap pixmap = QPixmap::fromImage(img).scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        XFree(data);

        return pixmap;
    }

    return defaultPixmap;
}