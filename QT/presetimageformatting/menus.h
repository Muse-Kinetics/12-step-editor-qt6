// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MENUS_H
#define MENUS_H

#include <QStringList>
#include "menu.h"

#define MENU_OFF 0
enum KEYSAFETY { SINGLEKEY=1,MULTIKEY};
enum NOTEMODE { NORMAL=1,LEGATO,TOGGLE,TOGCLR};
enum SOURCEMODE {SOURCE_OFF=1,PLUS_VELOCITY,MINUS_VELOCITY,PLUS_PRESSURE,MINUS_PRESSURE,PLUS_TILE,MINUS_TILE,PLUS_KEYNUM,MINUS_KEYNUM,PLUS_PEDAL,MINUS_PEDAL};
enum TABLE {TABLE_LINEAR=1,TABLE_SIN,TABLE_COS,TABLE_EXP,TABLE_LOG};


class Menus
{
public:
    Menus();

    static int bla;

    static Menu sourceMenu;
    static Menu tableMenu;
    static Menu noteModeMenu;
    static Menu destinationMenu;
    static Menu initModeMenu;
    static Menu displayMenu;
    static Menu keySafety;

};

#endif // MENUS_H
