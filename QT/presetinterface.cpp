// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "presetinterface.h"
//#include <midiio/mac/mididevicemanager.h>

PresetInterface::PresetInterface(QWidget *parent) :
    QWidget(parent)
{
    //writeDefaultJSON();
    slotConstructDefaultMap();
}

void PresetInterface::closeEvent(QCloseEvent *)
{
    //qDebug() << "closing...";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////   JSON   ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PresetInterface::slotUpdateJSONPath()
{
    jsonPath = QCoreApplication::applicationDirPath(); //get bundle path

#if defined(Q_OS_MAC)
    jsonPath.remove(jsonPath.length() - 5, jsonPath.length()); //Remove "MacOS" from path string
    jsonPath.append("Resources/presets/12Step.json");
#else
    //qDebug() << "jp: " << jsonPath;
    jsonPath.append("/presets/12Step.json");
#endif
}

void PresetInterface::slotReadJSON()
{
    //Load json into QFile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug("12Step Editor JSON Found");

        QByteArray jsonByteArray = jsonFile->readAll(); //load json file into a QByteArray to be converted into a QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonByteArray);
        jsonMasterMap = jsonDoc.toVariant().toMap();
        jsonMasterMapCopy = jsonMasterMap;
    }
    else
    {
        qDebug() << "12Step JSON path: " << jsonPath;
        qFatal("ERROR: 12Step Editor JSON Not Found");
    }
    jsonFile->close();
}

void PresetInterface::slotWriteJSON(QVariantMap jsonMap)
{
    //Load json into QFile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //serialize json, write to file
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(jsonMap);
        QByteArray ba = jsonDoc.toJson();

        jsonFile->resize(0);
        jsonFile->write(ba);
    }
    else
    {
        qDebug() << "12Step Editor JSON Not Found";
    }
    jsonFile->close();
}

void PresetInterface::writeDefaultJSON()
{
    slotConstructDefaultMap();
    jsonPath = QString("./presets/12Step.json");

    for(int i = 0; i < 4; i++)
    {
        jsonMasterMap.insert(slotGetPresetStringFromInt(i),defaultPresetMap);
    }

    //load json into QFile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //serialize json, write to file
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(jsonMasterMap);
        QByteArray ba = jsonDoc.toJson();
        jsonFile->resize(0);
        jsonFile->write(ba);
    }
    else
    {
        qDebug() << QString("12Step Editor JSON Not Found: %1").arg(jsonPath);
    }

    jsonFile->close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////   Storage / Recall  ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PresetInterface::slotRecallPreset(int i)
{
    if(i == -1)
    {
        i = currentPresetNum;
    }
    else
    {
        currentPresetNum = i;
    }

    emit signalRecallPreset(jsonMasterMapCopy.value(slotGetPresetStringFromInt(currentPresetNum)).toMap(), jsonMasterMapCopy);

    slotCheckSaveState();
}

void PresetInterface::slotStoreValue(QString name, QVariant value, int presetNum)
{
    if(presetNum == -1)
    {
        presetNum = currentPresetNum;
    }

    QVariantMap presetMap = jsonMasterMapCopy.value(slotGetPresetStringFromInt(presetNum)).toMap();
    presetMap.insert(name, value);
    jsonMasterMapCopy.insert(slotGetPresetStringFromInt(presetNum), presetMap);
}

