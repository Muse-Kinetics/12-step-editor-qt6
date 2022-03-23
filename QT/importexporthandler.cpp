// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "importexporthandler.h"

ImportExportHandler::ImportExportHandler(PresetInterface *pi, QWidget *parent) :
    QWidget(parent)
{
    presetInterface = pi;
}

void ImportExportHandler::slotImportPreset()
{
    QString filename = NULL;

    QSettings settings;

    const QString DEFAULT_DIR_KEY("default_dir");

    filename = QFileDialog::getOpenFileName(this, tr("Import Preset"), settings.value(DEFAULT_DIR_KEY).toString(), tr("12 Step Editor Preset Files (*.twelvesteppreset)"));


    //if file is selected
    if(!filename.isEmpty() && !filename.isNull())
    {
        //this gets the filename without the path
        QFileInfo fileInfo(filename);

        // store this folder location for the next time we open a file
        QString thisDirectory = fileInfo.absolutePath();
        settings.setValue(DEFAULT_DIR_KEY, thisDirectory);

        //open file
        QFile* presetFile = new QFile(filename);
        presetFile->open(QIODevice::ReadOnly);

        QByteArray presetByteArray = presetFile->readAll();
        presetFile->close();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(presetByteArray);
        QVariantMap importedPresetMap = jsonDoc.toVariant().toMap();

        //use the normalize slot to remove extra parameters from the preset map -- if there are any
        QVariantMap newImportedPresetMap = slotNormalizePresetMap(importedPresetMap);

        //add new parameters (if any) to the preset map and give them default values
        if(!presetInterface->defaultPresetMap.isEmpty())
        {
            QMapIterator<QString, QVariant> i(presetInterface->defaultPresetMap);

            //iterate through default map and compare with imported preset
            while(i.hasNext())
            {
                i.next();

                if(!newImportedPresetMap.contains(i.key()))
                {
                    //if imported preset map does not contain a value in the default map, insert it
                    newImportedPresetMap.insert(i.key(), i.value());
                }
            }
        }

        //check all the preset names and append a suffix if it's a duplicate
        QString saveAsName = presetInterface->slotAppendSuffixToNewPresetName(newImportedPresetMap.value(QString("preset_name")).toString());
        newImportedPresetMap.insert("preset_name", saveAsName);

        //set imported preset to new preset and update
        presetInterface->presetListCopy.clear();
        presetInterface->presetListMaster.clear();

        int numPresets = presetInterface->slotGetNumPresetsInJson();

        for(int i = 0; i < numPresets; i++)
        {
            presetInterface->presetListCopy.append(presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(i)).toMap());
            presetInterface->presetListMaster.append(presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(i)).toMap());
        }
        presetInterface->presetListCopy.append(newImportedPresetMap);
        presetInterface->presetListMaster.append(newImportedPresetMap);

        presetInterface->slotOrderPresetsInJson();
        presetInterface->slotWriteJSON(presetInterface->jsonMasterMap);
        emit signalAddOrRemovePreset();
        emit signalPresetMenu(numPresets);
    }
    else
    {
        qDebug("nothing selected");
    }
}

