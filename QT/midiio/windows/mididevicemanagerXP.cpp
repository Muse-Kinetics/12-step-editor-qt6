// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "mididevicemanager.h"

MidiDeviceManager::MidiDeviceManager(QWidget *parent) :
    QWidget(parent)
{
    globalsRecieved = false;
    globalPoller = new QTimer(this);
    connect(globalPoller, SIGNAL(timeout()), this, SLOT(slotPollGlobals()));

    sysExType = "None";
    globals.clear();

    qDebug() <<"ERROR" << MMSYSERR_ALLOCATED;

    inBootloader = false;
    fwUpdateRequested = false;

    //Load Firmware File into a byte array
    firmware = new QFile(":firmware/resources/firmware/QuNexus_Firmware.syx");
    firmware->open(QIODevice::ReadOnly);
    firmwareByteArray = firmware->readAll();

    //------------------------------------- Set Version Expectations & Initialize Found Strings
    bootloaderVersion[0] = 0x01;
    bootloaderVersion[1] = 0x00;
    bootloaderVersion[2] = 0x00;

    firmwareVersion[0] = 0x01;
    firmwareVersion[1] = 0x01;
    firmwareVersion[2] = 0x09;

    versionSum = 0;

    foundBootloaderVersion = QString("Found Bootloader Version: Not Connected\n");

    foundFirmwwareVersion = QString("Found Firmware Version: Not Connected\n");

    expectedBootloaderVersion = QString("Expected Bootloader Version: %1.%2.%3\n")
            .arg(int(bootloaderVersion[0]))
            .arg(int(bootloaderVersion[1]))
            .arg(int(bootloaderVersion[2]));

    expectedFirmwareVersion = QString("Expected Firmware Version: %1.%2.%3\n")
            .arg(int(firmwareVersion[0]))
            .arg(int(firmwareVersion[1]))
            .arg(int(firmwareVersion[2]));

    inHandle = NULL;
    outHandle = NULL;
    connected = false;
    refreshDevices = true;

    qDebug() << "winID" << this->winId();
}

void MidiDeviceManager::slotEnterBootloader()
{
    //-------------- Enter Bootloader ---------------//
    char bytes[] = {0xF0, 0x00, 0x01, 0x5F, 0x7A, 0x19, 0x00, 0x01,  0x00, 0x02, 0x11, 0x00, 0x5A, 0x62, 0x00, 0x30, 0xF7};
    QByteArray byteArray(bytes, 17);

    slotSendSysEx("Enter Bootloader" , byteArray, "QuNexus");

    qDebug() << "Request Enter Bootloader Called";
}

void MidiDeviceManager::slotQueryQuNexus()
{
    qDebug() << "Query Called";

    //-------------- Device Inquiry -----------------//
    char bytes[] = {0xF0, 0x7E, 0x7F, 0x06, 0x01, 0xF7};
    QByteArray byteArray(bytes, 6);

    qDebug() << bytes;
    slotSendSysEx("Device Inquiry", byteArray, "QuNexus");
}