void PresetInterface::slotCheckSaveState()
{
    QStringList keyList = jsonMasterMapCopy.value(slotGetPresetStringFromInt(currentPresetNum)).toMap().keys();

    bool dirty = false;

    for(int i = 0; i < keyList.size(); i++)
    {
        if(jsonMasterMapCopy.value(slotGetPresetStringFromInt(currentPresetNum)).toMap().value(keyList.at(i)) !=
                jsonMasterMap.value(slotGetPresetStringFromInt(currentPresetNum)).toMap().value(keyList.at(i)))
        {
            dirty = true;
        }
    }
    emit signalPresetDirty(dirty);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////   Save, SaveAs, Revert, Delete  /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PresetInterface::slotSavePreset()
{
    //store copy of current preset into master json
    jsonMasterMap.insert(slotGetPresetStringFromInt(currentPresetNum), jsonMasterMapCopy.value(slotGetPresetStringFromInt(currentPresetNum)).toMap());

    //qDebug() << "update with this preset" << currentPresetNum;

    slotCheckSaveState();

    slotWriteJSON(jsonMasterMap);

    slotPopulatePresetLists();
}

void PresetInterface::slotSavePresetAs(QString presetName)
{
    //qDebug() << "Save As: " << presetName << slotGetNumPresetsInJson();

    //copy json maps into the preset lists
    slotPopulatePresetLists();

    //get preset params into map
    QVariantMap preset = presetListCopy.at(currentPresetNum);

    //Insert preset name param
    preset.insert("preset_name", presetName);

    //add to active preset lists
    presetListCopy.append(preset);
    presetListMaster.append(preset);

    //add and order json maps
    slotOrderPresetsInJson();

    slotRevertPreset();

    //save json file
    slotWriteJSON(jsonMasterMap);

    //repopulate preset menu -- calls slotPopulatePresetMenu()
    emit signalAddOrRemovePreset();

    int goToPresetNum = slotGetNumPresetsInJson();

    emit signalPresetMenu(goToPresetNum-1);
}

void PresetInterface::slotRevertPreset()
{
    if(currentPresetNum != -1)
    {
        //load preset from master map into copy
        jsonMasterMapCopy.insert(slotGetPresetStringFromInt(currentPresetNum), jsonMasterMap.value(slotGetPresetStringFromInt(currentPresetNum)).toMap());
        slotRecallPreset(currentPresetNum);
    }
}

void PresetInterface::slotDeletePreset()
{
    //remove preset from active preset lists
    slotPopulatePresetLists();
    presetListMaster.removeAt(currentPresetNum);
    presetListCopy.removeAt(currentPresetNum);

    if((presetListMaster.size() - 1) < currentPresetNum)
    {
        currentPresetNum = presetListMaster.size() - 1;
    }

    //re-iterate through active lists and set properly index in json
    slotOrderPresetsInJson();

    //save json file
    slotWriteJSON(jsonMasterMap);

    //repopulate preset menu -- calls slotPopulatePresetMenu()
    emit signalAddOrRemovePreset();
    emit signalPresetMenu(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////   Preset Menu  ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PresetInterface::slotPopulatePresetMenu(QComboBox* presetMenu)
{
    disconnect(presetMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRecallPreset(int)));

    //all presets should be stored and arranged in the json before calling this function!
    slotPopulatePresetLists();

    presetMenu->clear();

    int numPresets = slotGetNumPresetsInJson();

    //iterate through presets in numerical order, which is not guranteed by map iterator
    for(int i = 0; i < numPresets; i++)
    {
        QString presetName = jsonMasterMapCopy.value(slotGetPresetStringFromInt(i)).toMap().value("preset_name").toString();

        presetMenu->addItem(presetName, 0);
    }

    connect(presetMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRecallPreset(int)));

    emit signalPopulateSetlistMenus(presetMenu);
}

void PresetInterface::slotPopulatePresetLists()
{
    //all presets should be stored and arranged in the JSON before calling this function
    presetListCopy.clear();
    presetListMaster.clear();

    int numPreset = slotGetNumPresetsInJson();

    for(int i = 0; i < numPreset; i++)
    {
        presetListCopy.append(jsonMasterMapCopy.value(slotGetPresetStringFromInt(i)).toMap());
        presetListMaster.append(jsonMasterMap.value(slotGetPresetStringFromInt(i)).toMap());
    }
}

void PresetInterface::slotOrderPresetsInJson()
{
    //this function is used to ensure presets are kept ordered (without skipping numbers) in json

    //get number of presets in json
    int numPresets = slotGetNumPresetsInJson();

    //remove all presets from json (keep globals)
    for(int i = 0; i < numPresets; i++)
    {
        jsonMasterMapCopy.remove(slotGetPresetStringFromInt(i));
        jsonMasterMap.remove(slotGetPresetStringFromInt(i));
    }

    //re-insert presets in correct order with new indexes (just use size of copy here, the number should be the same in both
    for(int i = 0; i < presetListCopy.size(); i++)
    {
        jsonMasterMapCopy.insert(slotGetPresetStringFromInt(i), presetListCopy.at(i));
        jsonMasterMap.insert(slotGetPresetStringFromInt(i), presetListMaster.at(i));
    }
}

int PresetInterface::slotGetNumPresetsInJson()
{
    int numPresets = 0;

    //iterate through master map, gets num presets
    QMapIterator<QString, QVariant> map(jsonMasterMapCopy);

    while(map.hasNext())
    {
        map.next();

        //if a preset within master map...
        if(map.key().contains("Preset"))
        {
            //inc preset count
            numPresets++;
        }
    }
    return numPresets;
}

QVariantMap PresetInterface::getPresetMap(int presetNum)
{
    return jsonMasterMapCopy.value(slotGetPresetStringFromInt(presetNum)).toMap();
}

QString PresetInterface::slotGetPresetStringFromInt(int i)
{
    if(i < 10)
    {
        return QString("Preset_00%1").arg(i);
    }
    else if(i < 100)
    {
        return QString("Preset_0%1").arg(i);
    }
    else if(i < 1000)
    {
        return QString("Preset_%1").arg(i);
    }
    return QString();
}

QString PresetInterface::slotAppendSuffixToNewPresetName(QString currentName)
{
    QList<QString> allPresetNames;
    QString newName;
    int count = 0;

    //disallow '[DEFAULT]' to be typed into the name field by the user
    if(currentName == "[DEFAULT]")
    {
        currentName = "default";
    }

    //Iterates through all saved presets and checks to see if a preset with the same original name exists, then counts how many of them there are
    for(int i = 0; i < slotGetNumPresetsInJson(); i++)
    {
        QString iterationName = jsonMasterMap.value(slotGetPresetStringFromInt(i)).toMap().value(QString("preset_name")).toString();
        //int stringLength = iterationName.length();
        allPresetNames.append(iterationName);

        //take out the preset name's copy suffix if it already has one
        if(iterationName.contains(" copy") && iterationName.at(iterationName.length()-6) == QString(" ").at(0))
        {
            iterationName.chop(6);
        }
        else if(iterationName.contains(" copy") && iterationName.at(iterationName.length()-7) == QString(" ").at(0))
        {
            iterationName.chop(7);
        }
        else if(iterationName.contains(" copy") && iterationName.at(iterationName.length()-8) == QString(" ").at(0))
        {
            iterationName.chop(8);
        }

        //check to see if the user typed copy into the text field
        if(currentName.contains(" copy") && currentName.at(currentName.length()-6) == QString(" ").at(0) && i == 0)
        {
            currentName.chop(6);
        }
        else if(currentName.contains(" copy") && currentName.at(currentName.length()-7) == QString(" ").at(0) && i == 0)
        {
            currentName.chop(7);
        }
        else if(currentName.contains(" copy") && currentName.at(currentName.length()-8) == QString(" ").at(0) && i == 0)
        {
            currentName.chop(8);
        }

        //increment the count to see how many presets in the preset list match with the typed name
        if(iterationName == currentName)
        {
            count++;
        }
    }

    //add a suffix to the preset name if the counter found a copy
    if(count > 0)
    {
        newName = QString("%1 copy%2").arg(currentName).arg(count);
    }
    else
    {
        newName = currentName;
    }

    //this will increment the copy number of the suffix if the save as name is already present
    //acts as a compensator if a person were to have 2 preset copies and deleted the first one - it would have been given a 'copy2' suffix but there would already be one with that suffix
    if(newName.contains(QString("%1 copy").arg(currentName)) && allPresetNames.contains(newName))
    {
        QString tempNameHolder = newName;
        int copyNumber = tempNameHolder.remove(QString("%1 copy").arg(currentName)).toInt();

        while(allPresetNames.contains(newName))
        {
            copyNumber++;
            newName = QString("%1 copy%2").arg(currentName).arg(copyNumber);
        }
    }

    return newName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////   Default Maps  ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PresetInterface::slotConstructDefaultMap()
{
    //preset naming
    defaultPresetMap["preset_displayname_1"] = "D";
    defaultPresetMap["preset_displayname_2"] = "F";
    defaultPresetMap["preset_displayname_3"] = "L";
    defaultPresetMap["preset_displayname_4"] = "T";
    defaultPresetMap["preset_name"] = "DefaultPreset";

    //settings
    defaultPresetMap["settings_key_safety_mode"] = "SingleKey";
    defaultPresetMap["settings_note_mode"] = "Normal";

    //voice a settings
    defaultPresetMap["voice_a_bank"] = 0;
    defaultPresetMap["voice_a_bend_range"] = 2;
    defaultPresetMap["voice_a_channel"] = 1;
    //defaultPresetMap["voice_a_enable_programchange"] = false;
    defaultPresetMap["voice_a_programchange"] = 0;
    defaultPresetMap["voice_a_transpose"] = 0;

    //voice b settings
    defaultPresetMap["voice_b_bank"] = 0;
    defaultPresetMap["voice_b_bend_range"] = 2;
    defaultPresetMap["voice_b_channel"] = 2;
    //defaultPresetMap["voice_b_enable_programchange"] = false;
    defaultPresetMap["voice_b_programchange"] = 0;
    defaultPresetMap["voice_b_transpose"] = 0;

    //modlines
    defaultPresetMap["modline1_destination"] = "Note";
    defaultPresetMap["modline1_cc_number"] = -1;
    defaultPresetMap["modline1_gain"] = 1;
    defaultPresetMap["modline1_max"] = 127;
    defaultPresetMap["modline1_min"] = 0;
    defaultPresetMap["modline1_offset"] = 0;
    defaultPresetMap["modline1_smooth"] = 0;
    defaultPresetMap["modline1_source_a"] = "Velocity";
    defaultPresetMap["modline1_source_b"] = "Off";
    defaultPresetMap["modline1_table"] = "Linear";
    defaultPresetMap["modline2_destination"] = "Off";
    defaultPresetMap["modline2_cc_number"] = -1;
    defaultPresetMap["modline2_gain"] = 1;
    defaultPresetMap["modline2_max"] = 127;
    defaultPresetMap["modline2_min"] = 0;
    defaultPresetMap["modline2_offset"] = 0;
    defaultPresetMap["modline2_smooth"] = 0;
    defaultPresetMap["modline2_source_a"] = "Off";
    defaultPresetMap["modline2_source_b"] = "Off";
    defaultPresetMap["modline2_table"] = "Linear";
    defaultPresetMap["modline3_destination"] = "Off";
    defaultPresetMap["modline3_cc_number"] = 20;
    defaultPresetMap["modline3_gain"] = 1;
    defaultPresetMap["modline3_max"] = 127;
    defaultPresetMap["modline3_min"] = 0;
    defaultPresetMap["modline3_offset"] = 0;
    defaultPresetMap["modline3_smooth"] = 0;
    defaultPresetMap["modline3_source_a"] = "Off";
    defaultPresetMap["modline3_source_b"] = "Off";
    defaultPresetMap["modline3_table"] = "Linear";
    defaultPresetMap["modline4_destination"] = "Off";
    defaultPresetMap["modline4_cc_number"] = 21;
    defaultPresetMap["modline4_gain"] = 1;
    defaultPresetMap["modline4_max"] = 127;
    defaultPresetMap["modline4_min"] = 0;
    defaultPresetMap["modline4_offset"] = 0;
    defaultPresetMap["modline4_smooth"] = 0;
    defaultPresetMap["modline4_source_a"] = "Off";
    defaultPresetMap["modline4_source_b"] = "Off";
    defaultPresetMap["modline4_table"] = "Linear";
    defaultPresetMap["modline5_destination"] = "Off";
    defaultPresetMap["modline5_cc_number"] = 0;
    defaultPresetMap["modline5_gain"] = 1;
    defaultPresetMap["modline5_max"] = 127;
    defaultPresetMap["modline5_min"] = 0;
    defaultPresetMap["modline5_offset"] = 0;
    defaultPresetMap["modline5_smooth"] = 0;
    defaultPresetMap["modline5_source_a"] = "Off";
    defaultPresetMap["modline5_source_b"] = "Off";
    defaultPresetMap["modline5_table"] = "Linear";
    defaultPresetMap["modline6_destination"] = "Off";
    defaultPresetMap["modline6_cc_number"] = 0;
    defaultPresetMap["modline6_gain"] = 1;
    defaultPresetMap["modline6_max"] = 127;
    defaultPresetMap["modline6_min"] = 0;
    defaultPresetMap["modline6_offset"] = 0;
    defaultPresetMap["modline6_smooth"] = 0;
    defaultPresetMap["modline6_source_a"] = "Off";
    defaultPresetMap["modline6_source_b"] = "Off";
    defaultPresetMap["modline6_table"] = "Linear";

    //key notes (keys are numbered left to right)
    defaultPresetMap["key01_note1"] = 60;
    defaultPresetMap["key01_note2"] = -1;
    defaultPresetMap["key01_note3"] = -1;
    defaultPresetMap["key01_note4"] = -1;
    defaultPresetMap["key01_note5"] = -1;
    defaultPresetMap["key02_note1"] = 61;
    defaultPresetMap["key02_note2"] = -1;
    defaultPresetMap["key02_note3"] = -1;
    defaultPresetMap["key02_note4"] = -1;
    defaultPresetMap["key02_note5"] = -1;
    defaultPresetMap["key03_note1"] = 62;
    defaultPresetMap["key03_note2"] = -1;
    defaultPresetMap["key03_note3"] = -1;
    defaultPresetMap["key03_note4"] = -1;
    defaultPresetMap["key03_note5"] = -1;
    defaultPresetMap["key04_note1"] = 63;
    defaultPresetMap["key04_note2"] = -1;
    defaultPresetMap["key04_note3"] = -1;
    defaultPresetMap["key04_note4"] = -1;
    defaultPresetMap["key04_note5"] = -1;
    defaultPresetMap["key05_note1"] = 64;
    defaultPresetMap["key05_note2"] = -1;
    defaultPresetMap["key05_note3"] = -1;
    defaultPresetMap["key05_note4"] = -1;
    defaultPresetMap["key05_note5"] = -1;
    defaultPresetMap["key06_note1"] = 65;
    defaultPresetMap["key06_note2"] = -1;
    defaultPresetMap["key06_note3"] = -1;
    defaultPresetMap["key06_note4"] = -1;
    defaultPresetMap["key06_note5"] = -1;
    defaultPresetMap["key07_note1"] = 66;
    defaultPresetMap["key07_note2"] = -1;
    defaultPresetMap["key07_note3"] = -1;
    defaultPresetMap["key07_note4"] = -1;
    defaultPresetMap["key07_note5"] = -1;
    defaultPresetMap["key08_note1"] = 67;
    defaultPresetMap["key08_note2"] = -1;
    defaultPresetMap["key08_note3"] = -1;
    defaultPresetMap["key08_note4"] = -1;
    defaultPresetMap["key08_note5"] = -1;
    defaultPresetMap["key09_note1"] = 68;
    defaultPresetMap["key09_note2"] = -1;
    defaultPresetMap["key09_note3"] = -1;
    defaultPresetMap["key09_note4"] = -1;
    defaultPresetMap["key09_note5"] = -1;
    defaultPresetMap["key10_note1"] = 69;
    defaultPresetMap["key10_note2"] = -1;
    defaultPresetMap["key10_note3"] = -1;
    defaultPresetMap["key10_note4"] = -1;
    defaultPresetMap["key10_note5"] = -1;
    defaultPresetMap["key11_note1"] = 70;
    defaultPresetMap["key11_note2"] = -1;
    defaultPresetMap["key11_note3"] = -1;
    defaultPresetMap["key11_note4"] = -1;
    defaultPresetMap["key11_note5"] = -1;
    defaultPresetMap["key12_note1"] = 71;
    defaultPresetMap["key12_note2"] = -1;
    defaultPresetMap["key12_note3"] = -1;
    defaultPresetMap["key12_note4"] = -1;
    defaultPresetMap["key12_note5"] = -1;
    defaultPresetMap["key13_note1"] = 60;
    defaultPresetMap["key13_note2"] = -1;
    defaultPresetMap["key13_note3"] = -1;
    defaultPresetMap["key13_note4"] = -1;
    defaultPresetMap["key13_note5"] = -1;
}
