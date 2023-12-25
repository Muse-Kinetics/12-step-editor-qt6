// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "./midiio/mac/MidiDeviceManager.h"

/*12s_MidiDeviceManager *callbackClassPointer;
void midiSystemChanged(const MIDINotification *message, void *refCon);                          //Called when the system's MIDI has changed
void incomingMidi(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);    //Called upon incoming midi from connected port
void sysExComplete(MIDISysexSendRequest*);    */                                                  //Called when sysex event has been completely sent


12s_MidiDeviceManager::12s_MidiDeviceManager(QWidget *parent) :
    QWidget(parent)
{

    globals.clear();

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

    callbackClassPointer = this;

    createAppMidiClient();
}

void 12s_MidiDeviceManager::createAppMidiClient()
{
    MIDIClientCreate(CFSTR("QuNexus MIDI Client"), midiSystemChanged, this, &appClientRef);
    MIDIInputPortCreate(appClientRef, CFSTR("QuNexus MIDI Client In Port"), incomingMidi, this, &appInPortRef);
    MIDIOutputPortCreate(appClientRef, CFSTR("QuNexus MIDI Client Out Port"), &appOutPortRef);
}

QMap<QString, int> 12s_MidiDeviceManager::getMidiSources()
{
    QMap<QString, int> sourceMap;

    //Get Sources
    for(unsigned long i=0; i<MIDIGetNumberOfSources(); i++)
    {
        sourceMap.insert(getDisplayName(MIDIGetSource(i)), i);
    }

    return sourceMap;
}

QMap<QString, int> 12s_MidiDeviceManager::getMidiDestinations()
{
    QMap<QString, int> destinationMap;

    //Get Destinations
    for(unsigned long i=0; i<MIDIGetNumberOfDestinations(); i++)
    {
        destinationMap.insert(getDisplayName(MIDIGetDestination(i)), i);
    }

    return destinationMap;
}

int 12s_MidiDeviceManager::slotFindQuNexusPort1Source()
{

    QList<int> qunexiIndicies;
    bool qunexusFound = false;

    //Get device indexes of QuNexi
    for(unsigned long i=0; i<MIDIGetNumberOfSources(); i++)
    {
        if(getDisplayName(MIDIGetSource(i)).contains("Max"))
        {
            qunexiIndicies.append(i);
            qunexusFound = true;
        }
    }

    if(qunexusFound)
    {

        //Get lowest index which should be port 1
        int port1Index = qunexiIndicies.at(0);

        for(int i =0; i < qunexiIndicies.count(); i++)
        {
            if(qunexiIndicies.at(i) < port1Index)
            {
                port1Index = qunexiIndicies.at(i);
            }
        }

        qDebug() << "max port index: " << port1Index;
        return port1Index;
    }
    else
    {
        return -1;
    }
}

int 12s_MidiDeviceManager::slotFindQuNexusPort1Dest()
{

    QList<int> qunexiIndicies;
    bool qunexusFound = false;

    //Get device indexes of QuNexi
    for(unsigned long i=0; i<MIDIGetNumberOfDestinations(); i++)
    {
        if(getDisplayName(MIDIGetDestination(i)).contains("Max"))
        {
            qDebug() << getDisplayName(MIDIGetDestination(i));
            qunexiIndicies.append(i);
            qunexusFound = true;
        }
    }

    if(qunexusFound)
    {

        //Get lowest index which should be port 1
        int port1Index = qunexiIndicies.at(0);

        for(int i =0; i < qunexiIndicies.count(); i++)
        {
            if(qunexiIndicies.at(i) < port1Index)
            {
                port1Index = qunexiIndicies.at(i);
            }
        }

        return port1Index;
    }
    else
    {
        return -1;
    }
}

