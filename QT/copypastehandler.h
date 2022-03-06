// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef COPYPASTEHANDLER_H
#define COPYPASTEHANDLER_H

#include <QObject>
#include <QVariant>
//#include <QtGui>

#include "presetinterface.h"

class CopyPasteHandler : public QObject
{
    Q_OBJECT
public:
    explicit CopyPasteHandler(PresetInterface *presetInterfacer, QObject *parent = 0);

    PresetInterface *presetInterface;

    QVariantMap presetCopiedMap;

signals:
    void signalUpdatePasteAvailability();
    void signalAddOrRemovePreset();
    void signalPresetMenu(int numPresets);

public slots:
    void slotClearPreset();
    void slotCopyPreset();
    void slotPasteNewPreset();
    void slotPastePreset();
};

#endif // COPYPASTEHANDLER_H
