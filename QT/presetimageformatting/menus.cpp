// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "menus.h"
#include <QString>
#include <QStringList>

int Menus::bla;

Menu Menus::sourceMenu;
Menu Menus::tableMenu;
Menu Menus::noteModeMenu;
Menu Menus::destinationMenu;
Menu Menus::initModeMenu;
Menu Menus::displayMenu;
Menu Menus::keySafety;


Menus::Menus()
{
        sourceMenu
                << "1 Off" << "---"
                << "2 +Velocity" << "3 -Velocity" << "4 +Pressure" << "5 -Pressure" << "6 +Tilt" << "7 -Tilt" << "8 +KeyNum" << "9 -KeyNum" << "10 +Pedal" << "11 -Pedal";

        noteModeMenu
                << "1 Normal" << "2 Legato" << "3 Toggle" << "4 Hold";

        tableMenu << "1 1 Lin" << "2 2 Sin" << "3 3 Cos" << "4 4 Exponential" << "5 5 Logarithmic" << "6 6 DeadZone";

        destinationMenu << "1 None" << "---" << "2 Note Set" << "3 Note Live" << "4 CC" << "5 Bank"
                        << "6 Program" << "7 OSC" << "8 Pitch Bend" << "9 MMC" << "10 AfterTouch" << "11 Poly AfterTouch" << "12 GarageBand" << "13 HUI" << "---"
                        << "14 X Set" << "15 Y Set";

        initModeMenu << "1 None" << "2 Once" << "3 Always";

        displayMenu << "1  " << "2 -" << "3 _" << "4 0" << "5 1" << "6 2" << "7 3" << "8 4" << "9 5" << "10 6" << "11 7" << "12 8" << "13 9" <<"---"
        << "14 A" << "15 B" << "16 C" << "17 D" << "18 E" << "19 F" << "20 G" << "21 H" << "22 I" << "23 J" << "24 L"
        << "25 N" << "26 O" << "27 P" << "28 Q" << "29 R" << "30 S" << "31 T" << "32 U" << "33 Y" << "34 Z";

        keySafety  << "1 Multi Key" << "2 Single Key";

}
