// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "imageformatter.h"
#include "midi.h"

// uncomment this to enable writing the setlist to a c file (when sending it to 12 Step)
// You need to create a "12stepPresetsC" folder in your downloads folder for this to work

//#define GENERATE_FACTORY_PRESETS_C_FILE

ImageFormatter::ImageFormatter()
{
    // Prepare file handlers to output an 8051 compatible C file that contains factory presets

    // Get the Downloads folder path
    downloadsFolderPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    // Ensure there's a directory separator at the end (QDir::separator() ensures platform compatibility)
    if (!downloadsFolderPath.endsWith(QDir::separator()))
        downloadsFolderPath += QDir::separator();

    presetsFileName = downloadsFolderPath + "12stepPresetsC" + QDir::separator() + "factory_presets.c";

}

void ImageFormatter::formatImage(QVariantMap reducedSetlist)
{
    //-----------------------------------------------------------------------------------------------------------------//
    //This function assumes that the "reducedSetlist" variable has already accounted for "[EMPTY]" slots in the setlist//
    //-----------------------------------------------------------------------------------------------------------------//

    //---- Bool for variant conversion
    //bool ok = true;

    //---- Zero our image pointer
    image = 0;

    //Number pre presets in setlist (doesn't account for duplicates yet, not sure we need to)
    int numScenes = reducedSetlist.size();

    qDebug() << "\nformatImage called: numScenes:" << numScenes;

    //Allocate the number of images (presets) that are in our setlist
    image = (IMAGE *) malloc( numScenes * sizeof(IMAGE) );

    //Zero out the memory slot which will contain our presets (images)
    memset(image, 0, numScenes * sizeof(IMAGE));

#ifdef GENERATE_FACTORY_PRESETS_C_FILE
        // setup the factory presets C file
        preparePresetsCFile(numScenes);
#endif

    //Iterate through Setlist
    for(int slotIndex = 0; slotIndex < numScenes; slotIndex++)
    {
        qDebug() << "\nSetlist slotIndex: " << slotIndex;
        //Handle multiple CC destinations per preset, only two allowed
        bool CC1Used = false;

        QVariantMap currentPreset = reducedSetlist.value(QString("%1").arg(slotIndex)).toMap();

        //---- Set this images slot number (position in the setlist)
        image[slotIndex].slotNum = slotIndex;


        //----------------------------------------------------------------------------------------------------------//
        //--------------------------------------------- Voice Settings ---------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        //---- Voice Settings A
        image[slotIndex].voiceA.bankLSB = currentPreset.value("voice_a_bank").toInt();
        image[slotIndex].voiceA.bankMSB = currentPreset.value("voice_a_bank_msb").toInt();
        //image[slotIndex].voiceA.bendRange = toFixedPt(currentPreset.value("voice_a_bend_range").toDouble());
        image[slotIndex].voiceA.channel = currentPreset.value("voice_a_channel").toInt();
        image[slotIndex].voiceA.programChange = currentPreset.value("voice_a_programchange").toInt();

        //prior to 12Step2 we disabled/enabled program change using an off state in the number box. Now the firmware
        //recognizes -1 as disabling bank and program change messages
        //image[slotIndex].voiceA.programChangeEnable = currentPreset.value("voice_a_enable_programchange").toInt();
//        if(image[slotIndex].voiceA.programChange == -1)
//        {
//            image[slotIndex].voiceA.programChangeEnable = false;
//        }
//        else
//        {
//            image[slotIndex].voiceA.programChangeEnable = true;
//        }

        image[slotIndex].voiceA.transpose = currentPreset.value("voice_a_transpose").toInt();

        //---- Voice Settings B
        image[slotIndex].voiceB.bankLSB = currentPreset.value("voice_b_bank").toInt();
        image[slotIndex].voiceB.bankMSB = currentPreset.value("voice_b_bank_msb").toInt();
        //image[slotIndex].voiceB.bendRange = toFixedPt(currentPreset.value("voice_b_bend_range").toDouble());
        image[slotIndex].voiceB.channel = currentPreset.value("voice_b_channel").toInt();
        image[slotIndex].voiceB.programChange = currentPreset.value("voice_b_programchange").toInt();

        //image[slotIndex].voiceB.programChangeEnable = currentPreset.value("voice_b_enable_programchange").toInt();
//        if(image[slotIndex].voiceB.programChange == -1)
//        {
//            image[slotIndex].voiceB.programChangeEnable = false;
//        }
//        else
//        {
//            image[slotIndex].voiceB.programChangeEnable = true;
//        }
        image[slotIndex].voiceB.transpose = currentPreset.value("voice_b_transpose").toInt();


        //----------------------------------------------------------------------------------------------------------//
        //------------------------------------------------ Modlines ------------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        //Prepare modlines (set all to off)
        prepareModlines(slotIndex);

        //Iterate through modlines, sorting per each destination
        for(int modlineNum = 0; modlineNum < 6; modlineNum++)
        {
            //Get current modline's destination
            QString currentModlineDestination = currentPreset.value(QString("modline%1_destination").arg(modlineNum)).toString();

            //---- Note (Velocity)
            if(currentModlineDestination == "Note")
            {
                //qDebug() << "current preset" << currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString();

                //---- Velocity (Now known as Note)
                image[slotIndex].modlines.Velociy.Source_A = sourceMenuNumber( currentPreset.value(QString("modline%1_source_a").arg(modlineNum)).toString() );
                image[slotIndex].modlines.Velociy.Source_B = sourceMenuNumber( currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString() );
                image[slotIndex].modlines.Velociy.Gain = toFixedPt(currentPreset.value(QString("modline%1_gain").arg(modlineNum)).toDouble());
                image[slotIndex].modlines.Velociy.Offset = currentPreset.value(QString("modline%1_offset").arg(modlineNum)).toInt();
                image[slotIndex].modlines.Velociy.Table = tableMenuNumber( currentPreset.value(QString("modline%1_table").arg(modlineNum)).toString() );
                image[slotIndex].modlines.Velociy.Min = currentPreset.value(QString("modline%1_min").arg(modlineNum)).toInt();
                image[slotIndex].modlines.Velociy.Max = currentPreset.value(QString("modline%1_max").arg(modlineNum)).toInt();
                image[slotIndex].modlines.Velociy.Smooth = currentPreset.value(QString("modline%1_smooth").arg(modlineNum)).toInt();
            }

            //---- Bend
            else if(currentModlineDestination == "Bend")
            {
                image[slotIndex].modlines.bend.Source_A = sourceMenuNumber( currentPreset.value(QString("modline%1_source_a").arg(modlineNum)).toString() );
                image[slotIndex].modlines.bend.Source_B = sourceMenuNumber( currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString() );
                image[slotIndex].modlines.bend.Gain = toFixedPt(currentPreset.value(QString("modline%1_gain").arg(modlineNum)).toDouble());
                image[slotIndex].modlines.bend.Offset = currentPreset.value(QString("modline%1_offset").arg(modlineNum)).toInt();
                image[slotIndex].modlines.bend.Table = tableMenuNumber( currentPreset.value(QString("modline%1_table").arg(modlineNum)).toString() );
                image[slotIndex].modlines.bend.Min = currentPreset.value(QString("modline%1_min").arg(modlineNum)).toInt();
                image[slotIndex].modlines.bend.Max = currentPreset.value(QString("modline%1_max").arg(modlineNum)).toInt();
                image[slotIndex].modlines.bend.Smooth = currentPreset.value(QString("modline%1_smooth").arg(modlineNum)).toInt();
            }

            //---- Volume
            else if(currentModlineDestination == "Volume")
            {
                //Not currently used, turned off in initialization of modlines
            }

            //---- XFade
            else if(currentModlineDestination == "XFade")
            {
                //Not currently used, turned off in initialization of modlines
            }

            //---- Pan
            else if(currentModlineDestination == "Pan")
            {
                //Not currently used, turned off in initialization of modlines
            }

            //---- CC #1
            else if(currentModlineDestination == "CC" && !CC1Used)
            {
                image[slotIndex].modlines.CC1.Source_A = sourceMenuNumber( currentPreset.value(QString("modline%1_source_a").arg(modlineNum)).toString() );
                image[slotIndex].modlines.CC1.Source_B = sourceMenuNumber( currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString() );
                image[slotIndex].modlines.CC1.Gain = toFixedPt(currentPreset.value(QString("modline%1_gain").arg(modlineNum)).toDouble());
                image[slotIndex].modlines.CC1.Offset = currentPreset.value(QString("modline%1_offset").arg(modlineNum)).toInt();
                image[slotIndex].modlines.CC1.Table = tableMenuNumber( currentPreset.value(QString("modline%1_table").arg(modlineNum)).toString() );
                image[slotIndex].modlines.CC1.Min = currentPreset.value(QString("modline%1_min").arg(modlineNum)).toInt();
                image[slotIndex].modlines.CC1.Max = currentPreset.value(QString("modline%1_max").arg(modlineNum)).toInt();
                image[slotIndex].modlines.CC1.Smooth = currentPreset.value(QString("modline%1_smooth").arg(modlineNum)).toInt();
                image[slotIndex].CtlNum1 = currentPreset.value(QString("modline%1_cc_number").arg(modlineNum)).toInt();

                CC1Used = true;
            }

            //---- CC #2
            else if(currentModlineDestination == "CC" && CC1Used)
            {
                image[slotIndex].modlines.CC2.Source_A = sourceMenuNumber( currentPreset.value(QString("modline%1_source_a").arg(modlineNum)).toString() );
                image[slotIndex].modlines.CC2.Source_B = sourceMenuNumber( currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString() );
                image[slotIndex].modlines.CC2.Gain = toFixedPt(currentPreset.value(QString("modline%1_gain").arg(modlineNum)).toDouble());
                image[slotIndex].modlines.CC2.Offset = currentPreset.value(QString("modline%1_offset").arg(modlineNum)).toInt();
                image[slotIndex].modlines.CC2.Table = tableMenuNumber( currentPreset.value(QString("modline%1_table").arg(modlineNum)).toString() );
                image[slotIndex].modlines.CC2.Min = currentPreset.value(QString("modline%1_min").arg(modlineNum)).toInt();
                image[slotIndex].modlines.CC2.Max = currentPreset.value(QString("modline%1_max").arg(modlineNum)).toInt();
                image[slotIndex].modlines.CC2.Smooth = currentPreset.value(QString("modline%1_smooth").arg(modlineNum)).toInt();
                image[slotIndex].CtlNum2 = currentPreset.value(QString("modline%1_cc_number").arg(modlineNum)).toInt();


            }

            //---- Aftertouch (Channel Pressure)
            else if(currentModlineDestination == "Channel Pressure")
            {
                image[slotIndex].modlines.AfTch.Source_A = sourceMenuNumber( currentPreset.value(QString("modline%1_source_a").arg(modlineNum)).toString() );
                image[slotIndex].modlines.AfTch.Source_B = sourceMenuNumber( currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString() );
                image[slotIndex].modlines.AfTch.Gain = toFixedPt(currentPreset.value(QString("modline%1_gain").arg(modlineNum)).toDouble());
                image[slotIndex].modlines.AfTch.Offset = currentPreset.value(QString("modline%1_offset").arg(modlineNum)).toInt();
                image[slotIndex].modlines.AfTch.Table = tableMenuNumber( currentPreset.value(QString("modline%1_table").arg(modlineNum)).toString() );
                image[slotIndex].modlines.AfTch.Min = currentPreset.value(QString("modline%1_min").arg(modlineNum)).toInt();
                image[slotIndex].modlines.AfTch.Max = currentPreset.value(QString("modline%1_max").arg(modlineNum)).toInt();
                image[slotIndex].modlines.AfTch.Smooth = currentPreset.value(QString("modline%1_smooth").arg(modlineNum)).toInt();
            }

            //---- Poly Aftertouch
            else if(currentModlineDestination == "Poly Aftertouch")
            {
                image[slotIndex].modlines.PlyAftTch.Source_A = sourceMenuNumber( currentPreset.value(QString("modline%1_source_a").arg(modlineNum)).toString() );
                image[slotIndex].modlines.PlyAftTch.Source_B = sourceMenuNumber( currentPreset.value(QString("modline%1_source_b").arg(modlineNum)).toString() );
                image[slotIndex].modlines.PlyAftTch.Gain = toFixedPt(currentPreset.value(QString("modline%1_gain").arg(modlineNum)).toDouble());
                image[slotIndex].modlines.PlyAftTch.Offset = currentPreset.value(QString("modline%1_offset").arg(modlineNum)).toInt();
                image[slotIndex].modlines.PlyAftTch.Table = tableMenuNumber( currentPreset.value(QString("modline%1_table").arg(modlineNum)).toString() );
                image[slotIndex].modlines.PlyAftTch.Min = currentPreset.value(QString("modline%1_min").arg(modlineNum)).toInt();
                image[slotIndex].modlines.PlyAftTch.Max = currentPreset.value(QString("modline%1_max").arg(modlineNum)).toInt();
                image[slotIndex].modlines.PlyAftTch.Smooth = currentPreset.value(QString("modline%1_smooth").arg(modlineNum)).toInt();
            }
        }


        //----------------------------------------------------------------------------------------------------------//
        //------------------------------------------------ CV 1 & 2 ------------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        QMap<QString, unsigned char> cvLocalMap, cvUSBMap;

        cvLocalMap["Default (Gate)"] = 0;
        cvLocalMap["Default (Pitch)"] = 0;
        cvLocalMap["Gate"] = 1;
        cvLocalMap["Pitch"] = 2;
        cvLocalMap["Velocity"] = 3;
        cvLocalMap["Pressure"] = 4;
        cvLocalMap["Tilt"] = 5;
        cvLocalMap["Expression Pedal"] = 6;
        cvLocalMap["Disabled"] = 7;

        cvUSBMap["Gate"] = 0;
        cvUSBMap["Pitch"] = 1;
        cvUSBMap["Velocity"] = 2;
        cvUSBMap["Bend / Mod"] = 3;
        cvUSBMap["Ch 1"] = 0;
        cvUSBMap["Ch 2"] = 1;

        image[slotIndex].cv1ModeLocal = cvLocalMap.value(currentPreset.value("settings_cv1_local").toString(), 0); // default to 0 if not found
        image[slotIndex].cv1ModeUSB = cvUSBMap.value(currentPreset.value("settings_cv1_usb").toString(), 0); // default to 0 if not found
        image[slotIndex].cv1USBChannel = cvUSBMap.value(currentPreset.value("settings_cv1_usb_ch").toString(), 0); // default to 0 if not found

        image[slotIndex].cv2ModeLocal = cvLocalMap.value(currentPreset.value("settings_cv2_local").toString(), 0); // default to 0 if not found
        image[slotIndex].cv2ModeUSB = cvUSBMap.value(currentPreset.value("settings_cv2_usb").toString(), 0); // default to 0 if not found
        image[slotIndex].cv2USBChannel = cvUSBMap.value(currentPreset.value("settings_cv2_usb_ch").toString(), 0); // default to 0 if not found



        //----------------------------------------------------------------------------------------------------------//
        //------------------------------------------------ Note Mode -----------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        QMap<QString, int> noteModeMap;
        noteModeMap["Normal"] = 0;
        noteModeMap["Legato"] = 1;
        noteModeMap["Toggle"] = 2;
        noteModeMap["Hold"] = 3;

        QString noteMode = currentPreset.value("settings_note_mode").toString();
        image[slotIndex].noteMode = noteModeMap.value(noteMode, 0); // Default to 0 if not found



        //----------------------------------------------------------------------------------------------------------//
        //------------------------------------------------ Foot Mode -----------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        //This is known in the UI as "Key Safety"

        QMap<QString, int> keySafetyMap;
        keySafetyMap["SingleKey"] = 1;      // backwards compatibility needs no spaces when reading legacy json
        keySafetyMap["MultiKey"] = 0;
        keySafetyMap["Single Key"] = 1;     // qComboBox values had spaces during dev, leaving this here for extra safety
        keySafetyMap["Multi Key"] = 0;
        keySafetyMap["Mono"] = 1;     // qComboBox values had spaces during dev, leaving this here for extra safety
        keySafetyMap["Poly"] = 0;

        QString keySafety = currentPreset.value("settings_key_safety_mode").toString();

        image[slotIndex].footMode = keySafetyMap.value(keySafety, 0); // Default to 0 if not found
        qDebug() << "imageFormatter - footMode = " << image[slotIndex].footMode;


        //----------------------------------------------------------------------------------------------------------//
        //---------------------------------------------- Alphanumeric ----------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        //Note: these are of type "char"

        //qDebug() << "check" << currentPreset.value(QString("preset_displayname_1")).toString() << "check";

        QString thisVal = "None";
        QString thisKey = "";
        QString thisChar;

        for (int i = 0; i < 4; i++)
        {
            thisKey = QString("preset_displayname_%1").arg(i+1);
            thisVal = currentPreset.value(thisKey).toString();

            if (thisVal.length())
            {
                thisChar = image[slotIndex].display[i] = thisVal.toLatin1().at(0);
            }
            else
            {
                thisChar = image[slotIndex].display[i] = QString(" ").toLatin1().at(0);
            }

            qDebug() << QString("ImageFormatter, slotIndex[%1] - char[%2] length: %3 val: %4").arg(slotIndex).arg(i).arg(thisVal.length()).arg(thisChar);
        }

//        //Display Slot 1
//        if(!currentPreset.value(QString("preset_displayname_1")).toString().isNull())
//        {
//            image[slotIndex].display[0] = currentPreset.value(QString("preset_displayname_1")).toString().toLatin1().at(0);
//        }
//        else
//        {
//            image[slotIndex].display[0] = QString(" ").toLatin1().at(0);
//        }
//        //Display Slot 2
//        if(!currentPreset.value(QString("preset_displayname_2")).toString().isNull())
//        {
//            image[slotIndex].display[1] = currentPreset.value(QString("preset_displayname_2")).toString().toLatin1().at(0);
//        }
//        else
//        {
//            image[slotIndex].display[1] = QString(" ").toLatin1().at(0);
//        }
//        //Display Slot 3
//        if(!currentPreset.value(QString("preset_displayname_3")).toString().isNull())
//        {
//            image[slotIndex].display[2] = currentPreset.value(QString("preset_displayname_3")).toString().toLatin1().at(0);
//        }
//        else
//        {
//            image[slotIndex].display[2] = QString(" ").toLatin1().at(0);
//        }
//        //Display Slot 4
//        if(!currentPreset.value(QString("preset_displayname_4")).toString().isNull())
//        {
//            image[slotIndex].display[3] = currentPreset.value(QString("preset_displayname_4")).toString().toLatin1().at(0);
//        }
//        else
//        {
//            image[slotIndex].display[3] = QString(" ").toLatin1().at(0);
//        }

        //----------------------------------------------------------------------------------------------------------//
        //-------------------------------------------------- Keys --------------------------------------------------//
        //----------------------------------------------------------------------------------------------------------//

        //Iterate through keys to set their notes-- notes are ints (-1 is off?)
        for(int keyNum = 0; keyNum < NUM_KEYS; keyNum++)
        {

            //Format string according to JSON param name
            QString keyNumString;

            if( (keyNum + 1) < 10)
            {
                keyNumString = QString("key0%1_note").arg(keyNum + 1);
            }
            else
            {
                keyNumString = QString("key%1_note").arg(keyNum + 1);
            }


            //Add note# to string and get int value in current preset
            image[slotIndex].keys[keyNum].notes[0] = currentPreset.value(QString(keyNumString) + QString("1")).toInt();
            image[slotIndex].keys[keyNum].notes[1] = currentPreset.value(QString(keyNumString) + QString("2")).toInt();
            image[slotIndex].keys[keyNum].notes[2] = currentPreset.value(QString(keyNumString) + QString("3")).toInt();
            image[slotIndex].keys[keyNum].notes[3] = currentPreset.value(QString(keyNumString) + QString("4")).toInt();
            image[slotIndex].keys[keyNum].notes[4] = currentPreset.value(QString(keyNumString) + QString("5")).toInt();

            //qDebug() << QString(keyNumString) + QString("1") << currentPreset.value(QString(keyNumString) + QString("1")).toInt();
        }

        #ifdef GENERATE_FACTORY_PRESETS_C_FILE
            // save preset to C file
            savePresetToCFile(image[slotIndex], slotIndex, currentPreset.value("preset_name").toString());
        #endif
    } // end for loop

    #ifdef GENERATE_FACTORY_PRESETS_C_FILE
        closePresetsCFile();
    #endif

    //-------- Formate Preset/Setlist Image as SysEx and emit send signal
    deviceManager.slot_sendPresets(imageRaw(), numScenes);
}

