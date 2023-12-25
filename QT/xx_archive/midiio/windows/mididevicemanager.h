// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MIDIDEVICEMANAGER_H
#define MIDIDEVICEMANAGER_H

#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QtWidgets> // was QT Gui
#include <QTimer>
#include <Windows.h>
#include <MMSystem.h>
#include <Dbt.h>

#include "sysexencode.h"
#include "sysexdecode.h"

class MidiDeviceManager : public QWidget
{
    Q_OBJECT
public:
    explicit MidiDeviceManager(QWidget *parent = 0);

    SysExEncode*        sysExEncode;
    SysExDecode*        sysExDecode;

    //----------------------------------------- Windows MIDI & Windowing Services -----------------------------------------//
    MIDIOUTCAPS     mocs;
    HMIDIOUT        outHandle;
    HANDLE          sysExOutBuffer;
    MIDIHDR         sysExOutHdr;
    MIDIINCAPS      mics;
    HMIDIIN         inHandle;
    HANDLE          sysExInBuffer;
    MIDIHDR         sysExInHdr;
    HANDLE          hBuffer;


    static void CALLBACK midiInCallback(HMIDIIN hMidiIn,UINT wMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2);
    static void CALLBACK midiOutCallback(HMIDIOUT handle, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam, DWORD_PTR dwParam1);
    bool winEvent(MSG *message, long *result);

    bool connected;
    bool refreshDevices;

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

    //Check mode of connected QuNexus
    void queryQuNexus();

    //Connects a source to our app
    bool connectSource();

    //Helper variables to process sysex
    QByteArray sysExMessage; //Message to be processed;
    bool isSysEx;

    //Return MIDI source/dest name,index maps
    QMap<QString, int> getMidiDestinations();
    QMap<QString, int> getMidiSources();

    //Describes whether or not a fw update has been requested-- useful for managing bootloader reconnects
    bool fwUpdateRequested;
    bool inBootloader;

    QByteArray globals;
    QString    sysExType;
    bool       globalsRecieved;

    QTimer* globalPoller;

signals:
    void signalFirmwareOutOfDate(QString expectedBoot, QString foundBoot, QString expectedFirmware, QString foundFirmware);
    void signalProgressDialog(QString messageType, int val);
    void signalFirmwareUpdateComplete();
    void signalQuNexusConnected(bool);

    void signalSendGlobals(QString, QByteArray);
    void signalEncodeGlobals(QString, QList<int>);void signalRestoreGlobals();

public slots:
    //Connects a source to our app
    bool slotConnectDisconnectDevice();
    void slotRefreshDevices();

    void slotRequestFirmwareUpdate();
    void slotUpdateFirmware();
    void slotEnterBootloader();
    void slotSendSysEx(QString messageID, QByteArray sysExMessageByteArray, QString destinationName);
    void slotProcessSysEx(QByteArray sysExMessageByteArray);

    QMap<QString, int> slotGetMidiInDevices();
    QMap<QString, int> slotGetMidiOutDevices();

    void slotOpenMidiIn(int index);
    void slotOpenMidiOut(int index);
    void slotCloseMidiIn();
    void slotCloseMidiOut();

    void slotQueryQuNexus();
    void slotRestoreGlobals(QByteArray globalsByteArray);
    void slotSendGlobalsRequest();
    void slotPollGlobals();
};

#endif // MIDIDEVICEMANAGER_H
