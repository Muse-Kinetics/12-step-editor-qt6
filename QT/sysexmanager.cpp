// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "sysexmanager.h"

#ifdef Q_OS_MAC
SysexManager::SysexManager(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("SysexManager");

    //create our midi manager and processor
    //midiManager = new MidiManager();

    fwQueryTimeout = new QTimer(this);
    slotResetVariables();

    //set path for sysex file and 12Step port name
    sysExPath = QCoreApplication::applicationDirPath(); //get bundle path
#if defined(Q_OS_MAC) && !defined(QT_DEBUG)
    deviceName = "12Step Port 1";
    sysExPath.remove(sysExPath.length() - 5, sysExPath.length()); //remove "MacOS" from path string
    sysExPath.append("Resources/12step.syx");
#elif defined(Q_OS_MAC)
    deviceName = "12Step Port 1"; // EB TODO - fix this for espanol
    sysExPath = "./12step.syx";
#endif

    slotSetUpMidiPorts();

    //connect(midiManager, SIGNAL(signal_MidiInputsChanged()), this, SLOT(slotMidiInputsChanged()));
    //connect(midiManager, SIGNAL(signal_MidiOutputsChanged()), this, SLOT(slotMidiOutputsChanged()));
    //connect(midiManager, SIGNAL(signal_ReceiveMidiInput(QString,MidiMessage)), this, SLOT(slotReceiveFwQuery(QString,MidiMessage)));
    //connect(fwQueryTimeout, SIGNAL(timeout()), this, SLOT(slotSendFwQuery()));
}

#else
SysexManager::SysexManager() :
    QObject()
{

}

void SysexManager::process()
{
    this->setObjectName("SysexManager");

    //create our midi manager and processor
    midiManager = new MidiManager();

    fwQueryTimeout = new QTimer(this);
    slotResetVariables();

    //set path for sysex file and 12Step port name
    sysExPath = QCoreApplication::applicationDirPath(); //get bundle path
    deviceName = "12Step";
    sysExPath = "./12step.syx";

    slotSetUpMidiPorts();

    connect(midiManager, SIGNAL(signal_MidiInputsChanged()), this, SLOT(slotMidiInputsChanged()));
    connect(midiManager, SIGNAL(signal_MidiOutputsChanged()), this, SLOT(slotMidiOutputsChanged()));
    connect(midiManager, SIGNAL(signal_ReceiveMidiInput(QString,MidiMessage)), this, SLOT(slotReceiveFwQuery(QString,MidiMessage)));
    connect(fwQueryTimeout, SIGNAL(timeout()), this, SLOT(slotSendFwQuery()));
    connect(midiManager, SIGNAL(signalResendFirmware()), this, SLOT(slotUpdateFw()));
}
#endif

void SysexManager::slotStartTimer(int ms)
{
    fwQueryTimeout->start(ms);
}

void SysexManager::slotStopTimer()
{
    fwQueryTimeout->stop();
}

// -------------------------------------------------------------------- MIDI Ports

void SysexManager::slotSetUpMidiPorts()
{
#ifdef Q_OS_MAC
    //bool inputConnected = midiManager->openInputDevice(deviceName);
    //bool outputConnected = midiManager->openOutputDevice(deviceName);

    //qDebug() << "12Step Input Opened:" << inputConnected;
    //qDebug() << "12Step Output Opened:" << outputConnected;

    //connected = outputConnected;
#else   //on Windows I must force the ports to set up as disconnected even if the 12Step is plugged in on connect
        //this prevents the Windows bug where 12Step can't send the firmware version after a hot plug if it's plugged in on load
    bool inputConnected = false;
    bool outputConnected = false;

    qDebug() << "12Step Input Opened:" << inputConnected;
    qDebug() << "12Step Output Opened:" << outputConnected;
#endif
}

void SysexManager::slotClosePorts()
{
    //midiManager->closeInputDevice(deviceName);
    //midiManager->closeOutputDevice(deviceName);
}

