// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "menus.h"
#include "standaloneimage.h"
#include "midi.h"

StandaloneImage::StandaloneImage(QVariantMap scenes, QList<QString> setList)
{
    Q_UNUSED(scenes);
    Q_UNUSED(setList);
    //Initialize our image
    image = 0;

    //Initialize destinations
    destinations = false;

    //Initialize voice settings
    voiceSettings = false;

    //Zeros settings, sets modes
    setSettingsDefaults();

    //Scan the input settings, this is on the same level as the slots container
    //scan_input_settings(scenes);

    //Scans slots
    //scan_slots(scenes,setList);

}
StandaloneImage::~StandaloneImage()
{
    m_sceneCount = 0;
    if (image)
        free(image);

    //    qDebug("StandaloneImage::~StandaloneImage");
}

void StandaloneImage::setSettingsDefaults()
{
    memset(&settings,0,sizeof(settings));
    settings.connect_mode.standalone = 1;
    settings.connect_mode.tether = 0;
//    settings.pedal_filter.hysteresis = 5;
//    settings.pedal_filter.length = 3;
    settings.progchg_rx_channel = MIDI_CH_10;

}

void StandaloneImage::setDefaults()
{
}

void StandaloneImage::scan_input_settings(TreeItem *scenes)
{
    TreeItem *tiInput = scenes->treeItemByString("Input_Settings");
    if (tiInput)
        scan(tiInput);
}

void StandaloneImage::scan_slots(TreeItem *scenes,QList<QString> list)
{
    //Get our tree item of "slots", which represent our setlist
    TreeItem *tiSlots = scenes->treeItemByString("slots");

    //If slots are found
    if (tiSlots)
    {
        //How many items in our setlist?
        m_sceneCount = list.count();

        //---------------------------- Allocate an image the size of our setlist * preset size (IMAGE) --------------------------//
        image = (IMAGE *) malloc( m_sceneCount * sizeof(IMAGE) );

        //Zero our image memory block
        memset(image,0,m_sceneCount * sizeof(IMAGE));

        //Does absolutely nothing
        setDefaults();

        //How many slots?
        qDebug("scan found %d slots",m_sceneCount);

        //Iterate through slots in the setlist
        for (slotIndex = 0; slotIndex < m_sceneCount; slotIndex++)
        {
            bool ok;
            int i;

            //Get list of member parts
            QStringList nameParts = list.at(slotIndex).split(" ");

            //Iterate through child items
            for (i = 0;i < tiSlots->childItems.count();i++)
            {
                //Returns pointer to an individual slot in setlist
                TreeItem *tiSlot = tiSlots->childItems.at(i);

                //Double check that the name of this slot corresponds to the name specified in our setlist
                if (tiSlot->toString(ok) == nameParts[1]) //--- Corresponds to "name" object in top level of slot in .json
                {
                    //If name of slot was properly acquired and matches name
                    if (ok)
                    {
                        //Set our slot number at index to the "id", or setlist number specified in the json
                        image[slotIndex].slotNum = nameParts[0].toInt(&ok);

                        //Scan that individual slot's data
                        scan(tiSlot);
                        break;
                    }
                }
            }
        }
    }

}
#ifdef UNUSED
if (slotLevel)
{
    bool ok;
    int slotIndexName = getSlotIndex(item->childItems.at(i),&ok);
    if (ok){
        slotIndex = calcSlotIndex(slotIndexName,item->childItems);
        qDebug("slotIndexName[%d] -> slotIndex[%d]",slotIndexName,slotIndex);
        scan(item->childItems.at(i));
    }
} else
#endif

void StandaloneImage::scan_slot(int slotNum)
{
    Q_UNUSED(slotNum);
}