void ImageFormatter::formatSettings(QVariantMap settingsMap)
{
    //---- Zero/clear settings memory block, set defaults
    memset(&settings,0,sizeof(settings));

    //---- Settings
    settings.connect_mode.standalone = 1;
    settings.connect_mode.tether = 0;
//    settings.pedal_filter.hysteresis = 5;
//    settings.pedal_filter.length = 3;
    settings.progchg_rx_channel = MIDI_CH_10;

    //---- Input Settings
    //Global Sensitivity
    settings.input_settings.GlobalSensitivity = toFixedPt(settingsMap.value("globalSensitivity").toDouble()); //JSON value set here, use conversion below
    qDebug() << "settings.input_settings.GlobalSensitivity: " << settings.input_settings.GlobalSensitivity.whole;

    //Select Sensitivity
    //settings.input_settings.SelectSensitivity = (int) (settingsMap.value("selectSensitivity").toDouble() * 100); //JSON value set here, use conversion below
    settings.input_settings.SelectSensitivity = (int) settingsMap.value("selectSensitivity").toInt();
    qDebug() << "settings.input_settings.SelectSensitivity: " << settings.input_settings.SelectSensitivity;

    //Backlight Brightness
    settings.keyL_brightness = (int) (settingsMap.value("backlightBrightness").toInt()); //JSON value set here, use conversion below
    qDebug() << "settings.keyL_brightness: " << settings.keyL_brightness;

    //Program Change RX channel
    settings.progchg_rx_channel = (int) (settingsMap.value("progchgRXchannel", MIDI_CH_10).toInt());
    qDebug() << "settings.progchg_rx_channel: " << settings.progchg_rx_channel;

    // expression pedal calibration
    //
    if (settingsMap.value("pedal_calibration_min").isNull() == false)
    {
        settings.pedal_calibration.heel = settingsMap.value("pedal_calibration_min").toInt();
    }
    else
    {
        settings.pedal_calibration.heel = 20; // default
    }

    if (settingsMap.value("pedal_calibration_max").isNull() == false)
    {
        settings.pedal_calibration.toe = settingsMap.value("pedal_calibration_max").toInt();
    }
    else
    {
        settings.pedal_calibration.toe = 230; // default
    }

    if (settingsMap.value("pedal_calibration_table").isNull() == false)
    {
        settings.pedal_calibration.table = settingsMap.value("pedal_calibration_table").toInt();
    }
    else
    {
        settings.pedal_calibration.table = 0; // default
    }


    //On Threshold
    settings.input_settings.onThreshold = (unsigned char) (settingsMap.value("onThreshold", 15).toInt());

    //Off Threshold
    settings.input_settings.offThreshold = (unsigned char) (settingsMap.value("offThreshold", 5).toInt());

    //connect mode??
    settings.connect_mode.midi_volume_reset = (unsigned char) (settingsMap.value("midiVolume").toInt());
    //qDebug() << "settings midi volume" << (unsigned char) (settingsMap.value("midiVolume").toInt());
    settings.connect_mode.key_velocity_disable = (unsigned char) (settingsMap.value("velocityOverride").toInt());
    qDebug() << "settings velocity override" << (unsigned char) (settingsMap.value("velocityOverride").toInt());

    //Format Settings into SysEx and emit send signal
    deviceManager.slot_sendSettings(settingsRaw());
}

