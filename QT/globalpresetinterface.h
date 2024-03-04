// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef GLOBALPRESETINTERFACE_H
#define GLOBALPRESETINTERFACE_H

#include <QWidget>
#include <QDebug>
#include <QVariant>
#include <QtGui>
#include <QJsonDocument>

#include "definitions.h"

class GlobalPresetInterface : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalPresetInterface(QWidget *parent = 0, QSettings *_sessionSettings = nullptr);

    //QTimer *saveSettingsTimeout;
    //int saveSettingsTimeoutTime;

    QSettings *sessionSettings;
    QVariantMap settings;
    QVariantMap defaultGlobalMap;

    QString jsonPath;
    QFile *jsonFile;

signals:
    void signalRecallSettings(QVariantMap preset); //, QVariantMap settings);

public slots:
    void slotSetJSONPath();
    void slotReadSettings();
    void slotWriteSettings();
    void slotWriteDefaultSettings();
    void slotStoreSettings(QString name, QVariant value);
    void slotConstructSettingsDefaultMap();

    void slotRecallSettings();
    //void slotSaveSettingsTimeout();
};

#endif // GLOBALPRESETINTERFACE_H