void StandaloneImage::scan(TreeItem *item)
{
    //--------------------------------------------------------------------------------//
    //This function recursively scans through a slotItem, setting the images properties
    //--------------------------------------------------------------------------------//

    //These "levels" are subcontainers within the .json data slot
    int i;

    //Not used, we start here
//    bool slotLevel = false;

    //Various levels within "data" in json
    bool destinationsLevel = false;
    bool voiceSettingsLevel = false;
    bool inputSettingsLevel = false;
    bool keysLevel = false;

    //Set item to our current member item, or our current item being examined, used below
    m_item = item;

    //    qDebug("%s",item->showData("").toLatin1().data());

    //Check what type of data we're looking at
    switch(item->itemData->typeId())
    {

    //------------ For all strings... which describe level
    case QMetaType::QString:

        //---- DESTINATIONS
        if (destinations)
        {
            //---- Assigne our modlines for our next recursion, modline will only get set to one per iteration
            modlineCheck("AftTch_Modline::",image[slotIndex].modlines.AfTch);
            modlineCheck("Bend_Modline::",image[slotIndex].modlines.bend);
            modlineCheck("CC1_Modline::",image[slotIndex].modlines.CC1);
            modlineCheck("CC2_Modline::",image[slotIndex].modlines.CC2);
            modlineCheck("Pan_Modline::",image[slotIndex].modlines.Pan);
            modlineCheck("PlyAftTch_Modline::",image[slotIndex].modlines.PlyAftTch);
            modlineCheck("Velocity_Modline::",image[slotIndex].modlines.Velociy);
            modlineCheck("Volume_Modline::",image[slotIndex].modlines.Volume);
            modlineCheck("XFade_Modline::",image[slotIndex].modlines.XFade);

            //Assigns actual parameters to modlines
            modlineItemCheck();

            if (scanCompareChild("CtlNum1"))
            {
                image[slotIndex].CtlNum1 = itemInt();
            }

            if (scanCompareChild("CtlNum2"))
            {
                image[slotIndex].CtlNum2 = itemInt();
            }

        }

        //---- VOICE SETTINGS
        if (voiceSettings)
        {
            voiceItemCheck();
        }

        //---- NOTE MODE (not a sub container like others)
        if (!voiceSettings && scanCompareChild("Note_Mode"))
        {
            image[slotIndex].noteMode = itemMenu(Menus::noteModeMenu);
        }

        //---- FOOT MODE (not a sub container like others)
        if (scanCompareChild("Foot_Mode"))
        {
            image[slotIndex].footMode = itemMenu(Menus::keySafety);
        }

        //---- INPUT SETTINGS... This is not within a slot, but resides on the level of the slots container
        if (inputSettings)
        {
            inputSettingsItemCheck();
        }

        //---- KEYS
        if (keys)
        {
            keysItemCheck();
        }


        //---- DESTINATIONS
        if (scanCompare("Destinations::"))
        {
            destinationsLevel = destinations = true;
        }

        //---- VOICE SETTINGS A
        if (scanCompare("Voice_Settings_A::"))
        {
            voiceSettingsLevel = voiceSettings = true;
            voice = &image[slotIndex].voiceA;
        }

        //---- VOICE SETTINGS B
        if (scanCompare("Voice_Settings_B::"))
        {
            voiceSettingsLevel = voiceSettings = true;
            voice = &image[slotIndex].voiceB;
        }

        //---- INPUT SETTINGS
        if (scanCompare("Input_Settings"))
        {
            inputSettingsLevel = inputSettings = true;
            input = &settings.input_settings;
        }

        //---- KEYS
        if (scanCompareKeys())
        {
            keysLevel = keys = true;
        }

        //---- DISPLAY SLOTS
        if (scanCompareChild("Display_Slot_1"))
        {
            image[slotIndex].display[0] = itemChar();
        }

        if (scanCompareChild("Display_Slot_2"))
        {
            image[slotIndex].display[1] = itemChar();
        }

        if (scanCompareChild("Display_Slot_3"))
        {
            image[slotIndex].display[2] = itemChar();
        }

        if (scanCompareChild("Display_Slot_4"))
        {
            image[slotIndex].display[3] = itemChar();
        }

        break;

    case QMetaType::LongLong:
        item->itemData->toLongLong();
        break;

    case QMetaType::ULongLong:
        item->itemData->toULongLong();
        break;

    case QMetaType::Double:
        item->itemData->toDouble();
        break;

    case QMetaType::Int:
        item->itemData->toInt();
        break;

    case QMetaType::QVariantList:break;

    default:
        qDebug("StandaloneImage::scan: unknown qvariant type[%d]",item->itemData->typeId());
        break;
    }

    //------- Here's where the recursion happens, we scan the child items
    for (i=0;i < item->childCount();i++)
    {
        //---- Scan our child image
        scan(item->childItems.at(i));
    }

    //---- RESET RECURSION LEVELS
    if (destinationsLevel)
        destinations = false;
    if (voiceSettingsLevel)
        voiceSettings = false;
    if (inputSettingsLevel)
        inputSettings = false;
    if (keysLevel)
        keys = false;
}
void StandaloneImage::modlineCheck(const char *modlineType,MODLINE &modlineArg)
{
    //---- This checks to make sure the modline we want is valid and found, and then sets our current modline to that modline in image
    //---- We have a member variable "modline" that we dynamically set to our current modline in the image, not sure why this is necessary
    //---- It appears to come in handy for just writing modline->Gain = [something] instead of image[slotIndex].modline.Gain = [something],
    //---- but those statements are essentially equivalent

    //    qDebug("modlineCheck: %s == %s",m_item->itemData->toString().toLatin1().data(),modlineType);
    QString name(modlineType);

    //If this is actually the modline we want, set our member variable "modline" to the image's corresponding modline space
    if (scanCompare(name))
        modline = &modlineArg;
}