void ImageFormatter::prepareModlines(int slotIndex)
{
    //---- Velocity
    image[slotIndex].modlines.Velociy.Source_A = 0;
    image[slotIndex].modlines.Velociy.Source_B = 0;
    image[slotIndex].modlines.Velociy.Gain = toFixedPt(0);
    image[slotIndex].modlines.Velociy.Offset = 0;
    image[slotIndex].modlines.Velociy.Table = 0;
    image[slotIndex].modlines.Velociy.Min = 0;
    image[slotIndex].modlines.Velociy.Max = 0;
    image[slotIndex].modlines.Velociy.Smooth = 0;

    //---- Bend
    image[slotIndex].modlines.bend.Source_A = 0;
    image[slotIndex].modlines.bend.Source_B = 0;
    image[slotIndex].modlines.bend.Gain = toFixedPt(0);
    image[slotIndex].modlines.bend.Offset = 0;
    image[slotIndex].modlines.bend.Table = 0;
    image[slotIndex].modlines.bend.Min = 0;
    image[slotIndex].modlines.bend.Max = 0;
    image[slotIndex].modlines.bend.Smooth = 0;

    //---- Volume
    image[slotIndex].modlines.Volume.Source_A = 0;
    image[slotIndex].modlines.Volume.Source_B = 0;
    image[slotIndex].modlines.Volume.Gain = toFixedPt(0);
    image[slotIndex].modlines.Volume.Offset = 0;
    image[slotIndex].modlines.Volume.Table = 0;
    image[slotIndex].modlines.Volume.Min = 0;
    image[slotIndex].modlines.Volume.Max = 0;
    image[slotIndex].modlines.Volume.Smooth = 0;

    //---- XFade
    image[slotIndex].modlines.XFade.Source_A = 0;
    image[slotIndex].modlines.XFade.Source_B = 0;
    image[slotIndex].modlines.XFade.Gain = toFixedPt(0);
    image[slotIndex].modlines.XFade.Offset = 0;
    image[slotIndex].modlines.XFade.Table = 0;
    image[slotIndex].modlines.XFade.Min = 0;
    image[slotIndex].modlines.XFade.Max = 0;
    image[slotIndex].modlines.XFade.Smooth = 0;

    //---- Pan
    image[slotIndex].modlines.Pan.Source_A = 0;
    image[slotIndex].modlines.Pan.Source_B = 0;
    image[slotIndex].modlines.Pan.Gain = toFixedPt(0);
    image[slotIndex].modlines.Pan.Offset = 0;
    image[slotIndex].modlines.Pan.Table = 0;
    image[slotIndex].modlines.Pan.Min = 0;
    image[slotIndex].modlines.Pan.Max = 0;
    image[slotIndex].modlines.Pan.Smooth = 0;

    //---- CC #1
    image[slotIndex].modlines.CC1.Source_A = 0;
    image[slotIndex].modlines.CC1.Source_B = 0;
    image[slotIndex].modlines.CC1.Gain = toFixedPt(0);
    image[slotIndex].modlines.CC1.Offset = 0;
    image[slotIndex].modlines.CC1.Table = 0;
    image[slotIndex].modlines.CC1.Min = 0;
    image[slotIndex].modlines.CC1.Max = 0;
    image[slotIndex].modlines.CC1.Smooth = 0;
    image[slotIndex].CtlNum1 = 0;

    //---- CC #2
    image[slotIndex].modlines.CC2.Source_A = 0;
    image[slotIndex].modlines.CC2.Source_B = 0;
    image[slotIndex].modlines.CC2.Gain = toFixedPt(0);
    image[slotIndex].modlines.CC2.Offset = 0;
    image[slotIndex].modlines.CC2.Table = 0;
    image[slotIndex].modlines.CC2.Min = 0;
    image[slotIndex].modlines.CC2.Max = 0;
    image[slotIndex].modlines.CC2.Smooth = 0;
    image[slotIndex].CtlNum2 = 0;

    //---- AfterTouch
    image[slotIndex].modlines.AfTch.Source_A = 0;
    image[slotIndex].modlines.AfTch.Source_B = 0;
    image[slotIndex].modlines.AfTch.Gain = toFixedPt(0);
    image[slotIndex].modlines.AfTch.Offset = 0;
    image[slotIndex].modlines.AfTch.Table = 0;
    image[slotIndex].modlines.AfTch.Min = 0;
    image[slotIndex].modlines.AfTch.Max = 0;
    image[slotIndex].modlines.AfTch.Smooth = 0;

    //---- Poly AfterTouch
    image[slotIndex].modlines.PlyAftTch.Source_A = 0;
    image[slotIndex].modlines.PlyAftTch.Source_B = 0;
    image[slotIndex].modlines.PlyAftTch.Gain = toFixedPt(0);
    image[slotIndex].modlines.PlyAftTch.Offset = 0;
    image[slotIndex].modlines.PlyAftTch.Table = 0;
    image[slotIndex].modlines.PlyAftTch.Min = 0;
    image[slotIndex].modlines.PlyAftTch.Max = 0;
    image[slotIndex].modlines.PlyAftTch.Smooth = 0;
}