void SysexManager::slotMidiInputsChanged()
{
    /*
    qDebug() << "____New Input Devices:" << midiManager->getInputDevices();

    if(!midiManager->getInputDevices().isEmpty())
    {
        midiManager->openInputDevice(deviceName);
    }
    else
    {
        midiManager->closeInputDevice(deviceName);
    }*/
}

void SysexManager::slotMidiOutputsChanged()
{
    /*
    qDebug() << "____New Output Devices:" << midiManager->getOutputDevices();
    if(!midiManager->getOutputDevices().isEmpty())
    {
        connected = midiManager->openOutputDevice(deviceName);

    }
    else
    {
        connected = false;
        midiManager->closeOutputDevice(deviceName);
    }

    if(!connected)
    {
        //reset everything upon disconnection
        fwQuerySent = false;
        fwQueryMatched = false;
        fwUpdateStarted = false;
        fwUpdateCancelled = false;
        connect(midiManager, SIGNAL(signal_ReceiveMidiInput(QString,MidiMessage)), this, SLOT(slotReceiveFwQuery(QString,MidiMessage)));
        connect(fwQueryTimeout, SIGNAL(timeout()), this, SLOT(slotSendFwQuery()));
    }

    qDebug() << "connection" << connected;
    emit signalConnected(connected); */
}

// -------------------------------------------------------------------- Firmware Updating

void SysexManager::slotSendFwQuery()
{
    unsigned char fw_query_syx_12step[] = {
        0xF0,0x00,0x01,0x55,0x7A,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x04,0x40,
        0x00,0x30,0xF7
    };

    if(connected)
    {
        //qDebug() << "firmware query sent";

        fwQuerySent = true;

        //midiManager->outputSysEx(deviceName, fw_query_syx_12step, 67);

        slotQueryTimeout();
    }
    else
    {
        emit signalStopTimer();
    }
}

void SysexManager::slotQueryTimeout()
{
    if(connected && !fwQueryMatched) //if the device doesn't have the correct firmware
    {
        //qDebug() << "query timeout - no firmware match yet.";
        emit signalStartTimer(3500);
    }
    else
    {
        emit signalStopTimer();
        //qDebug() << QString("connected: %1 fwMatched: %2 fwUpdated: %3").arg(connected).arg(fwQueryMatched).arg(fwUpdated);
    }
}

//eb todo

//void SysexManager::slotReceiveFwQuery(QString sourceName, const MidiMessage &message)
void SysexManager::slotReceiveFwQuery(QString sourceName)