void MidiDeviceManager::slotRestoreGlobals(QByteArray globalsByteArray)
{

#ifdef Q_OS_MAC
#else
    typedef __int16 int16_t;
#endif

    qDebug() << "RESTORE globals size" << globalsByteArray;

    QList<int> globalsList;
    QByteArray cvOffsets;

    for(int i = 0; i < globalsByteArray.length(); i++)
    {
        //qDebug() << "byte" << (unsigned int)globalsByteArray.at(i);
    }

    //An 8 byte packet of sysex chars
    unsigned char  buffer[8];
    int         numPackets = 42;


    //For each of the 42 packets...
    for(int i = 1; i <= numPackets; i++)
    {
        //Clear buffer
        //buffer.clear();

        //Load Buffer, starting with first packet, index 8
        for(int j =0; j < 8; j++)
        {

            //qDebug() << "index:" << j + (i*8) << (unsigned char)globalsByteArray.at(j + (i*8));
            //Use packet num i to get index into global Byte array
            buffer[j] = globalsByteArray.at(j + (i*8));
        }

        //Decode packet
        for(int j =0; j < 7; j++)
        {
            //If decode byte is 1
            if(buffer[7] & 0x01)
            {
                buffer[j] |= 0x80;
            }

            buffer[7] >>=1;

            //qDebug() << "packet " << i << "global index" << j + (i*8) << "packet index" << j << "val" << (unsigned int)buffer[j];

            int globalIndex = j + (i*8);
            //-------------------------------------------- Per Key Sensitivities
            if(globalIndex >= 8 && globalIndex <= 35)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 36 && globalIndex <= 181)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 182 && globalIndex <= 328)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 329 && globalIndex <= 331)
            {
                globalsList.append(buffer[j]);
            }
            else if(globalIndex >= 332 && globalIndex <= 338)
            {
                cvOffsets.append(buffer[j]);
                qDebug() << "restore this value" << buffer[j];
            }
        }
    }

    //--------------------------------- Format Offsets
    int16_t voltPerOctaveMSB = cvOffsets.at(0) & 0xFF;
    int16_t voltPerOctaveLSB = cvOffsets.at(1) & 0xFF;

    voltPerOctaveMSB =  voltPerOctaveMSB << 8;
    voltPerOctaveMSB = voltPerOctaveMSB | voltPerOctaveLSB;

    int16_t _1_2_voltPerOctaveMSB = cvOffsets.at(2) & 0xFF;
    int16_t _1_2_voltPerOctaveLSB = cvOffsets.at(3) & 0xFF;

    _1_2_voltPerOctaveMSB = _1_2_voltPerOctaveMSB << 8;
    _1_2_voltPerOctaveMSB = _1_2_voltPerOctaveMSB | _1_2_voltPerOctaveLSB;

    int16_t hzPerVolteMSB = cvOffsets.at(4) & 0xFF;
    int16_t hzPerVolteLSB = cvOffsets.at(5) & 0xFF;

    hzPerVolteMSB = hzPerVolteMSB << 8;
    hzPerVolteMSB = hzPerVolteMSB | hzPerVolteLSB;

    //-------------------------------- Append to globals list

    globalsList.append(voltPerOctaveMSB);
    globalsList.append(_1_2_voltPerOctaveMSB);
    globalsList.append(hzPerVolteMSB);

    sysExEncode->slotSysExEncodeGlobal("Encode Globals", globalsList);
}

