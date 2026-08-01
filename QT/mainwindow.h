// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QThread>
#include <QPointer>

// midi overhaul
#include "KMI_ports.h"
#include "KMI_mdm.h"
#include "RtMidi.h"
#include "KMI_DevData.h"
#include <kmiSysEx.h>

#include <pedalcal.h>
#include <cvCal.h>
#include <fwupdate.h>
#include <troubleshoot.h>

#include "KMI_updates.h"
#include "userdialog.h"

#include "presetinterface.h"
#include "globalpresetinterface.h"
#include "miditab.h"
#include "keytab.h"
#include "setlist.h"
#include "settings.h"
#include "copypastehandler.h"
#include "importexporthandler.h"
#include "tooltipeventfilter.h"
#include "sysexmanager.h"

#ifdef Q_OS_MAC
#include "ui_mainwindow.h"
#include "ui_saveAsForm.h"
#include "ui_deleteForm.h"
#include "ui_aboutForm.h"
#include "ui_importOldFoundForm.h"
#include "ui_importOldNotFoundForm.h"
#else
#include "ui_mainwindowWin.h"
#include "ui_saveAsFormWin.h"
#include "ui_deleteFormWin.h"
#include "ui_aboutFormWin.h"
#include "ui_importOldFoundFormWin.h"
#include "ui_importOldNotFoundFormWin.h"
#endif

#include "ui_pedalcal.h"
#include "ui_cvCal.h"


// uncomment this to force firmware updates in a loop
//#define DEBUG_FW_BRICKED

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QByteArray applicationVersion, thisFw;

    QString betaVersion;

    KMI_Updates *checkUpdates;

    // ------ midi overhaul --------------------------------------------------------

    // kmiPorts handles MIDI I/O changes
    KMI_Ports *kmiPorts;

    // create KMI devices
    MidiDeviceManager *TwelveStep;

    KMI_Decode *kmiDecode; // for decoding packets from the 12 Step
    KMI_Encode *kmiEncode; // for encoding...

    // MIDI aux inputs and outputs are defined here. For products like SoftStep, you would define 8 inputs for controllers
    // and one output for hosted mode. For other editors you would likely define one output port for to mirror the
    // incoming MIDI from the controller, as a workaround for Windows not sharing ports.
    // For KMI_Central we are using these for the input/output dropdowns as a simple MIDI route demo.
    MidiDeviceManager *midiTHRU;

    //QComboBox *midiThruDropdown;

    QString MIDI_AUX_KEY;
    bool    recallMidiAuxPort;
    QString recallMidiAuxPortName;

    // version strings for console and about window
    QString deviceBootloaderVersionString();
    QString deviceFirmwareVersionString();
    QString applicationFirmwareVersionString();

    QPointer<fwUpdate> fwUpdateWindow;
    QPointer<troubleshoot> troubleshootWindow;
    QPointer<pedalCal> pedalCalWindow;
    QPointer<cvCal> cvCalWindow;

    //----------------------------------- Stylesheets
    // General
    QFile*              generalStylesFile;
    QString             generalStylesString;

    // Dialog Styles
    QFile*              dialogStylesFile;
    QString             dialogStylesString;

    //Buttons
    QFile* blueStyleFile;
    QString blueStyleString;
    QFile* grayStyleFile;
    QString grayStyleString;

    // Tabs
    QString tabStyleString;

    // ------ 12 Step ------------------------------------------------------------

    QSettings *sessionSettings;

    PresetInterface *presetInterface;
    GlobalPresetInterface *globalPresetInterface;
    CopyPasteHandler *copyPasteHandler;
    ImportExportHandler *importExportHandler;
    ToolTipEventFilter toolTipEventFilter;

    // eb todo
    SysexManager *sysexManager;
    ImageFormatter imageFormatter;

    //Disabling Widget
    QWidget *disableWidget;

    //Dialogs
    QWidget *saveAsDialogWidget;
    QWidget *deleteDialogWidget;
    QWidget *aboutDialogWidget;
    QWidget *importOldDialogWidget;
    QWidget *importOldNotFoundDialogWidget;
    QWidget *pedalCalWidget;
    QWidget *cvCalWidget;

    //Menubar
    QMenuBar *menubar;
    QList<QAction *> actionList;

    // hardware menu
    QAction *openPedalCalibration;
    QAction *openCVCalibration;
    QAction *updateFirmwareAct;

    QAction *exportPreset;
    QAction *importPreset;
    QAction *importOldPreset;
    QAction *openAppDataDir;

    QAction *clearSetlist;
    QAction *autoPopulateSetlist;

    //copy / paste actions
    QAction *clearPresetAct;
    QAction *copyPresetAct;
    QAction *pastePresetAct;
    QAction *pasteNewPresetAct;
    QAction *toolTipsEnable;

    QAction *about;
    QAction *doc;
    QAction *update;
    QAction *troubleShoot;
    QAction *openLogLocation;

    //Ui Elements
    MidiTab *midiTab;
    KeyTab *keyTab;
    Setlist *setlistTab;
    Settings *settingsTab;

    QTabWidget *tabArea;
    QWidget *midiTabAreaWidget;
    QWidget *keyTabAreaWidget;
    QWidget *setlistTabAreaWidget;
    QWidget *settingsTabAreaWidget;

    //dynamic width and height variables
    int mainWindowHeight;
    int mainWindowWidth;
    bool forceFirmwareUpdate;

    //connection to device
    bool connected;
    bool firstRunUnsavedFlag;

    //is the current preset in the setlist and dirty?
    bool isInSetlistAndIsDirty;

    //presets currently sending -- to prevent quitting while presets are being sent to the board
    bool presetsSending;

    void closeEvent(QCloseEvent *);

