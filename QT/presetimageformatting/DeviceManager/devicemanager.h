// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "midicommon.h"
#include "standaloneimage.h"

class DeviceManager : public QObject
{
    Q_OBJECT

public:

    explicit DeviceManager(QObject *parent = 0);

    void sendMidiTriple(int status, int val1, int val2);
    void updateStandalone(StandaloneImage &standaloneImage);

//    void send_standalone_image(char *img);
    void appQuitting();

//    QTimer *fwQueryTimer;
//    int fwQueryCount;


//    const char *sigPadCorner(int padNum);



public slots:
    //void slotMidiPacket(MidiPacket *pkt);
    //void slotSysexRx(QByteArray &sysex);
    //void slotConnected();
    //void slotDisconnected();
    void slotDisplayStr(QString str);
    void slotStandalone(int state, int save);
    void slotTether(int state, int save);
//    void slotFwQuery();
//    void slotFwQueryTimeout();

    //-------- 2.0 Additions
    void slot_sendPresets(unsigned char *presetImage, int presetCount);
    void slot_sendSettings(unsigned char *settingsImage);


signals:
//    void sigPadCorner1(int corner,int val);
//    void sigPadCorner2(int corner,int val);
//    void sigPadCorner3(int corner,int val);
//    void sigPadCorner4(int corner,int val);
//    void sigPadCorner5(int corner,int val);
//    void sigPadCorner6(int corner,int val);
//    void sigPadCorner7(int corner,int val);
//    void sigPadCorner8(int corner,int val);
//    void sigPadCorner9(int corner,int val);
//    void sigPadCorner10(int corner,int val);

    //void sigMidiPacket(MidiPacket *pkt);
    void sigMidiTriple(int status, int val1, int val2);
    void sigSysex(unsigned char *sysex, int len);
    void sigFirmwareStatus(int buildNum, bool firmwareStatusIntact, QString version);
};

#endif // DEVICEMANAGER_H