void MidiDeviceManager::slotProcessSysEx(QByteArray sysExMessageByteArray)
{
    qDebug() << "Process SysEx";

    //---------------------------------------- Normal Mode - Inquiry Response ----------------------------------------//
    char normalInquiryResponse[] =          {0xF0, 0x7E, 0x00, 0x06, 0x02, 0x00, 0x01, 0x5F, 0x19,

                                             0x00, /* <-- bootloader bit */
                                             0x00,
                                             0x00,
                                             bootloaderVersion[0], /* <-- boot primary */
                                             bootloaderVersion[1], /* <-- boot secondary */
                                             bootloaderVersion[2], /* <-- boot tertiary */

                                             firmwareVersion[0], /* <-- fw primary */
                                             firmwareVersion[1], /* <-- fw secondary */
                                             firmwareVersion[2], /* <-- fw tertiary */

                                             0xF7
                                            };


    if(sysExMessageByteArray.size() == 19)
    {
        //--------------------------------- Set Versions and Emit
        foundBootloaderVersion = QString("Found Bootloader Version: %1.%2.%3\n")
                .arg(int(sysExMessageByteArray.at(12)))
                .arg(int(sysExMessageByteArray.at(13)))
                .arg(int(sysExMessageByteArray.at(14)));

        foundFirmwwareVersion = QString("Found Firmware Version: %1.%2.%3\n")
                .arg(int(sysExMessageByteArray.at(15)))
                .arg(int(sysExMessageByteArray.at(16)))
                .arg(int(sysExMessageByteArray.at(17)));

        versionSum = int(sysExMessageByteArray.at(15))*100 + int(sysExMessageByteArray.at(16))*10 + int(sysExMessageByteArray.at(17));

        qDebug() << "version sum" << versionSum;
    }



    //Byte array storing the above char array for easier processing
    QByteArray normalInquiryResponseBA(normalInquiryResponse,19);

    if(normalInquiryResponseBA ==  sysExMessageByteArray)
    {
        qDebug() << "Normal Response";
        inBootloader = false;

        if(fwUpdateRequested)
        {
            fwUpdateRequested = false;

            if(versionSum >= 117 && globals.count() == 345)
            {
                //slotReloadPerKeySensitivities();
                slotRestoreGlobals(globals);
            }

            emit signalProgressDialog("close", 0);
            emit signalFirmwareUpdateComplete();
        }
    }

    else if((int)sysExMessageByteArray.at(9)&& sysExMessageByteArray.size() == 19)
    {
        qDebug() << "Bootloader Response";
        inBootloader = true;

        if(fwUpdateRequested)
        {
            slotUpdateFirmware();
        }
    }

    //-------------------------------------------------- Globals
    else if(sysExDecode->slotIsGlobals(sysExMessageByteArray))
    {
        if(fwUpdateRequested)
        {
            globals.clear();
            globals = sysExMessageByteArray;

            slotEnterBootloader();
        }
        else
        {
            //Sends to GUI
            sysExDecode->slotProcessGlobals(sysExMessageByteArray);
        }
    }

    else
    {

        char deviceReplyHeader[] = {0x7E, 0x00, 0x06, 0x02, 0x00, 0x01, 0x5F, 0x19};
        if(sysExMessageByteArray.indexOf(QByteArray(deviceReplyHeader, 8)) == 1 && sysExMessageByteArray.size() == 19)
        {
            qDebug() << "Your firmware is out of date.";

            qDebug() << foundBootloaderVersion;
            qDebug() << foundFirmwwareVersion;
            qDebug() << expectedBootloaderVersion;
            qDebug() << expectedFirmwareVersion;

            emit signalFirmwareOutOfDate(expectedBootloaderVersion,foundBootloaderVersion,expectedFirmwareVersion,foundFirmwwareVersion);
        }
    }
}

void MidiDeviceManager::slotPollGlobals()
{

    if(!globalsRecieved)
    {
        slotSendGlobalsRequest();
    }
    else
    {
        globalPoller->stop();
    }

}

void MidiDeviceManager::slotRequestFirmwareUpdate()
{
    qDebug() << "Request Update Firmware Called";
    if(slotGetMidiOutDevices().contains("USB Audio Device"))
    {
        fwUpdateRequested = true;

        if(inBootloader)
        {
            qDebug() << "in bootloader request?";
            QTimer::singleShot(5000, this, SLOT(slotUpdateFirmware()));
        }
        else
        {
            emit signalProgressDialog("setup", firmwareByteArray.size());
            emit signalProgressDialog("val", firmwareByteArray.size());

            char sens[17] = { 0xF0 , 0x00 , 0x01 , 0x5F , 0x7A , 0x19 , 0x00 , 0x01 , 0x00 , 0x02 , 0x50 , 0x01 , 0x74 , 0x3E , 0x00 , 0x10 , 0xF7};
            QByteArray sensitivitiesBA = QByteArray(sens, 17);

            qDebug() << "enter bootloader called from firmware request" << versionSum;

            if(versionSum >= 117)
            {

                //slotSendSysEx("Globals Request", sensitivitiesBA, "QuNexus Port 1");
                qDebug() << "================================================= reached";

                //QTimer::singleShot(5000, this, SLOT(slotSendGlobalsRequest()));
                globalsRecieved = false;
                globalPoller->start(1000);
            }
            else
            {
                QTimer::singleShot(5000, this, SLOT(slotEnterBootloader()));
            }
        }
    }
    else
    {
        qDebug() << "QuNexus Not Connected";
    }
}

