// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef STANDALONEIMAGE_H
#define STANDALONEIMAGE_H

#include <QStringList>

#include "treeitem.h"
#include "menu.h"
#include "StandalonePreset.h"


class StandaloneImage
{
public:
    StandaloneImage(QVariantMap scenes,QList<QString> setList);
    ~StandaloneImage();

    void scan(TreeItem *item);
    void modlineCheck(const char *modlineType,MODLINE &modline);
    void modlineItemCheck();
    void voiceItemCheck();
    void inputSettingsItemCheck();
    void keysItemCheck();
    void setDefaults();

    bool scanCompare(QString name);
    bool scanCompareKeys();
    unsigned char *imageRaw();
    int sceneCount();
    unsigned char *settingsRaw();
    int settingsLen();

    int itemMenu(Menu &menu);
    int itemInt();
    char itemChar();
    bool scanCompareChild(const char *itemName);
    double itemDouble();
    void setSettingsDefaults();
    FIXED_PT toFixedPt(double val);
    int getSlotIndex(TreeItem *ti,bool *ok);
    int calcSlotIndex(int slotIndex,QList<TreeItem*> &list);
    void scan_input_settings(TreeItem *scenes);
    void scan_slots(TreeItem *scenes,QList<QString> list);
    void scan_slot(int slotNum);

    IMAGE *image;
    SETTINGS settings;
    VOICE *voice;
    KEY *key;
    INPUT_SETTINGS *input;
    int slotIndex;
    MODLINE *modline;
    TreeItem *m_item;
    int m_sceneCount;

    bool destinations;
    bool voiceSettings;
    bool inputSettings;
    bool keys;
};

#define	LE_short(val) (((val & 0xFF) << 8) + ( ((val) >> 8) & 0xFF))
//#define	LE_int(val)  (LE_short(((int)val)>>16) | LE_short((int)val & 0xffff))
#define	LE_int(val)  ( LE_short((int) (val) & 0xffff)<<16 | LE_short((int)(val)>>16) )

#endif // STANDALONEIMAGE_H