//-------- Returns a pointer to our image (all scenes), used in 'downloading' w/ device manager
unsigned char *ImageFormatter::imageRaw()
{
    return (unsigned char *) image;
}

//-------- Returns a pointer to our settings, used in 'downloading' w/ device manager
unsigned char *ImageFormatter::settingsRaw()
{
    return (unsigned char *) &settings;
}

//-------- Converts a double to an int, and uchars via a union
FIXED_PT ImageFormatter::toFixedPt(double val)
{
    FIXED_PT fp;
    fp.whole = LE_int(val * 65536);
    return fp;
}

int ImageFormatter::sourceMenuNumber(QString menuItem)
{
    if(menuItem == "Off")
    {
        return 0;
    }
    else if(menuItem == "Velocity")
    {
        return 1;
    }
    else if(menuItem == "Velocity (Inverted)")
    {
        return 2;
    }
    else if(menuItem == "Pressure")
    {
        return 3;
    }
    else if(menuItem == "Pressure (Inverted)")
    {
        return 4;
    }
    else if(menuItem == "Tilt")
    {
        return 5;
    }
    else if(menuItem == "Tilt (Inverted)")
    {
        return 6;
    }
    else if(menuItem == "KeyNum")
    {
        return 7;
    }
    else if(menuItem == "KeyNum (Inverted)")
    {
        return 8;
    }
    else if(menuItem == "Pedal")
    {
        return 9;
    }
    else if(menuItem == "Pedal (Inverted)")
    {
        return 10;
    }

    return 0;
}