void MidiDeviceManager::slotSendGlobalsRequest()
{
    //This Query also returns the user tables
    char sens[17] = { 0xF0 , 0x00 , 0x01 , 0x5F , 0x7A , 0x19 , 0x00 , 0x01 , 0x00 , 0x02 , 0x50 , 0x01 , 0x74 , 0x3E , 0x00 , 0x10 , 0xF7};
    QByteArray sensitivitiesBA = QByteArray(sens, 17);

    slotSendSysEx("Globals Request", sensitivitiesBA, "------");

    //QTimer::singleShot(1000, this, SLOT(slotEnableGlobalsWindows()));

}

void MidiDeviceManager::slotSendSysEx(QString messageID, QByteArray sysExMessageByteArray, QString destinationName)
{
    if(slotGetMidiOutDevices().contains("USB Audio Device"))
    {
        sysExType = messageID;

        if(sysExType == QString("Globals Request"))
        {
            globalsRecieved = false;
        }

        qDebug() << "sysExType" << sysExType;

        UINT err;

        sysExOutBuffer = GlobalAlloc(GHND, sysExMessageByteArray.size());
        sysExOutHdr.lpData = (LPSTR)GlobalLock(sysExOutBuffer);
        sysExOutHdr.dwBufferLength = sysExMessageByteArray.size();
        sysExOutHdr.dwFlags = 0;

        err = midiOutPrepareHeader(outHandle, &sysExOutHdr, sizeof(MIDIHDR));

        if(!err)
        {
            memcpy(sysExOutHdr.lpData, sysExMessageByteArray.data(), sysExMessageByteArray.size());

            qDebug() << "flags" << sysExOutHdr.dwFlags << MHDR_DONE << MHDR_INQUEUE << MHDR_ISSTRM << MHDR_PREPARED;

            err = midiOutLongMsg(outHandle, &sysExOutHdr, sizeof(MIDIHDR));

            if(err)
            {
                char errMsg[120];
                midiOutGetErrorText(err, (LPWSTR)errMsg, 120);
                qDebug()<<"err:" << errMsg;
            }
        }
    }
    else
    {
        qDebug() << "QuNexus Not Connected";
    }
}

void MidiDeviceManager::slotUpdateFirmware()
{
    qDebug() << "Send the firmware!" << firmwareByteArray;
    slotSendSysEx("Download Firmware", firmwareByteArray, "QuNexus");
    //slotSendSysEx(firmwareByteArray, "QuNexus");
}

bool MidiDeviceManager::slotConnectDisconnectDevice()
{
    qDebug() << "slot Connect Disconnect";
    if(slotGetMidiInDevices().contains("USB Audio Device"))
    {
        slotCloseMidiIn();
        slotCloseMidiOut();

        slotOpenMidiIn(slotGetMidiInDevices().value("USB Audio Device"));
        slotOpenMidiOut(slotGetMidiOutDevices().value("USB Audio Device"));
        emit signalQuNexusConnected(true);
        connected = true;
        qDebug() << "___ Connected ___";
        slotQueryQuNexus();
    }
    else if(!slotGetMidiInDevices().contains("USB Audio Device"))
    {
        qDebug() << "close????";
        slotCloseMidiIn();
        slotCloseMidiOut();
        emit signalQuNexusConnected(false);
        connected = false;
        qDebug() << "___ Disconnected ___";
    }

    return false;
}

QMap<QString, int> MidiDeviceManager::slotGetMidiInDevices()
{
    QMap<QString, int> devicesMap; //Stores: <QString deviceName, int deviceIndex> pairs

    MIDIINCAPS capabilities;

    //qDebug() << "Number of Devices" << midiInGetNumDevs();

    for(uint i =0; i<midiInGetNumDevs(); i++)
    {
        midiInGetDevCaps(i, &capabilities, sizeof(MIDIINCAPS));

        QString deviceName;
        uint charNum = 0;

        while(capabilities.szPname[charNum] != '\0')
        {
            deviceName.append(QChar(capabilities.szPname[charNum]));
            charNum++;
        }

        devicesMap.insert(deviceName, i);
    }

    qDebug() << devicesMap;

    return devicesMap;
}

