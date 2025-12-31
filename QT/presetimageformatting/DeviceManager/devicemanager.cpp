// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "devicemanager.h"
#include "device.h"
#include "sysexcmds.h"
#include "download.h"
#include "sysexdata.h"

DeviceManager::DeviceManager(QObject *parent) :
    QObject(parent)
{
    //fwQueryTimer = new QTimer(this);
    //connect(fwQueryTimer, SIGNAL(timeout()), this, SLOT(slotFwQueryTimeout()));
}

void DeviceManager::updateStandalone(StandaloneImage &standaloneImage)
{
//    int i;

//    qDebug("sizeof(FIXED_PT)[%d]",sizeof(FIXED_PT));
//    qDebug("sizeof(MODLINE)[%d]",sizeof(MODLINE));
//    qDebug("sizeof(MODLINES)[%d]",sizeof(MODLINES));
//    qDebug("sizeof(VOICE)[%d]x2[%d]",sizeof(VOICE),sizeof(VOICE)*2);
//    qDebug("sizeof(KEY)[%d] x%d[%d]",sizeof(KEY),NUM_KEYS,sizeof(standaloneImage.image->keys));
//    qDebug("sizeof(INPUT_SETTINGS)[%d]",sizeof(INPUT_SETTINGS));
//    qDebug("sizeof(IMAGE)[%d]",sizeof(IMAGE));

//    for (i=0;i<16;i++)
//        qDebug("image[%d] = %x",i,standaloneImage.imageRaw()[i]);

//    if (!firmware_compatable(0))
//    {
//            //qDebug("not compatable with build num %d",build_num);
//            return;
//    }

 //   download_start(0,standaloneImage.imageRaw(),standaloneImage.sceneCount(),standaloneImage.settingsRaw(),standaloneImage.settingsLen());

    //Formats our image, sends to c file, but doesn't actually send to device
    send_standalone_image(standaloneImage.imageRaw(),standaloneImage.sceneCount());

    //Sends the actual image
    emit sigSysex(midi_sysex_data(),midi_sysex_len());

    //Format settings
    send_standalone_settings(standaloneImage.settingsRaw(),standaloneImage.settingsLen());

    //Sends the settings image
    emit sigSysex(midi_sysex_data(),midi_sysex_len());

}

//const char *DeviceManager::sigPadCorner(int padNum)
//{
//    switch(padNum)
//    {
//        case 1:
//            return SIGNAL(sigPadCorner1(int,int));
//        case 2:
//            return SIGNAL(sigPadCorner2(int,int));
//        case 3:
//            return SIGNAL(sigPadCorner3(int,int));
//        case 4:
//            return SIGNAL(sigPadCorner4(int,int));
//        case 5:
//            return SIGNAL(sigPadCorner5(int,int));
//        case 6:
//            return SIGNAL(sigPadCorner6(int,int));
//        case 7:
//            return SIGNAL(sigPadCorner7(int,int));
//        case 8:
//            return SIGNAL(sigPadCorner8(int,int));
//        case 9:
//            return SIGNAL(sigPadCorner9(int,int));
//        case 10:
//            return SIGNAL(sigPadCorner10(int,int));
//    }
//    return 0;
//}
//struct FW_HEADER {
//        struct {unsigned char bank,block_num_last;unsigned short buildnum,length,crc;union FW_STATUS fw_status;} PACK_INLINE fixed;
//        char versionString[20];
//};


//void DeviceManager::slotSysexRx(QByteArray &sysex)
//{
//    int buildNum;
//    bool intact;
//    QString version;
//    SysexData sysexData(sysex);

//    sysexData.process();

//    if (sysexData.firmwareStatus(buildNum,intact,version))
//    {
//        fwQueryTimer->stop();
//        emit sigFirmwareStatus(buildNum,intact,version);
//    }

//    qDebug("********** sysex rx *********** [%d]",sysex.count());
//}


//void DeviceManager::slotMidiPacket(MidiPacket *pkt)
//{
////    qDebug("DeviceManager::slotMidiPacket");
//    if (pkt->length == 3 && pkt->data[0]==STATUS_CONTROLLER)
//    {
//        int ctlr = pkt->data[1];
//        if (ctlr>=PAD_CONTROLLER_BASE && ctlr<PAD_CONTROLLER_TOP)
//        {
//            int pad = (ctlr-PAD_CONTROLLER_BASE)/CONTROLLERS_PER_PAD;
//            int corner = ctlr % CONTROLLERS_PER_PAD;
//            int val = pkt->data[2];

