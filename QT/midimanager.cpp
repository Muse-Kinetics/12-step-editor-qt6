// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

//
//  MidiManager.cpp
//  qt_juce
//
//  Created by Conner Lacy on 4/15/14.
//  Copyright 2014. All rights reserved.
//

#include "midimanager.h"
#include "./jucesupport/JuceLibraryCode/JuceHeader.h"
#include <QString>

MidiManager::MidiManager(QObject *parent) :
    QObject(parent)
{
    //register juce::MidiMessage class so it can be used across signals/slots
    qRegisterMetaType<MidiMessage>("MidiMessage");

    //device polling, used to detect hot pluggin / midi system changes
    connect(&devicePoller, SIGNAL(timeout()), this, SLOT(slot_PollDevices()));
    devicePoller.start(500);
}

MidiManager::~MidiManager()
{

}

QStringList MidiManager::getInputDevices()
{
    //this function essentially converts a juce::StringArray to a QStringList
    QStringList inputsStringList;
    StringArray inputs = MidiInput::getDevices();

    for(int i = 0; i < inputs.size(); i++)
    {
        inputsStringList.append(QString(inputs[i].toUTF8()));
    }

    //Update output array and index maps, keeps unavailable devices out of our input array and index lookup
    QMapIterator<QString, int> i(midiInputArrayLookup);
    QStringList removalList;

    //look through output map
    while(i.hasNext())
    {
        i.next();

        //if our updated device list does not contain a device in out midiInputArrayLookup...
        if(!inputsStringList.contains(i.key()))
        {
            //add this item to our map removalList (can't remove while using qmapiterator)
            removalList.append(i.key());
            //remove it from lookup and input array
            midiInputs.remove(i.value(), true);
        }
    }

    //go through removal list and remove from output index lookup map
    for(int i = 0; i < removalList.size(); i++)
    {
        midiInputArrayLookup.remove(removalList.at(i));
    }

    return inputsStringList;
}

QStringList MidiManager::getOutputDevices()
{
    //this function essentially converts a juce::StringArray to a QStringList
    QStringList outputsStringList;
    StringArray outputs = MidiOutput::getDevices();

    for(int i = 0; i < outputs.size(); i++)
    {
        outputsStringList.append(QString(outputs[i].toUTF8()));
    }

    //update ouput array and index maps, keeps unavailable devices out of our output array and index lookup
    QMapIterator<QString, int> i(midiOutputArrayLookup);
    QStringList removalList;

    //look through output map
    while(i.hasNext())
    {
        i.next();

        //if our updated device list does not contain a device in out midiOutputArrayLookup...
        if(!outputsStringList.contains(i.key()))
        {
            //add this item to our map removalList (can't remove while using qmapiterator)
            removalList.append(i.key());
            //remove it from lookup and output array
            midiOutputs.remove(i.value(), true);
        }
    }

    //go through removal list and remove from output index lookup map
    for(int i = 0; i < removalList.size(); i++)
    {
        midiOutputArrayLookup.remove(removalList.at(i));
    }

    return outputsStringList;
}