bool 12s_MidiDeviceManager::connectSource(QString sourceName)
{

    if(slotFindQuNexusPort1Source() != -1)
    {
        //Connect Source
        MIDIEndpointRef endpointRef = MIDIGetSource(slotFindQuNexusPort1Source());
        MIDIPortConnectSource(appInPortRef, endpointRef, &endpointRef);
        qDebug() << "Source Connected: " << sourceName;
        emit signalQuNexusConnected(true);
        queryQuNexus();
        return true;
    }
    else
    {
        foundBootloaderVersion = QString("Found Bootloader Version: Not Connected\n");
        foundFirmwwareVersion = QString("Found Firmware Version: Not Connected\n");

        qDebug() << "Matching Source: " << sourceName << " NOT Found!";
        emit signalQuNexusConnected(false);
        return false;
    }
}

void 12s_MidiDeviceManager::queryQuNexus()
{
    qDebug() << "Query Called";

    //-------------- Device Inquiry -----------------//
    // EB TODO - removed for build
    char bytes[] = {static_cast<char>(0xF0), 0x7E, 0x7F, 0x06, 0x01, static_cast<char>(0xF7)};
    //char bytes[] = {0, 0, 0, 0, 0 ,0}; // temp
    QByteArray byteArray(bytes, 6);

    slotSendSysEx("Device Inquiry", byteArray, "QuNexus");
}

void 12s_MidiDeviceManager::slotRequestFirmwareUpdate()
{

    fwUpdateRequested = true;

    if(inBootloader)
    {
        qDebug() << "Update Firmware Called from slot requested";
        slotUpdateFirmware();
    }
    else
    {
        //EB TODO for build
        //char sens[17];
        char sens[17] = { static_cast<char>(0xF0) , 0x00 , 0x01 , 0x5F , 0x7A , 0x19 , 0x00 , 0x01 , 0x00 , 0x02 , 0x50 , 0x01 , 0x74 , 0x3E , 0x00 , 0x10 , static_cast<char>(0xF7)};
        QByteArray sensitivitiesBA = QByteArray(sens, 17);

        qDebug() << "enter bootloader called from firmware request" << "zero" << (int)firmwareVersion[0] << "two" << (int)firmwareVersion[2];

        if(versionSum >= 117)
        {
            slotSendSysEx("RequestGlobals", sensitivitiesBA, "QuNexus");
        }
        else
        {
            slotEnterBootloader();
        }
    }
}

void 12s_MidiDeviceManager::slotEnterBootloader()
{
    //-------------- Enter Bootloader ---------------//
    // EB TODO - fix for build
    //char bytes[17];
    char bytes[] = {static_cast<char>(0xF0), 0x00, 0x01, 0x5F, 0x7A, 0x19, 0x00, 0x01,  0x00, 0x02, 0x11, 0x00, 0x5A, 0x62, 0x00, 0x30, static_cast<char>(0xF7)};

    QByteArray byteArray(bytes, 17);

    slotSendSysEx("Enter Bootloader" , byteArray, "Max");
}

void 12s_MidiDeviceManager::slotUpdateFirmware()
{
    qDebug() << "Send the firmware!" << firmwareByteArray;
    emit signalProgressDialog("setup", firmwareByteArray.size());
    //slotSendSysEx(firmwareByteArray, "QuNexus Port 1");
}


////////////////////////////////////////////////////////
///////////////// Helper Funcitons /////////////////////
////////////////////////////////////////////////////////
QString 12s_MidiDeviceManager::getDisplayName(MIDIObjectRef object)
{
    // Returns the display name of a given MIDIObjectRef as an NSString
    CFStringRef name = nil; //place holder for name

    if(noErr != MIDIObjectGetStringProperty(object, kMIDIPropertyDisplayName, &name))
    {//get the name using midi services function
        return nil;
    }

    return QString(cFStringRefToQString(name)); //return the name
}

