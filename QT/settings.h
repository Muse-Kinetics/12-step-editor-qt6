// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QDebug>
#include <QtGui>
#include <QVariant>
#include <QJsonDocument>

#include "definitions.h"

#ifdef Q_OS_MAC
#include "ui_settingsForm.h"
#else
#include "ui_settingsFormWin.h"
#endif

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0, QSettings *_sessionSettings = nullptr);

    QSettings *sessionSettings;
    QTimer *saveSettingsTimeout;
    int saveSettingsTimeoutTime;

    QWidget *settingsWidget;

    // midi thru dropdown
    QComboBox * midiThru;
    QComboBox * progChgRxCh;

    //------ Form
    Ui::settingsForm *settingsForm;

    // wrappers for midiThru - overwrought workaround for access violation on windows
    QString midiThru_currentText();
    void midiThru_setCurrentIndex(int index);
    void midiThru_setCurrentText(QString portName);
    void midiThru_addItem(QString portName);
    void midiThru_removeItem(int index);
    int midiThru_findItem(QString portName);


signals:
    void signalStoreValue(QString name, QVariant value);
    void signalWriteSettings();

    void signalSettingsDirty();

    // update aux
    void signalUpdateMIDIaux();
    void slotUpdateNRPNChannel(int);

public slots:
    void slotEnableUIfor12S2(bool is12s2);
    void slotConnectElements();
    void slotDisconnectElements();
    void slotUpdateLabeLValues();
    void slotValueChanged();
    void slotRecallPreset(QVariantMap);
    void slotResetGlobalSensitivity();
    void slotResetSelectSensitivity();

    void slotSaveSettingsTimeout();

//private slots:
    //void on_selectSensitivity_sliderReleased();

private:
    bool _is12s2;

};

#endif // SETTINGS_H