bool MidiManager::openInputDevice(QString deviceName)
{
    QStringList inputs = getInputDevices();

    //is this device name in our list?
    if(inputs.contains(deviceName))
    {
        if(midiInputs.add(MidiInput::openDevice(inputs.indexOf(deviceName), this)) != nullptr)
        {
            midiInputArrayLookup.insert(deviceName, midiInputs.size() - 1);
            midiInputs.getLast()->start();
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

bool MidiManager::openOutputDevice(QString deviceName)
{
    QStringList outputs = getOutputDevices();

    //is this device name in our list?
    if(outputs.contains(deviceName))
    {
        if(midiOutputs.add(MidiOutput::openDevice(outputs.indexOf(deviceName))) != nullptr)
        {
            //stores the index of this device name in the midiOutput array, so we can close it later via deviceName lookup
            midiOutputArrayLookup.insert(deviceName, midiOutputs.size() -1);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

bool MidiManager::closeInputDevice(QString deviceName)
{
    if(midiInputArrayLookup.contains(deviceName))
    {
        midiInputs[midiInputArrayLookup.value(deviceName)]->stop();
        midiInputs.remove(midiInputArrayLookup.value(deviceName));
        midiInputArrayLookup.remove(deviceName);

        return 1;
    }
    return 0;
}

bool MidiManager::closeOutputDevice(QString deviceName)
{
    if(midiOutputArrayLookup.contains(deviceName))
    {
        midiOutputs.remove(midiOutputArrayLookup.value(deviceName));
        midiOutputArrayLookup.remove(deviceName);

        return 1;
    }
    return 0;
}

void MidiManager::outputSysEx(QString deviceName, unsigned char *data, int size)
{
    //qDebug() << "output sysex called";
    //if device is in our output array (and therefore our lookup)
    if(midiOutputArrayLookup.contains(deviceName))
    {
        //what's the index of our array
        int midiOutputsIndex = midiOutputArrayLookup.value(deviceName);

        //if index is within range of midiOutputs array
        if(midiOutputsIndex <= (midiOutputs.size() - 1))
        {
            //create new midi message with a sysex data
            MidiMessage message = MidiMessage(data, size, 0);

            //send midi message out through one of our devices in the midiOutputs array
            midiOutputs[midiOutputsIndex]->sendMessageNow(message);
        }
        //index is beyond what is contained in outputs array
        else
        {
            qDebug() << "____ERROR: Cannot send sysex to" << deviceName << "-- index out of range.";
        }
    }
    //if value is not contained in our output array
    else
    {
        qDebug() << "____ERROR: Cannot send sysex to" << deviceName << "-- not found in output array lookup." << "size:" << size;

#ifndef Q_OS_MAC
        //12Step specific code here - to make sure firmware is update on windows
        if(size == 73930) // if the sysex message is the whole firmware udpate
        {
            this->openOutputDevice(deviceName);
            this->openInputDevice(deviceName);
            emit signalResendFirmware();
        }
#endif
    }

    //updates arrays, maps as soon as firmware is finished
    getInputDevices();
    getOutputDevices();
}

void MidiManager::outputCC(QString deviceName, int channel, int controllerNum, int controllerValue)
{
    //if device is in our output array (and therefore our lookup)
    if(midiOutputArrayLookup.contains(deviceName))
    {
        //what's the index of our array
        int midiOutputsIndex = midiOutputArrayLookup.value(deviceName);

        //if index is within range of midiOutputs array
        if(midiOutputsIndex <= (midiOutputs.size() - 1))
        {
            //send midi message out through one of our devices in the midiOutputs array
            midiOutputs[midiOutputsIndex]->sendMessageNow(MidiMessage::controllerEvent(channel, controllerNum, controllerValue));
        }
        //index is beyond what is contained in outputs array
        else
        {
            qDebug() << "____ERROR: Cannot send CC to" << deviceName << "-- index out of range.";
        }
    }
    //if value is not contained in our output array
    else
    {
        qDebug() << "____ERROR: Cannot send CC to" << deviceName << "-- not found in output array lookup.";
    }
}

void MidiManager::outputNoteOn(QString deviceName, int channel, int noteNum, int noteVelocity)
{
    //If device is in our output array (and therefore our lookup)
    if(midiOutputArrayLookup.contains(deviceName))
    {
        //What's the index of our array
        int midiOutputsIndex = midiOutputArrayLookup.value(deviceName);

        //If index is within range of midiOutputs array
        if( midiOutputsIndex <= (midiOutputs.size() - 1) )
        {
            //Send midi message out through one of our devices in the midiOutputs array
            midiOutputs[midiOutputsIndex]->sendMessageNow(MidiMessage::noteOn(channel, noteNum, (uint8)noteVelocity));
        }
        //Index is beyond what is contained in outputs array
        else
        {
            qDebug() << "____ERROR: Cannot send Note On to" << deviceName << "-- index out of range.";
        }
    }
    //If value is not contained in our output array
    else
    {
        qDebug() << "____ERROR: Cannot send Note On to" << deviceName << "-- not found in output array lookup.";
    }
}

void MidiManager::outputNoteOff(QString deviceName, int channel, int noteNum, int noteVelocity)
{
    //If device is in our output array (and therefore our lookup)
    if(midiOutputArrayLookup.contains(deviceName))
    {
        //What's the index of our array
        int midiOutputsIndex = midiOutputArrayLookup.value(deviceName);

        //If index is within range of midiOutputs array
        if( midiOutputsIndex <= (midiOutputs.size() - 1) )
        {
            //Send midi message out through one of our devices in the midiOutputs array
            midiOutputs[midiOutputsIndex]->sendMessageNow(MidiMessage::noteOff(channel, noteNum, (uint8)noteVelocity));
        }
        //Index is beyond what is contained in outputs array
        else
        {
            qDebug() << "____ERROR: Cannot send Note Off to" << deviceName << "-- index out of range.";
        }
    }
    //If value is not contained in our output array
    else
    {
        qDebug() << "____ERROR: Cannot send Note Off to" << deviceName << "-- not found output array lookup.";
    }
}

void MidiManager::outputProgramChange(QString deviceName, int channel, int programNum)
{
    //If device is in our output array (and therefore our lookup)
    if(midiOutputArrayLookup.contains(deviceName))
    {
        //What's the index of our array
        int midiOutputsIndex = midiOutputArrayLookup.value(deviceName);

        //If index is within range of midiOutputs array
        if( midiOutputsIndex <= (midiOutputs.size() - 1) )
        {
            //Send midi message out through one of our devices in the midiOutputs array
            midiOutputs[midiOutputsIndex]->sendMessageNow(MidiMessage::programChange(channel, programNum));
        }
        //Index is beyond what is contained in outputs array
        else
        {
            qDebug() << "____ERROR: Cannot send Program Change to" << deviceName << "-- index out of range.";
        }
    }
    //If value is not contained in our output array
    else
    {
        qDebug() << "____ERROR: Cannot send Program Change to" << deviceName << "-- not found output array lookup.";
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// Private //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MidiManager::slot_PollDevices()
{
    //this function reports hot plugging/unplugging
    QStringList inputs = getInputDevices();
    QStringList outputs = getOutputDevices();

    //check MidiInputs
    //if input device list is unequal to last poll
    if(polledMidiInputDevices != inputs)
    {
        //update polled stored input device list
        polledMidiInputDevices = inputs;

        //emit signal indicating a change in input devices
        emit signal_MidiInputsChanged();
    }

    //check MidiOutputs
    //if output device list is unequal to last poll
    if(polledMidiOutputDevices != outputs)
    {
        //update polled stored output device list
        polledMidiOutputDevices = outputs;

        //emit signal indicating a change in output devices
        emit signal_MidiOutputsChanged();
    }

    //qDebug() << "size of outputs array" << midiOutputs.size();
}

//---------------------------------------- Virtual Callbacks --------------------------------------//

void MidiManager::handlePartialSysexMessage(MidiInput *source, const uint8 *messageData, int numBytesSoFar, double timestamp)
{
    //qDebug() << "from handlePartialSysexMessage" << source->getName().toUTF8() << messageData << numBytesSoFar << timestamp;
}

void MidiManager::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message)
{
    //Emit midi message to whatever is connected to this signal
    emit signal_ReceiveMidiInput(QString(source->getName().toUTF8()), message);
}