QString 12s_MidiDeviceManager::cFStringRefToQString(CFStringRef ref)
{
    //this function just translates a CFStringRef into a QString
    CFRange range;
    range.location = 0;
    range.length = CFStringGetLength(ref);
    QString result(range.length, QChar(0));

    UniChar *chars = new UniChar[range.length];
    CFStringGetCharacters(ref, range, chars);
    //[nsstr getCharacters:chars range:range];
    result = QString::fromUtf16(chars, range.length);
    delete[] chars;
    return result;
}

void 12s_MidiDeviceManager::slotSendSysEx(QString messageID,QByteArray sysExMessageByteArray, QString destinationName)
{
    //qDebug() << "slotSendSysEx called";

    if(slotFindQuNexusPort1Dest() != -1)
    {
        //Creat char array to hold sysex bytes
        char* sysExCharData = new char(sysExMessageByteArray.size());

        //Assign bytes to char array
        sysExCharData = sysExMessageByteArray.data();

        //Create new sysex event/request
        MIDISysexSendRequest* sysExMsgReq = new MIDISysexSendRequest;

        //Set the message's fields
        sysExMsgReq->destination = MIDIGetDestination(slotFindQuNexusPort1Dest());
        sysExMsgReq->data = (const Byte *)sysExCharData;
        sysExMsgReq->bytesToSend = sysExMessageByteArray.size();
        sysExMsgReq->complete = false;
        sysExMsgReq->completionProc = &sysExComplete;
        sysExMsgReq->completionRefCon = sysExCharData;

        //Send the message
        MIDISendSysex(sysExMsgReq);

        //Wait for entire message to be sent
        uint32 bytes = -1;

        while(!sysExMsgReq->complete)
        {
            //qDebug() << "BYTES LEFT" << sysExMsgReq->bytesToSend;

            if(bytes != sysExMsgReq->bytesToSend)
            {
                bytes = sysExMsgReq->bytesToSend;
            }

            if(messageID == "Download Firmware")
            {
                emit signalProgressDialog("byte", sysExMsgReq->bytesToSend);
            }

            qDebug() << "sysEx msg bytes left:" << bytes;

        }

        if(messageID == "Download Firmware")
        {
            emit signalProgressDialog("byte", 0);
        }

    }
    else
    {
        qDebug() << "Matching Destination: " << destinationName << " NOT Found!";
    }
}

