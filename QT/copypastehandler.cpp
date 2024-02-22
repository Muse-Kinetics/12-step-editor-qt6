// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "copypastehandler.h"

CopyPasteHandler::CopyPasteHandler(PresetInterface *presetInterfacer, QSettings* _sessionSettings, QObject *parent) :
    QObject(parent)
{
    presetInterface = presetInterfacer;
    sessionSettings = _sessionSettings;
}

void CopyPasteHandler::slotClearPreset()
{
    QString filename = sessionSettings->value("PRESET_DIR").toString(); //get bundle path
    filename.append("/Blank.twelvesteppreset");

    //open file
    QFile* presetFile = new QFile(filename);

    if(presetFile->exists())
    {
        presetFile->open(QIODevice::ReadOnly);

        qDebug("Blank JSON Found");

        QByteArray presetByteArray = presetFile->readAll();
        presetFile->close();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(presetByteArray);
        QVariantMap blankPresetMap = jsonDoc.toVariant().toMap();

        presetInterface->defaultPresetMap.clear();
        presetInterface->slotConstructDefaultMap();

        QMapIterator<QString, QVariant> i(presetInterface->defaultPresetMap);

        //iterate through the default map and compare with blank preset
        while(i.hasNext())
        {
            i.next();

            if(!blankPresetMap.contains(i.key()))
            {
                blankPresetMap.insert(i.key(), i.value());
                qDebug() << "insert missing params from default preset" << i.key() << i.value();
            }
        }
        //check for EXTRA parameters in the blank preset
        QMapIterator<QString, QVariant> j(blankPresetMap);
        QStringList badKeys;
        while(j.hasNext())
        {
            j.next();
            if(!presetInterface->defaultPresetMap.contains(j.key()))
            {
                badKeys.append(j.key());
            }
        }
        for(int i = 0; i < badKeys.count(); i++)
        {
            blankPresetMap.remove(badKeys.at(i));
            qDebug() << "remove extra params" << badKeys.at(i);
        }

        QString presetName = presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(presetInterface->currentPresetNum)).toMap().value("preset_name").toString();
        blankPresetMap.insert("preset_name", presetName);
        presetInterface->jsonMasterMapCopy.insert(presetInterface->slotGetPresetStringFromInt(presetInterface->currentPresetNum), blankPresetMap);
        presetInterface->slotRecallPreset(presetInterface->currentPresetNum);
        presetInterface->slotCheckSaveState();
    }
    else
    {
        qFatal("Blank JSON not found");
    }
}

void CopyPasteHandler::slotCopyPreset()
{
    presetCopiedMap = presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(presetInterface->currentPresetNum)).toMap();

    emit signalUpdatePasteAvailability();
}

void CopyPasteHandler::slotPastePreset()
{
    presetInterface->defaultPresetMap.clear();

    presetInterface->slotConstructDefaultMap();

    if(!presetInterface->defaultPresetMap.isEmpty())
    {
        QMapIterator<QString, QVariant> i(presetInterface->defaultPresetMap);

        //iterate through default map and compare with the presetCopiedMap
        while(i.hasNext())
        {
            i.next();
            if(!presetCopiedMap.contains(i.key()))
            {
                //if presetCopiedMap doesn't contain a value in the default map, insert it
                presetCopiedMap.insert(i.key(), i.value());
            }
        }

        //check for EXTRA parameters in the copied preset
        QMapIterator<QString, QVariant> j(presetCopiedMap);
        QStringList badKeys; // stores parameters we need to remove from the map

        while(j.hasNext())
        {
            j.next();

            //if the default map does not contain something in the new preset
            if(!presetInterface->defaultPresetMap.contains(j.key()))
            {
                //add to list of bad keys
                badKeys.append(j.key());
            }
        }
        //iterate through the bad keys and remove from preset
        for(int i = 0; i<badKeys.count(); i++)
        {
            presetCopiedMap.remove(badKeys.at(i));
        }

        presetInterface->jsonMasterMapCopy.insert(presetInterface->slotGetPresetStringFromInt(presetInterface->currentPresetNum), presetCopiedMap);
        presetInterface->slotRecallPreset(presetInterface->currentPresetNum);
        presetInterface->slotCheckSaveState();
    }
}

void CopyPasteHandler::slotPasteNewPreset()
{
    presetInterface->defaultPresetMap.clear();

    presetInterface->slotConstructDefaultMap();

    if(!presetInterface->defaultPresetMap.isEmpty())
    {
        QMapIterator<QString, QVariant> i(presetCopiedMap);

        //iterate through default map and compare with presetCopiedMap
        while(i.hasNext())
        {
            i.next();
            if(!presetCopiedMap.contains(i.key()))
            {
                //if presetCopiedMap doesn't contain a value in the default map, insert it
                presetCopiedMap.insert(i.key(), i.value());
            }
        }

        //check for EXTRA parameters in the copied preset
        QMapIterator<QString, QVariant> j(presetCopiedMap);
        QStringList badKeys; // stores parameters we need to remove from the map

        while(j.hasNext())
        {
            j.next();

            //if the default map does not contain something in the preset
            if(!presetInterface->defaultPresetMap.contains(j.key()))
            {
                //add to list of bad keys
                badKeys.append(j.key());
            }
        }

        //iteerate through the bad keys and remove from preset
        for(int i = 0; i<badKeys.count(); i++)
        {
            presetCopiedMap.remove(badKeys.at(i));
        }

        //check all the preset names and append a suffix if it's a duplicate
        QString saveAsName = presetInterface->slotAppendSuffixToNewPresetName(presetCopiedMap.value(QString("preset_name")).toString());
        presetCopiedMap.insert("preset_name", saveAsName);

        //set Imported Preset to New preset and Update
        presetInterface->presetListCopy.clear();
        presetInterface->presetListMaster.clear();

        int numPresets = presetInterface->slotGetNumPresetsInJson();

        for(int i = 0; i < numPresets; i++)
        {
            presetInterface->presetListCopy.append(presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(i)).toMap());
            presetInterface->presetListMaster.append(presetInterface->jsonMasterMap.value(presetInterface->slotGetPresetStringFromInt(i)).toMap());
        }
        presetInterface->presetListCopy.append(presetCopiedMap);
        presetInterface->presetListMaster.append(presetCopiedMap);

        presetInterface->slotOrderPresetsInJson();
        presetInterface->slotWriteJSON(presetInterface->jsonMasterMap);
        emit signalAddOrRemovePreset();
        emit signalPresetMenu(numPresets);
    }
}
