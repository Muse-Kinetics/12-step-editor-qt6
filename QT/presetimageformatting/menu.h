// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MENU_H
#define MENU_H

#include <QStringList>
#include <QMap>

class Menu : public QStringList
{
public:
    Menu();

    Menu &operator<<(const QString &str);
    int mapIndexOf(QString item);
    QMap<QString,int> map;

//    QStringList &uiList();
private:

    QStringList m_uiList;
};

#endif // MENU_H
