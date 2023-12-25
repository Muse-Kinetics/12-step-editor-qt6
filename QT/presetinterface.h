// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef PRESETINTERFACE_H
#define PRESETINTERFACE_H

#include <QWidget>
#include <QDebug>
#include <QVariant>
#include <QtGui>
#include <QJsonDocument>
#include <QComboBox>

#include "imageformatter.h"

class PresetInterface : public QWidget
{
    Q_OBJECT
public:
    explicit PresetInterface(QWidget *parent = 0, QSettings *_sessionSettings = nullptr);

    QSettings *sessionSettings;
    QVariantMap jsonMasterMap;
    QVariantMap jsonMasterMapCopy;
    QList<QVariantMap> presetListMaster;
    QList<QVariantMap> presetListCopy;

    QVariantMap defaultPresetMap;

    QString jsonPath;
    QFile *jsonFile;

    int currentPresetNum;
    QVariantMap currentPresetMap;

    void closeEvent(QCloseEvent *);
    void writeDefaultJSON();
    QVariantMap getPresetMap(int presetNum);


signals:
    void signalRecallPreset(QVariantMap preset, QVariantMap jsonMasterMapCopy);
    void signalPresetMenu(int goToPresetNum);
    void signalPopulateSetlistMenus(QComboBox *presetMenu);
    void signalAddOrRemovePreset();
    void signalPresetDirty(bool);

public slots:
    void slotUpdateJSONPath();
    void slotReadJSON();
    void slotWriteJSON(QVariantMap jsonMap);

    void slotStoreValue(QString name, QVariant value, int presetNum);
    void slotRecallPreset(int i);
    void slotCheckSaveState();

    void slotSavePreset();
    void slotSavePresetAs(QString presetName);
    void slotDeletePreset();
    void slotPopulatePresetLists();
    void slotRevertPreset();

    void slotPopulatePresetMenu(QComboBox *presetMenu);
    void slotOrderPresetsInJson();
    int slotGetNumPresetsInJson();
    QString slotGetPresetStringFromInt(int);

    QString slotAppendSuffixToNewPresetName(QString);

    void slotConstructDefaultMap();

};

#endif // PRESETINTERFACE_H