void 12s_MidiDeviceManager::slotProcessSysEx(QByteArray sysExMessageByteArray)
{

    qDebug() << "response size" << sysExMessageByteArray.size() << "version sum" << versionSum;

    //---------------------------------------- Normal Mode - Inquiry Response ----------------------------------------//

    // EB TODO for build
    //char normalInquiryResponse[17];
    char normalInquiryResponse[] =          {static_cast<char>(0xF0), 0x7E, 0x00, 0x06, 0x02, 0x00, 0x01, 0x5F, 0x19,

                                             0x00, /* <-- bootloader bit */
                                             0x00,
                                             0x00,
                                             bootloaderVersion[0], /* <-- boot primary */
                                             bootloaderVersion[1], /* <-- boot secondary */
                                             bootloaderVersion[2], /* <-- boot tertiary */

                                             firmwareVersion[0], /* <-- fw primary */
                                             firmwareVersion[1], /* <-- fw secondary */
                                             firmwareVersion[2], /* <-- fw tertiary */

                                             static_cast<char>(0xF7)
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
    }


    //Byte array storing the above char array for easier processing
    QByteArray normalInquiryResponseBA(normalInquiryResponse,19);

    if(normalInquiryResponseBA ==  sysExMessageByteArray)
    {
        qDebug() << "Normal Response" << versionSum << globals.count();
        inBootloader = false;

        if(fwUpdateRequested)
        {
            fwUpdateRequested = false;


            if(versionSum >= 117 && globals.count() == 345)
            {
                //----------------------------- Restore Globals after Firmware Update
                //sysExDecode->slotProcessGlobals(sysExMessageByteArray);
                slotRestoreGlobals(globals);
            }

            //----------------------------- Signal Update Complete
            emit signalFirmwareUpdateComplete();
        }
    }
    else if((int)sysExMessageByteArray.at(9) && sysExMessageByteArray.size() == 19)
    {
        qDebug() << "Bootloader Response";
        inBootloader = true;

        if(fwUpdateRequested)
        {
            slotUpdateFirmware();
        }
    }

    //------------------------------------------------------------- Globals
    else if(sysExDecode->slotIsGlobals(sysExMessageByteArray))
    {

        qDebug() << "globals byte count" << sysExMessageByteArray.size();

        if(fwUpdateRequested)
        {
            globals.clear();
            globals = sysExMessageByteArray;

            slotEnterBootloader();
        }
        else
        {
            //Sends to GUI
            //emit signalSendPerKeySensitivities(sysExMessageByteArray);
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

void 12s_MidiDeviceManager::slotRestoreGlobals(QByteArray globalsByteArray)
{
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

////////////////////////////////////////////////////////
////////////////Non-Class Callbacks ////////////////////
////////////////////////////////////////////////////////
void midiSystemChanged(const MIDINotification *message, void *refCon)
{
    qDebug() << "MIDI System Changed"<< message->messageID << "refcon: " << refCon;

    if(message->messageID == kMIDIMsgObjectAdded)
    {
        MIDIObjectAddRemoveNotification *msg = (MIDIObjectAddRemoveNotification *)message;

        if(msg->childType == kMIDIObjectType_Source)
        {
            if(callbackClassPointer->getDisplayName(msg->child).contains("Max") && callbackClassPointer->getDisplayName(msg->child).contains("1"))
            {
                //qDebug() << "Added Source" << callbackClassPointer->getDisplayName(msg->child);
                callbackClassPointer->connectSource("Max");
            }
        }
    }

    else if(message->messageID == kMIDIMsgObjectRemoved)
    {
        MIDIObjectAddRemoveNotification *msg = (MIDIObjectAddRemoveNotification *)message;

        if(msg->childType == kMIDIObjectType_Source)
        {
            if(callbackClassPointer->getDisplayName(msg->child).contains("Max") && callbackClassPointer->getDisplayName(msg->child).contains("1"))
            {
                //qDebug() << "Added Source" << callbackClassPointer->getDisplayName(msg->child);
                callbackClassPointer->connectSource("Max");
            }
        }
    }
}

void incomingMidi(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon){

    qDebug() << "readProcRefCon: " << readProcRefCon << "srcConnRefCon: " << srcConnRefCon; // to clear warning

    //iterate through midi packets and process according to type
    const MIDIPacket *packet = &pktlist->packet[0];

    //for number packets in packet list
    for(uint32 i =0; i < pktlist->numPackets; i++)
    {
        //for length of packet
        for(int j = 0; j < packet->length; j++)
        {
            //If a SysEx Start Byte, set filter switch
            if(packet->data[j] == 240)
            {
                callbackClassPointer->isSysEx = true;
            }

            //If a SysEx End Byte, set filter switch off and send last bytes
            else if (packet->data[j] == 247)
            {
                callbackClassPointer->isSysEx = false;
                callbackClassPointer->sysExMessage.append(packet->data[j]);
                //qDebug() << "----- SysEx In ----- :" << packet->data[j];

                callbackClassPointer->slotProcessSysEx(callbackClassPointer->sysExMessage);
                callbackClassPointer->sysExMessage.clear();
            }

            //Processes SysEx
            if(callbackClassPointer->isSysEx)
            {
                callbackClassPointer->sysExMessage.append(packet->data[j]);
                //qDebug() << "----- SysEx In ----- :" << packet->data[j];
            }
            else if(packet->data[j] != 247)
            {
                qDebug() << "MIDI Channel Event: " << packet->data[j];
            }
        }

        //advance packet in midi packet list
        packet = MIDIPacketNext(packet);
    }
}

void sysExComplete(MIDISysexSendRequest* request)
{
    qDebug() << "Sys Ex Sent:" << request;
}
