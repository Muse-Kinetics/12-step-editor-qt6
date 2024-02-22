// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "globalpresetinterface.h"
#include "midi.h"

GlobalPresetInterface::GlobalPresetInterface(QWidget *parent, QSettings *_sessionSettings) :
    QWidget(parent)
{
    //saveSettingsTimeout = new QTimer(this);
    //connect(saveSettingsTimeout, SIGNAL(timeout()), this, SLOT(slotSaveSettingsTimeout()));
    //saveSettingsTimeoutTime = 0;

    sessionSettings = _sessionSettings;
    slotSetJSONPath();
    slotReadSettings();

    bool flagWriteSettings = false;
    // double check that 12Step2 settings are loaded
    if (settings.value(QString("Global")).toMap().contains("backlightBrightness") == false)
    {
        qDebug() << "global settings: backlightBrightness entry missing, setting to default";
        slotStoreSettings("backlightBrightness", 29);

        flagWriteSettings = true;
    }

    if (settings.value(QString("Global")).toMap().contains("progchgRXchannel") == false)
    {
        qDebug() << "global settings: progchgRXchannel entry missing, setting to default";
        slotStoreSettings("progchgRXchannel", MIDI_CH_10);

        flagWriteSettings = true;
    }

    if (flagWriteSettings)
    {
        slotWriteSettings();
    }

    //slotWriteDefaultSettings();
    //slotRecallSettings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// SET UP JSON ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlobalPresetInterface::slotSetJSONPath()
{
    jsonPath = sessionSettings->value("PRESET_DIR").toString(); //get bundle path
    jsonPath.append("/settings.json");
    qDebug() << "globalpresetinerface slotSetJSONPath: " << jsonPath;
}

void GlobalPresetInterface::slotReadSettings()
{
    qDebug() << "globalpresetinerface slotReadSettings called";
    //load json into QFile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QByteArray settingsByteArray = jsonFile->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(settingsByteArray);
        settings = jsonDoc.toVariant().toMap();
    }
    else // this shouldn't ever happen now that mainwindow.cpp loads factory defaults if the files are missing
    {
        qDebug() << "Settings JSON: " << jsonPath;
        qFatal("WARNING: Settings JSON not found");
    }
    jsonFile->close();
}

void GlobalPresetInterface::slotWriteSettings()
{
    qDebug() << "globalpresetinerface slotWriteSettings called";
    //load json into QFile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //serialize JSON, write to file
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(settings);
        QByteArray ba = jsonDoc.toJson();

        jsonFile->resize(0);
        jsonFile->write(ba);
    }
    else
    {
        qDebug() << "Settings not found on write";
    }
    jsonFile->close();
}

void GlobalPresetInterface::slotWriteDefaultSettings()
{
    slotConstructSettingsDefaultMap();
    settings.insert(QString("Global"), defaultGlobalMap);

    slotWriteSettings();
}

void GlobalPresetInterface::slotConstructSettingsDefaultMap()
{
    defaultGlobalMap["globalSensitivity"] = 1.0;
    defaultGlobalMap["selectSensitivity"] = 10;
    defaultGlobalMap["backlightBrightness"] = 29;
    defaultGlobalMap["progchgRXchannel"] = MIDI_CH_10;
    defaultGlobalMap["velocityOverride"] = 0;
    defaultGlobalMap["midiVolume"] = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// SAVING & RECALLING //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlobalPresetInterface::slotStoreSettings(QString name, QVariant value)
{
    qDebug() << "globalpresetinterface slotStoreSettings called - name: " << name << " value: " << value;
    QVariantMap globalMap = settings.value(QString("Global")).toMap();
    globalMap.insert(name, value);
    settings.insert(QString("Global"), globalMap);
}

void GlobalPresetInterface::slotRecallSettings()
{
    qDebug() << "slotRecallSettings called";
    //called in constructor
    emit signalRecallSettings(settings.value(QString("Global")).toMap(), settings);
}

/*void GlobalPresetInterface::slotSaveSettingsTimeout()
{
    saveSettingsTimeoutTime++;

    //if 0.5s have elapsed since last value was changed
    if(saveSettingsTimeoutTime > 500)
    {
        //save settings
        slotWriteSettings();

        saveSettingsTimeout->stop();
    }
}*/