{
    //qDebug() << "query response" << sourceName << &message;
    //sysex message

    /*
    if(sourceName == deviceName && message.isSysEx())
    {
        unsigned char *syxData = const_cast<unsigned char*>(message.getSysExData());
        QString inquiryResponse = NULL;

        //on windows I sometimes get an initial junk sysex response followed by the real sysex response, this variable will get set to true in those cases so I can skip opening the fw dialog
        bool skip = false;

        for(int i = 0; i < message.getSysExDataSize(); i++)
        {
            if(i >= 67 && i <= 78) //only look at the part of the sysex file that is different between version numbers
            {
                inquiryResponse.append(QString("%1 ").arg(syxData[i]));
            }
        }

        if(inquiryResponse.isNull())
        {
            qDebug() << "ERROR: an unidentified sysex message was received instead of the firmware query";
            skip = true;
        }

        //check the inquiry response against the response we are looking for
        if(inquiryResponse != QString("%1").arg(CURRENT_FW_QUERY_RESPONSE))
        {
            qDebug() << QString("inquiry response: %1  check response: %2").arg(inquiryResponse).arg(CURRENT_FW_QUERY_RESPONSE);

            if(inquiryResponse == "21 43 61 47 90 8 71 71 0 0 65 49 ") //A1's fw response
            {
                emit signalFwVersion("A1");
            }
            else if(inquiryResponse == "23 43 16 14 110 72 71 71 0 0 65 50 ") //A2's fw response
            {
                emit signalFwVersion("A2");
            }
            else if(inquiryResponse == "24 43 60 50 48 8 71 71 0 0 50 52 ") //24's fw response
            {
                emit signalFwVersion("24");
            }
            else if(inquiryResponse == "25 43 112 73 110 72 71 71 0 0 50 53 ")
            {
                emit signalFwVersion("25");
            }
            else if(inquiryResponse == "26 41 76 51 25 8 71 71 0 0 50 54 ")
            {
                emit signalFwVersion("26");
            }
            else if(inquiryResponse == "27 41 76 45 33 40 71 71 0 0 50 55 ")
            {
                emit signalFwVersion("27");
            }
            else if(inquiryResponse == "0 0 0 0 0 0 0 0 0 0 0 0 ")
            {
                emit signalFwVersion("Unknown");
                skip = true;
            }
            else
            {
                emit signalFwVersion("Unknown");
            }

            fwQueryMatched = false;
//#ifdef Q_OS_MAC
            if(!fwUpdateCancelled && !skip && !fwUpdateStarted) //prevents the fw dialog from opening again after cancelling the fw update
//#else
//            if(!fwUpdateCancelled && !skip)
//#endif
            {
                emit signalOpenFwDialog();
            }
            else if(skip)
            {
                //qDebug() << "skip is true, don't open firmware update dialog. Instead, send fw query.";
                slotQueryTimeout();
            }
            else
            {
                //qDebug() << "firmware update has been cancelled";
            }
        }
        else
        {
            qDebug() << "firmware match";
            emit signalFwVersion(FW_VERSION);
            fwQueryMatched = true;
            //disconnect(midiManager, SIGNAL(signal_ReceiveMidiInput(QString,MidiMessage)), this, SLOT(slotReceiveFwQuery(QString,MidiMessage)));
            disconnect(fwQueryTimeout, SIGNAL(timeout()), this, SLOT(slotSendFwQuery()));
            emit signalStopTimer();

            //if the query matched and the updated variable is true then that should mean that fw is finished updating
            if(fwUpdated)
            {
                emit signalFwUpdateFinished();
                //qDebug() << "firmware updated";
            }
        }
    }
    else
    {
        //qDebug() << "non-sysex type received";
    }*/
}

void SysexManager::slotCancelFwUpdate()
{
    fwUpdateCancelled = true;
    emit signalStopTimer();
}

void SysexManager::slotUpdateFw()
{
    if(fwUpdated)
    {
       qDebug() << "Something weird is happening: the fwUpdated variable is true and yet the slotUpdateFw was called again. So this is probably running on windows right now, isn't it?";
    }

    emit signalFwUpdateStarted();

    //open sysex firmware file
    QFile *firmware = new QFile(sysExPath);
    if(!firmware->exists())
    {
        qDebug() << "firmware sysex file doesn't exist" << sysExPath;
    }
    else
    {
        //this keeps the firmware dialog from opening back up while the fw is updating
        //disconnect(midiManager, SIGNAL(signal_ReceiveMidiInput(QString,MidiMessage)), this, SLOT(slotReceiveFwQuery(QString,MidiMessage)));
        disconnect(fwQueryTimeout, SIGNAL(timeout()), this, SLOT(slotSendFwQuery()));
        emit signalStopTimer();

        firmware->open(QIODevice::ReadOnly);
        QByteArray firmwareByteArray = firmware->readAll();

        qDebug() << "firmware should begin updating";
        //midiManager->outputSysEx(deviceName, (unsigned char *)firmwareByteArray.data(), firmwareByteArray.size());
#ifndef Q_OS_MAC
        slotClosePorts();
#endif
        fwUpdated = true;
    }
}

//all variables used to determine smart fw updates are reset to false upon clicking "ok" in the firmware updated complete dialog
//this allows hotplugged boards to start over and also allows the force firmware update to work from the hardware menu
void SysexManager::slotResetVariables()
{
    emit signalStopTimer();
    connected = false;
    fwQuerySent = false;
    fwQueryMatched = false;
    fwUpdateStarted = false;
    fwUpdated = false;
    fwUpdateCancelled = false;
}

//---------------------- Preset and Settings Sending -----------------------//
void SysexManager::slotSendSettingsOrPresetImage(char *presetOrSettingsImage, int length)
{
    qDebug() << "send presets.";

    //midiManager->outputSysEx(deviceName, (unsigned char *)presetOrSettingsImage, length);
}