//            switch(pad)
//            {
//                case 0:
//                    emit sigPadCorner6(corner,val);
//                    break;
//                case 1:
//                    emit sigPadCorner1(corner,val);
//                    break;
//                case 2:
//                    emit sigPadCorner7(corner,val);
//                    break;
//                case 3:
//                    emit sigPadCorner2(corner,val);
//                    break;
//                case 4:
//                    emit sigPadCorner8(corner,val);
//                    break;
//                case 5:
//                    emit sigPadCorner3(corner,val);
//                    break;
//                case 6:
//                    emit sigPadCorner9(corner,val);
//                    break;
//                case 7:
//                    emit sigPadCorner4(corner,val);
//                    break;
//                case 8:
//                    emit sigPadCorner10(corner,val);
//                    break;
//                case 9:
//                    emit sigPadCorner5(corner,val);
//                    break;
//                case 10:
//                    emit sigPadCorner1(corner,val);
//                    break;
//            }
//        }

//    }

//}

void DeviceManager::slotStandalone(int state, int save)
{
    unsigned char *buffer;
    int len;

    t_device_standalone(&buffer, &len, state, save);

    emit sigSysex(buffer,len);
}

void DeviceManager::slotTether(int state, int save)
{
    unsigned char *buffer;
    int len;

    t_device_tether(&buffer, &len, state, save);

    emit sigSysex(buffer,len);
}

//char fw_query_syx[] = {
//        0x00,0x00,0x01,0x55,0x7A,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // F0
//        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x04,0x40,
//        0x00,0x30,0x07 // F7
//};

//void DeviceManager::slotFwQuery()
//{
//    emit sigSysex(fw_query_syx,sizeof(fw_query_syx));
//}

void DeviceManager::appQuitting()
{
    slotStandalone(1,0);
    slotTether(0,0);
}

//void DeviceManager::slotConnected()
//{
//    qDebug("device manager is connected");

////    slotStandalone(0,0);
////    slotTether(1,0);

////    fwQueryCount = 3;  // try three times
////    fwQueryTimer->start(3000);
//}

//void DeviceManager::slotDisconnected()
//{
//    qDebug("device manager is disconnected");
//}

void DeviceManager::sendMidiTriple(int status,int val1,int val2)
{
//    MidiPacket pkt;
//    pkt.length = 3;
//    pkt.data[0] = status;
//    pkt.data[1] = val1;
//    pkt.data[2] = val2;

//    emit sigMidiPacket(&pkt);

    emit sigMidiTriple(status, val1, val2);
}

void DeviceManager::slotDisplayStr(QString str)
{
    int i,chr;
    for (i=0;i<DEVICE_DISPLAY_LEN;i++) {

        if (i<str.size())
            chr = str.toLatin1().data()[i];
        else
            chr = ' ';

        sendMidiTriple(STATUS_CONTROLLER, DEVICE_DISPLAY_CONTROLLER+i,chr);
    }
}

//void DeviceManager::slotFwQueryTimeout()
//{
//    if (fwQueryCount)
//    {
//        fwQueryCount--;
//        slotFwQuery();
//        fwQueryTimer->start(2000);
//    } else
//        fwQueryTimer->stop();

//}

//------------------------- 2.0 Additions
void DeviceManager::slot_sendPresets(unsigned char *presetImage, int presetCount)
{
    //Formats our image, sends to c file, but doesn't actually send to device
    send_standalone_image(presetImage,presetCount);

    //Sends the actual image
    emit sigSysex(midi_sysex_data(),midi_sysex_len());

}

void DeviceManager::slot_sendSettings(unsigned char *settingsImage)
{
    //----------------------------------------------------------------------//

    // Previously we were getting the size of a pointer-to-unsigned-char
    // which is 8 bytes, something like: 0x7fff5fbffa78, which just
    // represents an 8 byte (64-bit) hex address.
    //
    // We really needed to get the size of the SETTINGS struct, which
    // which looks like is 15. For some reason early on in the formatting
    // process the settings variable is not a pointer, and the image
    // variable is-- which ultimately led to our confusion.
    //
    // So in short, we just needed the number of bytes to process, and we
    // were only getting the first 8 of a 15-byte container/struct.
    // Uncomment qDebug below for a "tangible" explanation...

    //----------------------------------------------------------------------//

    //Used for getting the size of the struct actual struct
    SETTINGS tempSettings;

    //Format settings
    send_standalone_settings(settingsImage, sizeof(tempSettings));

    //Sends the settings image
    emit sigSysex(midi_sysex_data(), midi_sysex_len());

    //qDebug() << "POINTER ADDRESS VALUE:" << settingsImage << "POINTER ADDRESS SIZE:" << sizeof(settingsImage) << "SETTINGS STRUCT SIZE:" << sizeof(tempSettings) << "TEMP SETTINGS STRUCT ADDRESS VALUE:" << &tempSettings;
}

