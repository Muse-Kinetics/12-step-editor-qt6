// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef IMAGEFORMATTER_H
#define IMAGEFORMATTER_H

#include <QDebug.h>
#include <QVariantMap>

#include "menu.h"
#include "menus.h"
#include "StandalonePreset.h"
#include "devicemanager.h"

#define	LE_short(val) (((val & 0xFF) << 8) + ( ((val) >> 8) & 0xFF))
#define	LE_int(val)  ( LE_short((int) (val) & 0xffff)<<16 | LE_short((int)(val)>>16) )

class ImageFormatter
{
public:
    ImageFormatter();

    /// The DeviceManager does not actually manage devices in our new context.
    /// It basically serves as an interface to the SysEx formatting functions,
    /// which space accross multiple files.

    DeviceManager deviceManager;

    //Get params from json
    void formatImage(QVariantMap reducedSetlist);
    void formatSettings(QVariantMap settingsMap);

    //Init modlines
    void prepareModlines(int slotIndex);

    //Pointers to our firmware images
    unsigned char *imageRaw();
    unsigned char *settingsRaw();

    //Convert double to FP
    FIXED_PT toFixedPt(double val);

    //Convert String to Int for Source Menus
    int sourceMenuNumber(QString menuItem);
    int tableMenuNumber(QString menuItem);

    //int itemMenu(Menu &menu, QString menuName);

    //---- Set of structs to format image
    IMAGE *image;
    SETTINGS settings;
    VOICE *voice;
    KEY *key;
    INPUT_SETTINGS *input;
    int sI;
    MODLINE *modline;
    int m_sceneCount;
};

#endif // IMAGEFORMATTER_H
