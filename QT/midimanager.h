// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

//
//  MidiManager.h
//  qt_juce
//
//  Created by Conner Lacy on 4/15/14.
//  Copyright 2014. All rights reserved.
//

#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H

#include <QObject>
#include <QDebug>
#include <QTimer>

class MidiManager : public QObject,
                    //public MidiInputCallback
{
    Q_OBJECT
public:
    explicit MidiManager(QObject *parent = 0);
    ~MidiManager();

    //returns list of input/output device names
    QStringList getInputDevices();
    QStringList getOutputDevices();

    //opens an input/output for io
    bool openInputDevice(QString deviceName);
    bool openOutputDevice(QString deviceName);

    //closes an input/output if no longer needed
    bool closeInputDevice(QString deviceName);
    bool closeOutputDevice(QString deviceName);

    //output midi (more type functionality can be included, but for now I think this is all we need)
    void outputSysEx(QString deviceName, unsigned char *data, int size);
    void outputNoteOn(QString deviceName, int channel, int noteNum, int noteVelocity);
    void outputNoteOff(QString deviceName, int channel, int noteNum, int noteVelocity);
    void outputCC(QString deviceName, int channel, int controllerNum, int controllerValue);
    void outputProgramChange(QString deviceName, int channel, int programNum);

    void handlePartialSysexMessage(MidiInput *source, const unsigned int *messageData, int numBytesSoFar, double timestamp);
    void handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message);

private:
    QTimer devicePoller;
    QStringList polledMidiInputDevices;
    QStringList polledMidiOutputDevices;

    OwnedArray<MidiInput> midiInputs;
    QMap<QString, int> midiInputArrayLookup;
    OwnedArray<MidiOutput> midiOutputs;
    QMap<QString, int> midiOutputArrayLookup;

signals:
    void signal_MidiInputsChanged();
    void signal_MidiOutputsChanged();
    void signal_ReceiveMidiInput(QString sourceName, const MidiMessage &message);

    void signalResendFirmware(); //this is a 12Step specific signal that I made to make sure firmware gets updated for 12Step on Windows

public slots:

private slots:
    void slot_PollDevices();

};

#endif // MIDIMANAGER_H