QMap<QString, int> MidiDeviceManager::slotGetMidiOutDevices()
{
    QMap<QString, int> devicesMap; //Stores: <QString deviceName, int deviceIndex> pairs

    MIDIOUTCAPS capabilities;

    //qDebug() << "Number of Devices" << midiInGetNumDevs();

    for(uint i =0; i<midiOutGetNumDevs(); i++)
    {
        midiOutGetDevCaps(i, &capabilities, sizeof(MIDIOUTCAPS));

        QString deviceName;
        uint charNum = 0;

        while(capabilities.szPname[charNum] != '\0')
        {
            deviceName.append(QChar(capabilities.szPname[charNum]));
            charNum++;
        }

        devicesMap.insert(deviceName, i);
    }

    return devicesMap;
}

void MidiDeviceManager::slotOpenMidiIn(int index){

    DWORD   err;

    /* Is it not yet open? */
    if (!inHandle)
    {
        /* Open MIDI Input and set Windows to call my
          midiInputEvt() callback function. You may prefer
          to have something other than CALLBACK_FUNCTION. Also,
          I open device 0. You may want to give the user a choice */
        if (!(err = midiInOpen(&inHandle, index, (DWORD_PTR)MidiDeviceManager::midiInCallback, (DWORD_PTR)this, CALLBACK_FUNCTION)))
        {
            sysExInBuffer = GlobalAlloc(GHND, 500); //allocate sysex input buffer
            if(sysExInBuffer)
            { //if exists...
                sysExInHdr.lpData = (LPSTR)GlobalLock(sysExInBuffer); //set pointer to our buffer in header struct
                if(sysExInHdr.lpData)
                { //if above pointer successfully set...
                    sysExInHdr.dwBufferLength = 500; //allocate an input of 500 byte length
                    sysExInHdr.dwFlags = 0; //no flags

                    err = midiInPrepareHeader(inHandle, &sysExInHdr, sizeof(MIDIHDR)); //prepare the header (return MMRESULT err)
                    if(err == MMSYSERR_NOERROR)
                    { //if not error...
                        err = midiInAddBuffer(inHandle, &sysExInHdr, sizeof(MIDIHDR)); //add the buffer to our current device
                        if(err == MMSYSERR_NOERROR)
                        { //if no error...
                            err = midiInStart(inHandle); //start midi input***** (there's no midi out start)
                            if(err != MMSYSERR_NOERROR)
                            { //if error in starting...
                                qDebug("couldn't open midi in"); //print error
                            }
                            else
                            {
                                qDebug("device open"); //if successful, print device open
                            }
                        }
                    }
                }
            }

            /* Start recording Midi and return if SUCCESS */
            if (!(err = midiInStart(inHandle)))
            {

                //return(0);
            }

            /* ============== ERROR ============== */

            /* Close MIDI In and zero handle */
            //slotCloseMidiIn();


        }

        /* Return the error */
        qDebug() << "OPEN MIDI IN ERR:"<< (err);

    }
}

void MidiDeviceManager::slotOpenMidiOut(int index){

    DWORD   err;

    /* Is it not yet open? */
    if (!outHandle)
    {
        /* Open MIDI Output. */
        if (!(err = midiOutOpen(&outHandle, index, (DWORD_PTR)midiOutCallback, (DWORD_PTR)this, CALLBACK_FUNCTION)))
        {
            //return(0);
        }

        /* ============== ERROR ============== */
    }

    /* Return the error */
    qDebug() << "OPEN MIDI OUT ERR:"<< (err);

}

void MidiDeviceManager::slotCloseMidiIn(){

    DWORD   err;

    /* Is the device open? */
    if ((err = (DWORD)inHandle))
    {
        /* Unqueue any buffers we added. If you don't
          input System Exclusive, you won't need this */
        midiInReset(inHandle);

        /* Close device */
        if (!(err = midiInClose(inHandle)))
        {
            /* Clear handle so that it's safe to call closeMidiIn() anytime */
            inHandle = 0;
        }
    }

    qDebug() << "CLOSE MIDI IN ERR" << err;
}

