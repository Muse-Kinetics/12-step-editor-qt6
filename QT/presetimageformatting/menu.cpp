// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <QString>

#include "menu.h"

Menu::Menu()
{
}

Menu &Menu::operator<<(const QString &str)
{
    bool ok;
    QStringList split = str.split(" ");
    QString vstr = str;

    int val = split.at(0).toInt(&ok);

    if (ok)
    {
        split.removeFirst();
        vstr = split.join(" ");
        map.insert(vstr,val);
    }

    append(vstr);
    return *this;
}

int Menu::mapIndexOf(QString item)
{
    return map.value(item);
}