bool StandaloneImage::scanCompare(QString itemName)
{
    //---- This function confirms the item we're looking at matches the item name we want ----//

    //    qDebug("scanCompare[%s]",itemName.toLatin1().data());
    return m_item->itemData->toString() == itemName;
    //    return !m_item->itemData->toString().compare(itemName);
}
bool StandaloneImage::scanCompareKeys()
{
    QString keyName;
    int keyNum;
    for (keyNum=0;keyNum < NUM_KEYS;keyNum++)
    {
        keyName.asprintf("Key_%d::",keyNum+1);

        //If we are indeed looking at a key container (our current m_item, above)
        if (scanCompare(keyName))
        {
            //Assign our member key to the current one in our image
            key = &image[slotIndex].keys[keyNum];
            return true;
        }
    }
    return false;
}

bool StandaloneImage::scanCompareChild(const char *itemName)
{
    if ( m_item->childItems.count() != 1 )
        return false;

    if (m_item->itemData->typeId() == QMetaType::QString)
    {

        //       qDebug("scanCompareChild:[%s]==[%s]",m_item->itemData->toString().toLatin1().data(),itemName);

        return !m_item->itemData->toString().compare(itemName);
    }
    return false;
}

int StandaloneImage::itemInt()
{
    TreeItem *child = m_item->childItems.at(0);

    switch(child->itemData->typeId())
    {
    case QMetaType::LongLong:
        //        qDebug("itemInt:LongLong[%d]",(int) child->itemData->toLongLong());
        return child->itemData->toLongLong();break;
    case QMetaType::ULongLong:
        //        qDebug("itemInt:ULongLong[%d]",(int) child->itemData->toULongLong());
        return child->itemData->toULongLong();break;
    case QMetaType::Int:
        //        qDebug("itemInt:Int[%d]",(int) child->itemData->toInt());
        return child->itemData->toInt();break;
    default:
        qDebug("StandaloneImage::itemInt: unknnown type %d",child->itemData->typeId());
        break;
    }


    return 0;
}
char StandaloneImage::itemChar()
{
    TreeItem *child = m_item->childItems.at(0);
    switch(child->itemData->typeId())
    {
    case QMetaType::QString:
        return child->itemData->toString().data()[0].toLatin1();
        break;
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Int:
    default:
        qDebug("StandaloneImage::itemInt: unknnown type %d",child->itemData->typeId());
        break;
    }
    return 0;


}

int StandaloneImage::sceneCount()
{
    return m_sceneCount;
}

unsigned char *StandaloneImage::imageRaw()
{
    return (unsigned char *) image;
}

unsigned char *StandaloneImage::settingsRaw()
{
    return (unsigned char *) &settings;
}

int StandaloneImage::settingsLen()
{
    return sizeof(settings);
}

double StandaloneImage::itemDouble()
{
    TreeItem *child = m_item->childItems.at(0);

    switch(child->itemData->typeId())
    {
    case QMetaType::Double:
        //       qDebug("double[%f]",child->itemData->toDouble());
        return child->itemData->toDouble();break;
    default:
        qDebug("StandaloneImage::itemDouble: unknnown type %d",child->itemData->typeId());
        break;
    }


    return 0;
}

