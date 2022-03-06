// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SYSEXMANAGER_H
#define SYSEXMANAGER_H

#include <QObject>
#include <QtGui>
#include <QApplication>

#include "definitions.h"
//#include "midimanager.h"

class SysexManager : public QObject
{
    Q_OBJECT
public:
#ifdef Q_OS_MAC
    explicit SysexManager(QObject *parent = 0);
#else
    explicit SysexManager();
#endif

    QString deviceName;
    QString sysExPath;

    bool connected;
    bool fwQuerySent;
    bool fwQueryMatched;
    bool fwUpdateStarted;
    bool fwUpdated;
    bool fwUpdateCancelled;

    QTimer *fwQueryTimeout;

signals:

    void signalConnected(bool connected);
    void signalOpenFwDialog();
    void signalFwUpdateStarted();
    void signalFwUpdateFinished();
    void signalFwVersion(QString fwVersionNum);

    void signalStartTimer(int ms);
    void signalStopTimer();

public slots:

#ifndef Q_OS_MAC
    void process();
#endif

    void slotStartTimer(int ms);
    void slotStopTimer();

    //midi port stuff
    void slotSetUpMidiPorts();
    void slotMidiOutputsChanged();
    void slotMidiInputsChanged();
    void slotClosePorts(); // on quit

    //fw checking/updating
    void slotSendFwQuery();
    void slotQueryTimeout();
    //void slotReceiveFwQuery(QString sourceName, const MidiMessage &message);
    void slotReceiveFwQuery(QString sourceName);
    void slotCancelFwUpdate();
    void slotUpdateFw();
    void slotResetVariables();

    void slotSendSettingsOrPresetImage(char *presetOrSettingsImage, int length);

private:
    //MidiManager *midiManager;

};

#endif // SYSEXMANAGER_H
