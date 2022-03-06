// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MIDIDEVICEMANAGER_H
#define MIDIDEVICEMANAGER_H

#include <QDebug>
#include <QWidget>
#include <QtWidgets> // was QT Gui
#include <QApplication>
//#include <CoreMIDI/CoreMIDI.h>
//#include <CoreServices/CoreServices.h>
//#include <CoreFoundation/CoreFoundation.h>
//#include <AudioUnit/AudioUnit.h>
#include <vector>

#include "midiio/sysexencode.h"
#include "midiio/sysexdecode.h"

//enum{NORMAL, BOOTLOADER_POST_UPDATE_REQUEST, BOOTLOADER_NO_UPDATE_REQUEST};

class 12s_MidiDeviceManager : public QWidget
{
    Q_OBJECT
public:
    explicit 12s_MidiDeviceManager(QWidget *parent = 0);

    SysExEncode*        sysExEncode;
    SysExDecode*        sysExDecode;

    //Application MIDI Variables so out App can rx/tx MIDI
    MIDIClientRef appClientRef;
    MIDIPortRef appInPortRef;
    MIDIPortRef appOutPortRef;

    char bootloaderVersion[3];
    char firmwareVersion[3];
    int  versionSum;

    QFile *firmware;
    QByteArray firmwareByteArray;

    QString expectedBootloaderVersion;
    QString expectedFirmwareVersion;
    QString foundBootloaderVersion;
    QString foundFirmwwareVersion;

    //Sets up MIDI rx/tx using above vars
    void createAppMidiClient();

    //Helper variables to process sysex
    QByteArray sysExMessage; //Message to be processed;
    bool isSysEx;

    //Return MIDI source/dest name,index maps
    QMap<QString, int> getMidiDestinations();
    QMap<QString, int> getMidiSources();

    //Connects a source to our app
    bool connectSource(QString sourceName);

    //Check mode of connected QuNexus
    void queryQuNexus();

    //Describes whether or not a fw update has been requested-- useful for managing bootloader reconnects
    bool fwUpdateRequested;
    bool inBootloader;

    //------------- Helper Functions -------------//
    //Formats MIDI Device Names
    QString getDisplayName(MIDIObjectRef); //gets "name" of QuNeo device
    QString cFStringRefToQString(CFStringRef);

    QByteArray globals;
    
signals:
    void signalFirmwareOutOfDate(QString expectedBoot, QString foundBoot, QString expectedFirmware, QString foundFirmware);
    void signalProgressDialog(QString messageType, int val);
    void signalFirmwareUpdateComplete();
    void signalQuNexusConnected(bool);
    void signalSendGlobals(QString,QByteArray);
    void signalEncodeGlobals(QString, QList<int>);
    void signalRestoreGlobals();
    
public slots:
    void slotRequestFirmwareUpdate();
    void slotUpdateFirmware();
    void slotEnterBootloader();
    void slotSendSysEx(QString messageID, QByteArray sysExMessageByteArray, QString destinationName);
    void slotProcessSysEx(QByteArray sysExMessageByteArray);
    void slotRestoreGlobals(QByteArray globalsByteArray);

    int slotFindQuNexusPort1Source();
    int slotFindQuNexusPort1Dest();
    
};

#endif // 12s_MidiDeviceManager_H