int ImageFormatter::tableMenuNumber(QString menuItem)
{
    if(menuItem == "Linear")
    {
        return 0;
    }
    else if(menuItem == "Sine")
    {
        return 1;
    }
    else if(menuItem == "Cosine")
    {
        return 2;
    }
    else if(menuItem == "Exponential")
    {
        return 3;
    }
    else if(menuItem == "Logarithmic")
    {
        return 4;
    }
    else if(menuItem == "DeadZone")
    {
        return 5;
    }

    return 0;
}



// Assuming the definition of IMAGE and related structs and unions are globally accessible

void ImageFormatter::preparePresetsCFile(unsigned int num_presets)
{
    QFile presetsFile(presetsFileName);

    presetsFile.remove(); // delete file

    // Open the file in Append mode to add to the end of the file
    if (!presetsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return; // Handle the error appropriately

    QTextStream out(&presetsFile);

    qDebug() << "imageFormatter application version: " << QString(APP_VERSION);

    out << "// scenes.c - generated by 12 Step Editor version " << QString(APP_VERSION) << "\n\n";
    out << "\n";
    out << "code const unsigned char standalone_info[] = \n";
    out << "{\n";
    out << "    PRESET_INFO_FORMAT_VERSION,\n";
    out << "    0,0,     // reserved\n";
    out << "    " << num_presets << "       // num_presets\n";
    out << "};\n\n";

    out << "PRESET_IMAGE (*preset_images_ptr)[" << num_presets << "] = (PRESET_IMAGE (*)[" << num_presets << "])scenes;\n\n";

    out << "code const unsigned char scenes[] = \n{\n";

    presetsFile.close();
}

void ImageFormatter::closePresetsCFile()
{
    QFile presetsFile(presetsFileName);

    // Open the file in Append mode to add to the end of the file
    if (!presetsFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return; // Handle the error appropriately

    QTextStream out(&presetsFile);

    out << "};\n";
    presetsFile.close();
}

#define SET_NOTE_MODE(mode) ((mode) & 0x03)
#define SET_CV1_MODE_LOCAL(mode) (((mode) & 0x07) << 2)
#define SET_CV1_MODE_USB(mode) (((mode) & 0x03) << 5)
#define SET_CV1_USB_CHANNEL(channel) (((channel) & 0x01) << 7)

#define SET_FOOT_MODE(mode) ((mode) & 0x03)
#define SET_CV2_MODE_LOCAL(mode) (((mode) & 0x07) << 2)
#define SET_CV2_MODE_USB(mode) (((mode) & 0x03) << 5)
#define SET_CV2_USB_CHANNEL(channel) (((channel) & 0x01) << 7)

void ImageFormatter::savePresetToCFile(IMAGE preset, int preset_number, QString preset_name)
{

    QFile presetsFile(presetsFileName);
    // Open the file in Append mode to add to the end of the file
    if (!presetsFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return; // Handle the error appropriately

    QTextStream out(&presetsFile);

    // Begin writing the IMAGE struct
    out << "//---- PRESET IMAGE " << preset_number << " [" << preset_name << "] ------------------------------------------\n";


    // Serialize the IMAGE struct here
    out << "    " << static_cast<unsigned char>(preset.slotNum) << ",    // slotNum\n";

    // ***************************************************************************************************************
    // Serialize MODLINES
    // ***************************************************************************************************************

    // Helper lambda to serialize FIXED_PT 'whole' value directly
//    auto serializeFixedPt = [&](const FIXED_PT& fixedPt) -> QString {
//        return QString::number(fixedPt.whole);
//    };

    // Helper lambda to serialize a single MODLINE
    auto serializeModline = [&](const MODLINE& modline, const QString& name)
    {
        out << "\n    // modline - " << name << "\n";
        out << "        " << static_cast<unsigned char>(modline.Gain.v.a) << ", ";
        out <<               static_cast<unsigned char>(modline.Gain.v.b) << ", ";
        out <<               static_cast<unsigned char>(modline.Gain.v.c) << ", ";
        out <<               static_cast<unsigned char>(modline.Gain.v.d) << ", // Gain - fixed point, four bytes\n";
        out << "        " << static_cast<unsigned char>(modline.Max) <<       ",          // Max\n";
        out << "        " << static_cast<unsigned char>(modline.Min) <<       ",          // Min\n";
        out << "        " << static_cast<unsigned char>(modline.Offset) <<    ",          // Offset\n";
        out << "        " << static_cast<unsigned char>(modline.Smooth) <<    ",          // Smooth\n";
        out << "        " << static_cast<unsigned char>(modline.Source_A) <<  ",          // Source_A\n";
        out << "        " << static_cast<unsigned char>(modline.Source_B) <<  ",          // Source_B\n";
        out << "        " << static_cast<unsigned char>(modline.Table) <<     ",          // Table\n";
    };

    // Serializing each MODLINE in MODLINES
    serializeModline(preset.modlines.AfTch, "AfTch");
    serializeModline(preset.modlines.bend, "bend");
    serializeModline(preset.modlines.CC1, "CC1");
    serializeModline(preset.modlines.CC2, "CC2");
    serializeModline(preset.modlines.Pan, "Pan");
    serializeModline(preset.modlines.PlyAftTch, "PlyAftTch");
    serializeModline(preset.modlines.Velociy, "Velociy");
    serializeModline(preset.modlines.Volume, "Volume");
    serializeModline(preset.modlines.XFade, "XFade");


    // ***************************************************************************************************************
    // Serialize 'display'
    out << "\n    // Display Characters\n";
    for (int i = 0; i < 4; ++i)
    {
        char displayChar = preset.display[i];
        out << "    " << static_cast<unsigned char>(displayChar); // Output the integer value
        if (displayChar >= 32 && displayChar <= 126) // Printable ASCII range
        {
            out << ",    // '" << displayChar << "'"; // Output the ASCII character equivalent as a comment
        }
        else
        {
            out << ",    // Non-printable character"; // Handle non-printable characters
        }
        if (i < 3)
        {
            out << "\n";
        }
    }
    out << "\n"; // Newline after the last character for formatting

    // Serialize CtlNum1, CtlNum2, and bitfields
    out << "\n    // Controllers\n";
    out << "    " << static_cast<unsigned char>(preset.CtlNum1) << ",    // CtlNum1\n";
    out << "    " << static_cast<unsigned char>(preset.CtlNum2) << ",    // CtlNum2\n\n";

    out << "    // Combined bitfield values for note/foot modes, cv1 and cv2\n\n";
    out << "    " << (  SET_NOTE_MODE(static_cast<unsigned char>(preset.noteMode)) |
                        SET_CV1_MODE_LOCAL(static_cast<unsigned char>(preset.cv1ModeLocal)) |
                        SET_CV1_MODE_USB(static_cast<unsigned char>(preset.cv1ModeUSB)) |
                        SET_CV1_USB_CHANNEL(static_cast<unsigned char>(preset.cv1USBChannel))     )
                  << ",                 // " << QString("noteMode(%1), cv1ModeLocal(%2), cv1ModeUSB(%3), cv1USBChannel(%4)\n").arg(preset.noteMode).arg(preset.cv1ModeLocal).arg(preset.cv1ModeUSB).arg(preset.cv1USBChannel);

    out << "    " << (  SET_FOOT_MODE(static_cast<unsigned char>(preset.footMode)) |
                        SET_CV2_MODE_LOCAL(static_cast<unsigned char>(preset.cv2ModeLocal)) |
                        SET_CV2_MODE_USB(static_cast<unsigned char>(preset.cv2ModeUSB)) |
                        SET_CV2_USB_CHANNEL(static_cast<unsigned char>(preset.cv2USBChannel))       )
                  << ",                // " << QString("footMode(%1), cv2ModeLocal(%2), cv2ModeUSB(%3), cv2USBChannel(%4)\n\n").arg(preset.footMode).arg(preset.cv2ModeLocal).arg(preset.cv2ModeUSB).arg(preset.cv2USBChannel);

    // Helper lambda to serialize a single VOICE struct
    auto serializeVoice = [&](const VOICE& voice, const QString& name)
    {
        out << "    // " << name << " VOICE\n";
        out << "        0,0,0,       // reserved (3)\n";
        out << "        " << static_cast<unsigned char>(voice.bankMSB) << ",        // bankMSB\n";
        out << "        " << static_cast<unsigned char>(voice.bankLSB) << ",        // bankLSB\n";
        out << "        " << static_cast<unsigned char>(voice.channel) << ",           // channel\n";
        out << "        0,           // reserved (1)\n";
        out << "        " << static_cast<unsigned char>(voice.programChange) << ",        // programChange\n";
        out << "        " << static_cast<unsigned char>(voice.transpose) << ",          // transpose\n\n";
    };

    // Serialize voiceA and voiceB
    serializeVoice(preset.voiceA, "voiceA");
    serializeVoice(preset.voiceB, "voiceB");

    // Serialize KEY structs
    out << "    // KEYS (notes)\n\n";
    for (int i = 0; i < NUM_KEYS; ++i)
    {
        out << "        ";
        for (int j = 0; j < NUM_NOTES_PER_KEY; ++j)
        {
            out << static_cast<unsigned char>(preset.keys[i].notes[j]);
            if (j < NUM_NOTES_PER_KEY - 1)
            {
                out << ", "; // Separate notes within the same key
            }
        }
        if (i < NUM_KEYS - 1)
        {
            out << ",        // Key " << i << "\n"; // Separate different keys
        }
        else
        {
            out << ",        // Key " << i << "\n\n\n"; // Last key, prepare to close array
        }
    }

    presetsFile.close();
}