int StandaloneImage::itemMenu(Menu &menu)
{
    TreeItem *child = m_item->childItems.at(0);

    switch(child->itemData->typeId())
    {
    case QMetaType::QString:
    {
        int index = menu.mapIndexOf(child->itemData->toString());
        if (!index)
            qDebug("StandaloneImage::itemMenu: can't find %s in menu",child->itemData->toString().toLatin1().data());
        else
        {
            //                   qDebug("Matched %s to %d",child->itemData->toString().toLatin1().data(),index);
            return index-1;
        }
    }
        break;
    default:
        qDebug("StandaloneImage::itemInt: unknnown type %d",m_item->itemData->typeId());
        break;
    }

    return 0;
}

FIXED_PT StandaloneImage::toFixedPt(double val)
{
    FIXED_PT fp;
    fp.whole = LE_int(val * 65536);
    return fp;
}

void StandaloneImage::modlineItemCheck()
{
    //If we're not examining the destiations object within this slot, exit
    if (!destinations)
        return;

    //Double check names, and set modline variables (set via member variable modline, which corresponds to image[slotIndex].modline... etc.
    if (scanCompareChild("Gain"))
        modline->Gain = toFixedPt(itemDouble());
    if (scanCompareChild("Max"))
        modline->Max = itemInt();
    if (scanCompareChild("Min"))
        modline->Min = itemInt();
    if (scanCompareChild("Offset"))
        modline->Offset = itemInt();
    if (scanCompareChild("Smooth"))
        modline->Smooth = itemInt();
    if (scanCompareChild("Source_A"))
        modline->Source_A = itemMenu(Menus::sourceMenu);
    if (scanCompareChild("Source_B"))
        modline->Source_B = itemMenu(Menus::sourceMenu);
    if (scanCompareChild("Table"))
        modline->Table = itemMenu(Menus::tableMenu);
}
void StandaloneImage::voiceItemCheck()
{
    if (!voiceSettings)
        return;

    if (scanCompareChild("Bank"))
        voice->bankLSB = itemInt();
    if (scanCompareChild("BankMSB"))
        voice->bankMSB = itemInt();
//    if (scanCompareChild("Bend_Range"))
//        voice->bendRange = toFixedPt( (double) itemInt() / 12.0);
    if (scanCompareChild("Channel"))
        voice->channel = itemInt();
    //    if (scanCompareChild("Note_Mode"))
    //        voice->noteMode = itemMenu(Menus::noteModeMenu);
    if (scanCompareChild("Program_Change"))
        voice->programChange = itemInt();
    if (scanCompareChild("Transpose"))
        voice->transpose = itemInt();
//    if (scanCompareChild("Enable_Program_Change"))
//        voice->programChangeEnable = itemInt();

}
void StandaloneImage::inputSettingsItemCheck()
{
    if (!inputSettings)
        return;

    if (scanCompareChild("Global_Sensitivity"))
        input->GlobalSensitivity = toFixedPt((double) itemInt() / 50);

    if (scanCompareChild("Select_Sensitivity")){

        //input->SelectSensitivity = (70 * (100 - itemInt()) ) / 100 + 30;
        input->SelectSensitivity = itemInt();
        qDebug("************** setting SelectSensigivity[%d]  *******************",input->SelectSensitivity);
    }

    if (scanCompareChild("Off_Threshold"))
        input->offThreshold = itemInt();
    if (scanCompareChild("On_Threshold"))
        input->onThreshold = itemInt();

}
void StandaloneImage::keysItemCheck()
{
    if (!keys)
        return;

    if (scanCompareChild("Note_1"))
    {
        //     qDebug("found Note_1");
        key->notes[0] = itemInt();
    }
    if (scanCompareChild("Note_2"))
        key->notes[1] = itemInt();
    if (scanCompareChild("Note_3"))
        key->notes[2] = itemInt();
    if (scanCompareChild("Note_4"))
        key->notes[3] = itemInt();
    if (scanCompareChild("Note_5"))
        key->notes[4] = itemInt();

}
int StandaloneImage::getSlotIndex(TreeItem *ti,bool *ok)
{
    int slotNum;
    slotNum = ti->itemData->toString().toInt(ok)-1;
    if (ok)
        return slotNum;
    else
        return 0;

}

int StandaloneImage::calcSlotIndex(int slotIndex,QList<TreeItem*> &list)
{
    int i,index = 0;
    bool ok;
    for (i=0;i<list.count();i++)
    {
        int newIndex = getSlotIndex(list.at(i),&ok);
        if (ok)
        {
            if (newIndex<slotIndex)
                index ++;
        }
    }

    return index;

}