void ImportExportHandler::slotExportPreset()
{
    QVariantMap exportedPresetMap = presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(presetInterface->currentPresetNum)).toMap();

    QSettings settings;

    const QString DEFAULT_DIR_KEY("default_dir");

    //set path and filename (default filename is the preset name)
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Preset"), QString("%1/%2").arg(settings.value(DEFAULT_DIR_KEY).toString()).arg(exportedPresetMap.value("preset_name").toString()), tr("12 Step Editor Preset Files (*.twelvesteppreset)"));

    if(!filename.isEmpty() && !filename.isNull())
    {
        //this gets the filename without the path
        QFileInfo fileInfo(filename);

        // store this folder location for the next time we open a file
        QString thisDirectory = fileInfo.absolutePath();
        settings.setValue(DEFAULT_DIR_KEY, thisDirectory);

        //open new file to be saved
        QFile* presetFile = new QFile(filename);

        //remove extension from filename
        QString exportedPresetName = fileInfo.fileName().remove(".twelvesteppreset");

        qDebug() << QString("filename: %1").arg(exportedPresetName);

        //replace "preset_name" with filename typed in dialog
        exportedPresetMap.insert("preset_name", exportedPresetName);

        //open, write, and close
        presetFile->open(QIODevice::WriteOnly);

        QJsonDocument jsonDoc = QJsonDocument::fromVariant(exportedPresetMap);
        QByteArray presetByteArray = jsonDoc.toJson();

        presetFile->resize(0);
        presetFile->write(presetByteArray);
        presetFile->close();

        presetByteArray.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Import Old Preset Stuff //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImportExportHandler::slotImportOldPreset()
{
    presetName = "";

    QString filename = NULL;

    QSettings settings;

    const QString DEFAULT_DIR_KEY("default_dir");

    QFileDialog dialog(presetInterface, tr("Navigate to your 12Step.json file from 12 Step Editor V1.0"), settings.value(DEFAULT_DIR_KEY).toString(), tr("12 Step Editor version 1.0 Preset File (*.json)"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(dialog.exec())
    {
        filename = dialog.selectedFiles().at(0);
    }
    //If file is selected
    if(!filename.isEmpty() && !filename.isNull())
    {
        //this gets the filename without the path
        QFileInfo fileInfo(filename);

        // store this folder location for the next time we open a file
        QString thisDirectory = fileInfo.absolutePath();
        settings.setValue(DEFAULT_DIR_KEY, thisDirectory);

        QFile *presetFile = new QFile(filename);

        if(filename != NULL && !presetFile->exists())
        {
            emit signalPathNotFound();
        }
        else if(filename != NULL)
        {
            emit signalPathFound();

            presetFile->open(QIODevice::ReadOnly);

            QByteArray presetByteArray = presetFile->readAll();
            presetFile->close();

            QJsonDocument jsonDoc = QJsonDocument::fromJson(presetByteArray);
            QVariantMap importedMap = jsonDoc.toVariant().toMap();
            QMapIterator<QString, QVariant> i(importedMap);

            int numPresets = 1;

            while(i.hasNext())
            {
                i.next();

                QVariantMap patterstorage = i.value().toMap();
                QMapIterator<QString, QVariant> j(patterstorage);

                while(j.hasNext())
                {
                    j.next();

                    QVariantMap slot = j.value().toMap();
                    QMapIterator<QString, QVariant> k(slot);

                    while(k.hasNext())
                    {
                        k.next();

                        QVariantMap slotNum = k.value().toMap();
                        QMapIterator<QString, QVariant> l(slotNum);

                        while(l.hasNext())
                        {
                            l.next();

                            if(l.key() == "data")
                            {
                                importedOldPresetMap = l.value().toMap();

                                importedNewPresetMap = slotConvertPreset();

                                //get the preset name
                                presetName = slotNum.value("name").toList().at(0).toString();
                                QString changeablePresetName = presetName;
                                QList<QString> allPresetNames;
                                QString saveAsName;

                                //check all the preset names and append a suffix if it's a duplicate
                                int count = 0;
                                for(int i = 0; i < presetInterface->slotGetNumPresetsInJson(); i++)
                                {
                                    QString iteratedPresetName = presetInterface->jsonMasterMap.value(presetInterface->slotGetPresetStringFromInt(i)).toMap().value(QString("preset_name")).toString();
                                    allPresetNames.append(iteratedPresetName);

                                    //take out the presetName's copy suffix if it has one
                                    if(iteratedPresetName.contains(" V1.0 ") && iteratedPresetName.at(iteratedPresetName.length()-7) == QString(" ").at(0))
                                    {
                                        iteratedPresetName.chop(7);
                                    }
                                    else if(iteratedPresetName.contains(" V1.0 ") && iteratedPresetName.at(iteratedPresetName.length()-8) == QString(" ").at(0))
                                    {
                                        iteratedPresetName.chop(8);
                                    }
                                    else if(iteratedPresetName.contains(" V1.0 ") && iteratedPresetName.at(iteratedPresetName.length()-9) == QString(" ").at(0))
                                    {
                                        iteratedPresetName.chop(9);
                                    }

                                    //take out any suffixes that were already in the incoming presets
                                    if(changeablePresetName.contains(" V1.0 ") && changeablePresetName.at(changeablePresetName.length()-7) == QString(" ").at(0))
                                    {
                                        changeablePresetName.chop(7);
                                    }
                                    else if(changeablePresetName.contains(" V1.0 ") && changeablePresetName.at(changeablePresetName.length()-8) == QString(" ").at(0))
                                    {
                                        changeablePresetName.chop(8);
                                    }
                                    else if(changeablePresetName.contains(" V1.0 ") && changeablePresetName.at(changeablePresetName.length()-9) == QString(" ").at(0))
                                    {
                                        changeablePresetName.chop(9);
                                    }

                                    //increment the count to see how many presets in the preset list match with the typed name
                                    if(iteratedPresetName == changeablePresetName)
                                    {
                                        count++;
                                    }
                                }

                                //add a suffix to the preset name if the counter found a copy
                                if(count > 0)
                                {
                                    saveAsName = QString("%1 V1.0 %2").arg(changeablePresetName).arg(count);
                                    //importedNewPresetMap.insert("preset_name", QString("%1 V1.0 %2").arg(changeablePresetName).arg(count));
                                }
                                else
                                {
                                    saveAsName = presetName;
                                    //importedNewPresetMap.insert("preset_name", presetName);
                                }

                                //this will increment the copy number of the suffix if the save as name is already present
                                //acts as a compensator if inner copy numbers get deleted while leaving the latest copy
                                if(saveAsName.contains(QString("%1 V1.0 ").arg(changeablePresetName)) && allPresetNames.contains(saveAsName))
                                {
                                    QString tempNameHolder = saveAsName;
                                    int copyNumber = tempNameHolder.remove(QString("%1 V1.0 ").arg(changeablePresetName)).toInt();

                                    while (allPresetNames.contains(saveAsName))
                                    {
                                        copyNumber++;
                                        saveAsName = QString("%1 V1.0 %2").arg(changeablePresetName).arg(copyNumber);
                                    }
                                }

                                importedNewPresetMap.insert("preset_name", saveAsName);

                                //qDebug() << "Name of Importing Preset" << presetName;

                                //-------------- Set Imported Preset to new preset and update ---------------
                                presetInterface->presetListCopy.clear();
                                presetInterface->presetListMaster.clear();

                                numPresets = presetInterface->slotGetNumPresetsInJson();

                                for(int i = 0; i < numPresets; i++)
                                {
                                    presetInterface->presetListCopy.append(presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(i)).toMap());
                                    presetInterface->presetListMaster.append(presetInterface->jsonMasterMapCopy.value(presetInterface->slotGetPresetStringFromInt(i)).toMap());
                                }
                                presetInterface->presetListCopy.append(importedNewPresetMap);
                                presetInterface->presetListMaster.append(importedNewPresetMap);

                                presetInterface->slotOrderPresetsInJson();
                                presetInterface->slotWriteJSON(presetInterface->jsonMasterMap);
                                emit signalAddOrRemovePreset();
                            }
                        }
                    }
                }
            }
            emit signalPresetMenu(numPresets);
            emit signalImportingComplete();
        }
    }
}

QVariantMap ImportExportHandler::slotConvertPreset()
{
    QApplication::processEvents();
    if(presetName.length() > 0)
    {
        emit signalImportingPresetNum(QString("<center>Importing Preset '%1' Please Wait...</center>").arg(presetName));
    }
    else
    {
        emit signalImportingPresetNum(QString("<center>Importing Presets Please Wait...</center>"));
    }
    QApplication::processEvents();

    presetInterface->defaultPresetMap.clear();
    presetInterface->slotConstructDefaultMap();

    //set up needed variables for conversions
    QVariantMap correspondingSources;
    correspondingSources.insert("Off", "Off");
    correspondingSources.insert("+Velocity", "Velocity");
    correspondingSources.insert("-Velocity", "Velocity (Inverted)");
    correspondingSources.insert("+Pressure", "Pressure");
    correspondingSources.insert("-Pressure", "Pressure (Inverted)");
    correspondingSources.insert("+Tilt", "Tilt");
    correspondingSources.insert("-Tilt", "Tilt (Inverted)");
    correspondingSources.insert("+KeyNum", "KeyNum");
    correspondingSources.insert("-KeyNum", "KeyNum (Inverted)");
    correspondingSources.insert("+Pedal", "Pedal");
    correspondingSources.insert("-Pedal", "Pedal (Inverted)");

    QVariantMap correspondingTables;
    correspondingTables.insert("1 Lin", "Linear");
    correspondingTables.insert("2 Sin", "Sine");
    correspondingTables.insert("3 Cos", "Cosine");
    correspondingTables.insert("4 Exponential", "Exponential");
    correspondingTables.insert("5 Logarithmic", "Logarithmic");
    correspondingTables.insert("6 DeadZone", "DeadZone");

    //--------------------------------------------- Stuff for checking priority of which CC Related modlines to use!
    //priority: cc1, cc2, volume, pan, xfade

    QVariantMap destinationMapCC1 = importedOldPresetMap.value("Destinations::").toMap().value("CC1_Modline::").toMap();
    QVariantMap destinationMapCC2 = importedOldPresetMap.value("Destinations::").toMap().value("CC2_Modline::").toMap();
    QVariantMap destinationMapVol = importedOldPresetMap.value("Destinations::").toMap().value("Volume_Modline::").toMap();
    QVariantMap destinationMapPan = importedOldPresetMap.value("Destinations::").toMap().value("Pan_Modline::").toMap();
    QVariantMap destinationMapXFade = importedOldPresetMap.value("Destinations::").toMap().value("XFade_Modline::").toMap();
    QStringList destinationNamesToUse;

    if(destinationMapCC1.value("Source_A").toList().at(0).toString() != "Off" || destinationMapCC1.value("Source_B").toList().at(0).toString() != "Off")
    {
        destinationNamesToUse.append("CC1_Modline::");
    }
    if(destinationMapCC2.value("Source_A").toList().at(0).toString() != "Off" || destinationMapCC2.value("Source_B").toList().at(0).toString() != "Off")
    {
        destinationNamesToUse.append("CC2_Modline::");
    }
    if(destinationMapVol.value("Source_A").toList().at(0).toString() != "Off" || destinationMapVol.value("Source_B").toList().at(0).toString() != "Off")
    {
        destinationNamesToUse.append("Volume_Modline::");
    }
    if(destinationMapPan.value("Source_A").toList().at(0).toString() != "Off" || destinationMapPan.value("Source_B").toList().at(0).toString() != "Off")
    {
        destinationNamesToUse.append("Pan_Modline::");
    }
    if(destinationMapXFade.value("Source_A").toList().at(0).toString() != "Off" || destinationMapXFade.value("Source_B").toList().at(0).toString() != "Off")
    {
        destinationNamesToUse.append("XFade_Modline::");
    }
    if(destinationNamesToUse.size() < 2)    //just making sure there are enough strings in the list - an error occurs if nothing is in slot 0 & 1
    {
        destinationNamesToUse.append("NULL");
        destinationNamesToUse.append("NULL");
    }

    QStringList correspondingDestinations;
    correspondingDestinations.append("Velocity_Modline::");
    correspondingDestinations.append("Bend_Modline::");
    if(destinationNamesToUse.at(0).contains("Modline"))
    {
        correspondingDestinations.append(destinationNamesToUse.at(0));
    }
    if(destinationNamesToUse.at(1).contains("Modline"))
    {
        correspondingDestinations.append(destinationNamesToUse.at(1));
    }
    correspondingDestinations.append("AftTch_Modline::");
    correspondingDestinations.append("PlyAftTch_Modline::");

    if(correspondingDestinations.size() < 6)    //again making sure there are enough strings in the list - an error occurs if nothing is in slot 4 & 5
    {
        correspondingDestinations.append("NULL");
        correspondingDestinations.append("NULL");
    }

    QVariantMap newMap = presetInterface->defaultPresetMap;
    QMapIterator<QString, QVariant> newParams(presetInterface->defaultPresetMap);

    //---------------------------------------------- This while loop is where all the parameters are actually converted
    while(newParams.hasNext())
    {
        newParams.next();
        QString newParameterName = newParams.key();
        QString oldParameterName = "NULL";
        QVariant presetValue = QString("NULL");

        if(newParameterName.contains(QString("preset_displayname")))
        {
            QVariantMap displayChars = importedOldPresetMap.value("Alphanum_Display::").toMap();
            oldParameterName = QString("Display_Slot_%1").arg(newParameterName.at(19));
            presetValue = displayChars.value(oldParameterName).toList().at(0);
        }
        //parameters pertaining to the settings
        else if(newParameterName.contains("settings_key_safety_mode"))
        {
            oldParameterName = QString("Foot_Mode");
            QString oldPresetValue = importedOldPresetMap.value(oldParameterName).toList().at(0).toString();

            if(oldPresetValue == "Single Key")
            {
                presetValue = "SingleKey";
            }
            else
            {
                presetValue = "MultiKey";
            }
        }
        else if(newParameterName.contains("settings_note_mode"))
        {
            oldParameterName = QString("Note_Mode");
            presetValue = importedOldPresetMap.value(oldParameterName).toList().at(0);
        }
        //parameters pertaining to the keyboard tab
        else if(newParameterName.contains(QString("key")) && newParameterName.contains(QString("_note")))
        {
            //get an int of the Key Number
            QString keyNumberString = QString("%1%2").arg(newParameterName.at(3)).arg(newParameterName.at(4));
            int keyNum = keyNumberString.toInt();
            //get an int ofo the Note Slot
            QString noteSlotString = newParameterName.at(10);
            int noteSlot = noteSlotString.toInt();

            QVariantMap oldNoteSlots = importedOldPresetMap.value(QString("Key_%1::").arg(keyNum)).toMap();
            oldParameterName = QString("Note_%1").arg(noteSlot);
            presetValue = oldNoteSlots.value(oldParameterName).toList().at(0);
        }
        //parameters pertaining to the midi tab
        else if(newParameterName.contains("voice_"))
        {
            //which voice?
            QChar voice = newParameterName.at(6).toUpper();
            QVariantMap oldVoiceSlot = importedOldPresetMap.value(QString("Voice_Settings_%1::").arg(voice)).toMap();

            if(newParameterName.contains("_bank"))
            {
                oldParameterName = "Bank";
                presetValue = oldVoiceSlot.value(oldParameterName).toList().at(0);
            }
            else if(newParameterName.contains("_bend_range"))
            {
                oldParameterName = "Bend_Range";
                presetValue = oldVoiceSlot.value(oldParameterName).toList().at(0);
            }
            else if(newParameterName.contains("_channel"))
            {
                oldParameterName = "Channel";
                presetValue = oldVoiceSlot.value(oldParameterName).toList().at(0);
            }
            else if(newParameterName.contains("_programchange"))
            {
                oldParameterName = "Program_Change";
                QString oldParameterEnable = "Enable_Program_Change";

                if(oldVoiceSlot.value(oldParameterEnable).toList().at(0) == false)
                {
                    presetValue = -1;
                }
                else
                {
                    presetValue = oldVoiceSlot.value(oldParameterName).toList().at(0);
                }
            }
            else if(newParameterName.contains("_transpose"))
            {
                oldParameterName = "Transpose";
                presetValue = oldVoiceSlot.value(oldParameterName).toList().at(0);
            }
        }
        else if(newParameterName.contains("modline"))
        {
            QString modlineNumberString = newParameterName.at(7);
            int modlineNumber = modlineNumberString.toInt();

            //qDebug() << newParameterName << correspondingDestinations.at(modlineNumber-1);

            //check to see if this modline needs to be populated / its data made it to the list of prioritized destinations
            if(correspondingDestinations.at(modlineNumber-1).contains("Modline"))
            {
                QVariantMap destinationMap = importedOldPresetMap.value("Destinations::").toMap().value(correspondingDestinations.at(modlineNumber-1)).toMap();

                //check to see if the modline is used
                if(destinationMap.value("Source_A").toList().at(0).toString() != "Off" || destinationMap.value("Source_B").toList().at(0).toString() != "Off")
                {
                    if(newParameterName.contains("_destination"))
                    {
                        QString fullDestName = QString(correspondingDestinations.at(modlineNumber-1));
                        QString destName = fullDestName.remove("_Modline::");
                        if(destName == "AftTch")
                        {
                            presetValue = "Channel Pressure";
                        }
                        else if(destName.contains("CC") || destName.contains("Volume") || destName.contains("Pan") || destName.contains("XFade"))
                        {
                            presetValue = "CC";
                        }
                        else if(destName == "PlyAftTch")
                        {
                            presetValue = "Poly Aftertouch";
                        }
                        else if(destName == "Velocity")
                        {
                            presetValue = "Note";
                        }
                        else
                        {
                            presetValue = destName;
                        }
                    }
                    else if(newParameterName.contains("_cc_number"))
                    {
                        if(correspondingDestinations.at(modlineNumber-1) == "CC1_Modline::")
                        {
                            presetValue = importedOldPresetMap.value("Destinations::").toMap().value("CtlNum1").toList().at(0);
                        }
                        else if(correspondingDestinations.at(modlineNumber-1) == "CC2_Modline::")
                        {
                            presetValue = importedOldPresetMap.value("Destinations::").toMap().value("CtlNum2").toList().at(0);
                        }
                        else if(correspondingDestinations.at(modlineNumber-1) == "Volume_Modline::")
                        {
                            presetValue = 7;
                        }
                        else if(correspondingDestinations.at(modlineNumber-1) == "Pan_Modline::")
                        {
                            presetValue = 10;
                        }
                        else if(correspondingDestinations.at(modlineNumber-1) == "XFade_Modline::")
                        {
                            presetValue = 7;
                        }
                    }
                    else if(newParameterName.contains("_source_a"))
                    {
                        oldParameterName = "Source_A";
                        presetValue = correspondingSources.value(destinationMap.value(oldParameterName).toList().at(0).toString());
                    }
                    else if(newParameterName.contains("_source_b"))
                    {
                        oldParameterName = "Source_B";
                        presetValue = correspondingSources.value(destinationMap.value(oldParameterName).toList().at(0).toString());
                    }
                    else if(newParameterName.contains("_gain"))
                    {
                        oldParameterName = "Gain";
                        presetValue = destinationMap.value(oldParameterName).toList().at(0);
                    }
                    else if(newParameterName.contains("_offset"))
                    {
                        oldParameterName = "Offset";
                        presetValue = destinationMap.value(oldParameterName).toList().at(0);
                    }
                    else if(newParameterName.contains("_table"))
                    {
                        oldParameterName = "Table";
                        presetValue = correspondingTables.value(destinationMap.value(oldParameterName).toList().at(0).toString());
                    }
                    else if(newParameterName.contains("_min"))
                    {
                        oldParameterName = "Min";
                        presetValue = destinationMap.value(oldParameterName).toList().at(0);
                    }
                    else if(newParameterName.contains("_max"))
                    {
                        oldParameterName = "Max";
                        presetValue = destinationMap.value(oldParameterName).toList().at(0);
                    }
                }
                else //set the destination value to off if both sources are off
                {
                    if(newParameterName.contains("_destination"))
                    {
                        presetValue = "Off";
                    }
                }
            }
        }

        if(presetValue != "NULL")
        {
            newMap.insert(newParameterName, presetValue);
        }
    }
    presetInterface->defaultPresetMap.clear();
    return slotNormalizePresetMap(newMap);
}

QVariantMap ImportExportHandler::slotNormalizePresetMap(QVariantMap normalizeThis)
{
    presetInterface->defaultPresetMap.clear();
    presetInterface->slotConstructDefaultMap();

    QMapIterator<QString, QVariant> i(presetInterface->defaultPresetMap);

    while(i.hasNext())
    {
        i.next();

        //----------------------- Check for EXTRA parameters in the Imported Preset
        QMapIterator<QString, QVariant> j(normalizeThis);

        QStringList badKeys;

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
        //Iterate through the bad keys and remove from preset
        for(int i = 0; i<badKeys.count(); i++)
        {
            normalizeThis.remove(badKeys.at(i));
        }
    }
    return normalizeThis;
}