signals:
    void signalStoreValue(QString name, QVariant value, int presetNum);
    void signalCheckSavedState();

    void signalSaveAs(QString presetName);

    void signalUpdateFw();
    void signalFwUpdateMessage(QString updateMessage);
    void signalClosePorts();

public slots:

    void windowHasLoaded();
    void slotOpenPresetDirectory();
    bool slotCheckPresets();
    void slotSetupStyleStringsAndFonts();

    // ------ midi overhaul --------------------------------------------------------
    void slotMIDIPortChange(QString, uchar, uchar, int); // handles changes to MIDI i/o
    void slotBootloaderMode(bool fwUpdateRequested);
    void relaunchApplication();

#ifdef DEBUG_FW_BRICKED
    void slotFirmwareDebugBricked();
    void slotFirmwareDebugBricked2();
    void slotFirmwareDebugBricked3();
#endif

    void slotFwUpdateSuccessCloseDialog(bool);
    void slotForceFirmwareUpdate();
    void slotFirmwareDetected(MidiDeviceManager *thisMDM, bool);
    void slotUpdateMIDIaux();
    void slotRecallMIDIaux();
    void slotEnableTether();
    void slotDisableTether();
    void slotProcessNRPN(uchar, int, int);

    // ------ end midi overhaul --------------------------------------------------------

    void slotConnectInterfaces();
    void slotConnectElements();
    void slotDisconnectElements();

    void slotValueChanged();
    void slotRecallPreset(QVariantMap, QVariantMap);
    void slotDisplaySaveState(bool);

    void slotSaveAs();

    void slotPopulatePresetMenu();
    void slotSetPresetMenu(int presetNum);

    void slotCleanUpSetlist();

    void slotShowGlobalDirtyStates();

    //menu bar
    void slotInitMenuBar();
    void slotUpdatePasteAvailability();
    void slotOpenDoc();
    void slotOpenLogDirectory();
    void slotEnableDisableToolTips();
    void slotAutoPopulateSetlist();

    //resizing functions for tabs
    void slotTabSizing(int);
    void slotMidiTabHeight(int);

    //midi ports & firmware updating
    void slotShowConnection(bool connection);
    void slotOpenTroubleshooting();

    //Preset & Settings Image Formatting, Sending
    void slotSendPresets();
    void slotSendSettings();

    void slotProcessKMIPacket(uint8_t PID, uint8_t category, uint8_t type, uint8_t* ptr, uint16_t length);

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private slots:

private:
    Ui::MainWindow *ui;

    //dialogs
    Ui::saveAsDialogForm        *saveAsDialogForm;
    Ui::deleteDialogForm        *deleteDialogForm;
    Ui::aboutDialogForm         *aboutDialogForm;
    Ui::importOldFoundDialog    *importOldFoundDialogForm;
    Ui::importOldNotFoundDialog *importOldNotFoundDialoglForm;
    Ui::pedalCal                *pedalCalForm;
    Ui::cvCal                   *cvCalForm;
};

#endif // MAINWINDOW_H