void MidiDeviceManager::slotCloseMidiOut(){
    DWORD   err;

    /* Is the device open? */
    if ((err = (DWORD)outHandle))
    {
        /* If you have any system exclusive buffers that
                       you sent via midiOutLongMsg(), and which are still being output,
                       you may need to wait for their MIDIERR_STILLPLAYING flags to be
                       cleared before you close the device. Some drivers won't close with
                       pending output, and will give an error. */

        /* Close device */
        if (!(err = midiOutClose(outHandle)))
        {
            /* Clear handle so that it's safe to call closeMidiOut() anytime */
            outHandle = 0;
        }
    }


    qDebug() << "CLOSE MIDI OUT ERR" << err;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////// Callbacks ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CALLBACK MidiDeviceManager::midiInCallback(HMIDIIN hMidiIn,UINT wMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2)
{
    qDebug() << "midi in" << dwParam1 << dwParam2;

    /*/qDebug() << MIM_DATA << uMsg;
    //qDebug() << dwParam;
    qDebug()    <<"status"<< ((dwParam) & 0xFF)
                << "data1" << ((dwParam>>8) & 0xFF)
                << "data2" << ((dwParam>>16) & 0xFF); //status byte*/

    MidiDeviceManager *mda = (MidiDeviceManager *) dwInstance;

    switch(wMsg){
    case MIM_OPEN:
        qDebug("MMOPEN");
        break;
    case MIM_CLOSE:
        qDebug("MIM_ClOSE");
        break;
    case MIM_DATA:
        qDebug("MIM_DATA");
        break;
    case MIM_LONGDATA:
    {
        qDebug("MIM_LONGDATA");

        LPMIDIHDR lpMidiHdr = (LPMIDIHDR) dwParam1;

        if(lpMidiHdr->dwBytesRecorded)
        {

            if(mda->sysExType == QString("Globals Request") && lpMidiHdr->dwBytesRecorded == 345)
            {
                mda->globalsRecieved = true;
                qDebug() << "globals received" << mda->globalsRecieved;


            }

            qDebug() << "----------- Bytes Recorded -----------" << lpMidiHdr->dwBytesRecorded;

            mda->slotProcessSysEx(QByteArray(lpMidiHdr->lpData, lpMidiHdr->dwBytesRecorded));

            midiInAddBuffer(hMidiIn, lpMidiHdr, sizeof(MIDIHDR));
        }
    }

        break;
    default:
        qDebug("in callback");
        break;
    }
}

void CALLBACK MidiDeviceManager::midiOutCallback(HMIDIOUT handle, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam, DWORD_PTR dwParam1)
{
    qDebug() <<  "msg type" << uMsg << MOM_DONE << MOM_CLOSE << MOM_OPEN;

    MidiDeviceManager *mda = (MidiDeviceManager *) dwInstance;

    if(uMsg == MOM_DONE){
        midiOutUnprepareHeader(mda->outHandle, &mda->sysExOutHdr, sizeof(MIDIHDR));
        GlobalUnlock(mda->sysExOutBuffer);
        GlobalFree(mda->sysExOutBuffer);
    }
}

bool MidiDeviceManager::winEvent(MSG *message, long *result)
{
    // qDebug() << "Window Event";
    if(message->message == WM_DEVICECHANGE && message->wParam == DBT_DEVNODES_CHANGED)
    {
        if(refreshDevices)
        {
            QTimer::singleShot(5000, this, SLOT(slotRefreshDevices()));
            refreshDevices = false;
        }
    }

    return false;
}

void MidiDeviceManager::slotRefreshDevices()
{
    qDebug() << "slot Refresh Devices" << midiInGetNumDevs();
    slotConnectDisconnectDevice();

    refreshDevices = true;
}
