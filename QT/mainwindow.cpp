// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "mainwindow.h"
#include <QWidget>
//#include <sysexmanager.h>
#include "KMI_FwVersions.h"
#include "inc/KMI_Updates/kmi_updates.h"

//#include "./jucesupport/JuceLibraryCode/JuceHeader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),

    saveAsDialogForm(new Ui::saveAsDialogForm),
    saveAsDialogWidget(new QWidget(this)),
    deleteDialogForm(new Ui::deleteDialogForm),
    deleteDialogWidget(new QWidget(this)),
    aboutDialogForm(new Ui::aboutDialogForm),
    aboutDialogWidget(new QWidget(this)),
    disableWidget(new QWidget(this)),
    importOldFoundDialogForm(new Ui::importOldFoundDialog),
    importOldDialogWidget(new QWidget(this)),
    importOldNotFoundDialoglForm(new Ui::importOldNotFoundDialog),
    importOldNotFoundDialogWidget(new QWidget(this)),
//    fwoodDialogForm(new Ui::FwoodDialog),
//    fwoodDialogWidget(new QWidget(this)),
//    fwUpdateCompleteDialogForm(new Ui::FwUpdateCompleteForm),
//    fwUpdateCompleteDialogWidget(new QWidget(this)),
//    fwProgressDialogForm(new Ui::FwProgressForm),
//    fwProgressDialogWidget(new QWidget(this)),

    presetInterface(new PresetInterface(this)),
    globalPresetInterface(new GlobalPresetInterface(this)),
    copyPasteHandler(new CopyPasteHandler(presetInterface, this)),
    importExportHandler(new ImportExportHandler(presetInterface, this))
//#ifdef Q_OS_MAC
//    sysexManager(new SysexManager(this))
//#else
//    sysexManager(new SysexManager())
//#endif
{
    //plist stuff
    QCoreApplication::setApplicationName("12 Step Editor");
    QCoreApplication::setOrganizationName("KeithMcMillenInstruments");
    QCoreApplication::setOrganizationDomain("keithmcmillen.com");


    qDebug() << "System Locale: " << QLocale::system().name();

    // application version
    applicationVersion.resize(3);

    applicationVersion[0] = 2;
    applicationVersion[1] = 1;
    applicationVersion[2] = 0;

    betaVersion = ""; // leave blank for release


    thisFw = QByteArray(reinterpret_cast<char*>(_fw_ver_12step), sizeof(_fw_ver_12step));


    // flag that the app has just started, avoid unsaved popup on open
    firstRunUnsavedFlag = true;


    // ******************************
    // KMI_Ports
    // ******************************

    // kmiPorts reports changes in MIDI i/o
    kmiPorts = new KMI_Ports(this);


    // start polling at 100ms intervals
    kmiPorts->devicePoller->start(100);

    // connect kmiPorts to our handler
    connect(kmiPorts, SIGNAL(signalPortUpdated(QString, uchar, uchar, int)), this, SLOT(slotMIDIPortChange(QString, uchar, uchar, int)));

    //qDebug() << "end connect";

    // ******************************
    // create KMI device handlers
    // ******************************

    TwelveStep = new MidiDeviceManager(this, PID_12STEP, "12Step");

    // setup firmware image
    QString thisFwFile = QString(":/resources/firmware/12Step_Firmware_v%1.%2.%3.syx")
            .arg(uchar(thisFw.at(0)))
            .arg(uchar(thisFw.at(1)))
            .arg(uchar(thisFw.at(2)));

    //qDebug() << "fwFilename: " << thisFwFile;

    if (!TwelveStep->slotOpenFirmwareFile(thisFwFile))
    {
        qDebug() << "ERROR - firmware file not found: " << thisFwFile;
    }

    // connect firmware signals
    qDebug() << "connect signalFirmwareDetected";

    // setup MIDI aux output
    midiAuxOut = new MidiDeviceManager(this, PID_AUX, "MIDI Thru");

    // ******************************
    // end KMI_Ports and device handlers
    // ******************************

    connected = false;

    sessionSettings = new QSettings(this); //session settings allow us to enable/disable tooltips -- see slotInitMenuBar

    // ******************************
    // check for updates and set default save locations
    // ******************************
    QString jsonVersionCheckURL = "https://files.keithmcmillen.com/products/12step/editor/softwareVersionCheck.json";
    checkUpdates = new KMI_Updates(this, "12step", sessionSettings, applicationVersion, jsonVersionCheckURL);

    // default file location
    const QString DEFAULT_DIR_KEY("default_dir");

    qDebug() << "Default file save location - pre: " << sessionSettings->value(DEFAULT_DIR_KEY).toString();

    // test if this is a directory
    QFileInfo check_file(sessionSettings->value(DEFAULT_DIR_KEY).toString());
    if (!check_file.exists() || !check_file.isDir() || sessionSettings->value(DEFAULT_DIR_KEY).toString().contains("Contents/MacOS"))
    {
        QString desktop = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
        qDebug() << "Desktop: " << desktop;
        sessionSettings->setValue(DEFAULT_DIR_KEY, desktop);     // if key doesn't exist, set it to desktop
    }

    qDebug() << "Default file save location - post: " << sessionSettings->value(DEFAULT_DIR_KEY).toString();

    // ******************************

    // ---- 12 Step
    mainWindowHeight = KEYTAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING;
    mainWindowWidth = KEYTAB_WIDTH + TAB_X_POS*2;

    ui->setupUi(this);

    // MIDI Overhaul and FW Update

    // Firmware update Window
    fwUpdateWindow = new fwUpdate(this, "12 Step", applicationFirmwareVersionString());
    //fwUpdateWindow->setStyleSheet(generalStylesString);

    // end MIDI Overhaul

    this->setWindowTitle("12 Step Editor");
    this->setGeometry(400, 35, mainWindowWidth, mainWindowHeight);
    this->setFixedSize(mainWindowWidth, mainWindowHeight);

    // ---- FONTS --------------------------
    qDebug() << "------------ [FONTS SETUP] ---------------------------------------------------";

    QString droidFont = ":/fonts/droid-sans/DroidSansMono.ttf";
    QString futuraFont = ":/fonts/futura/futura-normal.ttf";
    QString futuraBFont = ":/fonts/futura/Futura-Bold.ttf";
    QString corbelFont = ":/fonts/corbel/corbel.ttf";
    QString corbelBFont = ":/fonts/corbel/corbelb.ttf";

    if (QFontDatabase::addApplicationFont(droidFont) == -1) qDebug() << "Could not load font: " << droidFont;
    if (QFontDatabase::addApplicationFont(futuraFont) == -1) qDebug() << "Could not load font: " << futuraFont;
    if (QFontDatabase::addApplicationFont(futuraBFont) == -1) qDebug() << "Could not load font: " << futuraBFont;
    if (QFontDatabase::addApplicationFont(corbelFont) == -1) qDebug() << "Could not load font: " << corbelFont;
    if (QFontDatabase::addApplicationFont(corbelBFont) == -1) qDebug() << "Could not load font: " << corbelBFont;

    // ---- end FONTS -------------------------

    //-------------------- Disable Widget
    disableWidget->hide();
    disableWidget->setGeometry(0,0,mainWindowWidth, mainWindowHeight);
    disableWidget->setStyleSheet("background: rgba(0,0,0,200);");

    //construct the tab widget and add the tabs to them
    tabArea = new QTabWidget(this);
    tabArea->setGeometry(TAB_X_POS, TAB_Y_POS, KEYTAB_WIDTH, KEYTAB_HEIGHT);
    tabArea->setObjectName("tabArea");

    //set main stylesheet for all tabs here
#ifdef Q_OS_MAC
    tabArea->setStyleSheet("QTabWidget {}"
                           "QTabWidget::pane {border: 10px solid rgb(89, 89, 89);}"
                           "QTabWidget::tab-bar {left: 20px;}"
                           "QTabBar::tab {margin-left: 5; margin-right: 5; min-width: 100px; background-color: rgb(40, 40, 40); font: 14pt 'Futura PT'; color: rgb(242, 242, 242); border-left: 3px solid rgb(89, 89, 89); border-right: 3px solid rgb(89, 89, 89); border-top: 3px solid rgb(89, 89, 89); padding: 4px;}"
                           "QTabBar::tab:selected {background-color: rgb(89, 89, 89);}"
                           );
#else
    tabArea->setStyleSheet("QTabWidget {}"
                           "QTabWidget::pane {border: 10px solid rgb(89, 89, 89);}"
                           "QTabWidget::tab-bar {left: 20px;}"
                           "QTabBar::tab {margin-left: 5; margin-right: 5; min-width: 100px; background-color: rgb(40, 40, 40); font: 10pt 'Futura-Normal'; color: rgb(242, 242, 242); border-left: 3px solid rgb(89, 89, 89); border-right: 3px solid rgb(89, 89, 89); border-top: 3px solid rgb(89, 89, 89); padding: 4px;}"
                           "QTabBar::tab:selected {background-color: rgb(89, 89, 89);}"
                           );
#endif

    // ------------ TABS ------------------------------------------------
    keyTabAreaWidget = new QWidget(tabArea);
    tabArea->addTab(keyTabAreaWidget, QString("Note Entry"));  //move this when the keyboard tab is added

    keyTab = new KeyTab(keyTabAreaWidget);

    midiTabAreaWidget = new QWidget(tabArea);
    tabArea->addTab(midiTabAreaWidget, QString("MIDI Parameters"));

    midiTab = new MidiTab(midiTabAreaWidget);
    midiTab->slotConnectElements();

    setlistTabAreaWidget = new QWidget(tabArea);
    tabArea->addTab(setlistTabAreaWidget, QString("Setlist"));

    setlistTab = new Setlist(setlistTabAreaWidget);

    settingsTabAreaWidget = new QWidget(tabArea);
    tabArea->addTab(settingsTabAreaWidget, QString("Settings"));

    settingsTab = new Settings(settingsTabAreaWidget);

    tabArea->setTabToolTip(0,"Set up Notes here.");
    tabArea->setTabToolTip(1,"Manage the MIDI output here.");
    tabArea->setTabToolTip(2,"Changes in the setlist tab will not be sent to the 12 Step until the “Send Setlist to 12 Step” button is clicked.");
    tabArea->setTabToolTip(3,"Changes in the settings tab will not be sent to the 12 Step until the “Send Setlist to 12 Step” button is clicked.");
    
    // ------------ END TABS ------------------------------------------------

    //set up the sysex manager - on windows sysexManager must be in a separate thread
//#ifndef Q_OS_MAC
//    QThread *thread = new QThread;
//    sysexManager->moveToThread(thread);
//    connect(thread, SIGNAL(started()), sysexManager, SLOT(process()));
//    thread->start();
//#endif

    // MIDI thru dropdown
    // connect dropdowns and connection status to MIDI aux ports
    midiTab->
    connect(midiTab, SIGNAL(signalUpdateMIDIaux()), this, SLOT(slotUpdateMIDIaux()));
    connect(TwelveStep, SIGNAL(signalConnected(bool)), this, SLOT(slotUpdateMIDIaux()));

    // remember last selected MIDI aux port
    MIDI_AUX_KEY = "midi_aux_port";

    qDebug() << "connected aux port";

    //-------------------- Dialogs

    //save as
    saveAsDialogWidget->hide();
    saveAsDialogWidget->setGeometry(mainWindowWidth/2 - SAVEASWIDGET_WIDTH/2, mainWindowHeight/2 - SAVEASWDIGET_HEIGHT/2, SAVEASWIDGET_WIDTH, SAVEASWDIGET_HEIGHT);
    saveAsDialogForm->setupUi(saveAsDialogWidget);
    saveAsDialogForm->save->setEnabled(false);

    //delete
    deleteDialogWidget->hide();
    deleteDialogWidget->setGeometry(mainWindowWidth/2 - DELETEWIDGET_WIDTH/2, mainWindowHeight/2 - DELETEWIDGET_HEIGHT/2, DELETEWIDGET_WIDTH, DELETEWIDGET_HEIGHT);
    deleteDialogForm->setupUi(deleteDialogWidget);
    deleteDialogForm->delete_2->setDefault(true);

    //about
    aboutDialogWidget->hide();
    aboutDialogWidget->setGeometry(mainWindowWidth/2 - ABOUTWIDGET_WIDTH/2, mainWindowHeight/2 - ABOUTWIDGET_HEIGHT/2, ABOUTWIDGET_WIDTH, ABOUTWIDGET_HEIGHT);
    aboutDialogForm->setupUi(aboutDialogWidget);

    QString appVersionString = QString("12 Step Editor v%1.%2.%3")
                                              .arg(uchar(applicationVersion.at(0)))
                                              .arg(uchar(applicationVersion.at(1)))
                                              .arg(uchar(applicationVersion.at(2)));
    if (betaVersion != "")
    {
        appVersionString.append(betaVersion);
    }

    aboutDialogForm->application->setText(appVersionString);
    aboutDialogForm->expected->setText(applicationFirmwareVersionString());
    aboutDialogForm->found->setText("Not Connected");
    aboutDialogForm->ok->setDefault(true);

    //import old
    importOldDialogWidget->hide();
    importOldFoundDialogForm->setupUi(importOldDialogWidget);
    importOldDialogWidget->setGeometry(mainWindowWidth/2 - importOldDialogWidget->width()/2, mainWindowHeight/2 - importOldDialogWidget->height()/2, importOldDialogWidget->width(), importOldDialogWidget->height());
    importOldNotFoundDialogWidget->hide();
    importOldNotFoundDialoglForm->setupUi(importOldNotFoundDialogWidget);
    importOldNotFoundDialogWidget->setGeometry(mainWindowWidth/2 - importOldNotFoundDialogWidget->width()/2, mainWindowHeight/2 - importOldNotFoundDialogWidget->height()/2, importOldNotFoundDialogWidget->width(), importOldNotFoundDialogWidget->height());
    importOldNotFoundDialoglForm->ok->setDefault(true);
    //fw dialogs
//    fwoodDialogWidget->hide();
//    fwoodDialogForm->setupUi(fwoodDialogWidget);
//    fwoodDialogWidget->setGeometry(mainWindowWidth/2 - fwoodDialogWidget->width()/2, mainWindowHeight/2 - fwoodDialogWidget->height()/2, fwoodDialogWidget->width(), fwoodDialogWidget->height());
//    fwoodDialogForm->expected->setText(FW_VERSION);
//    fwoodDialogForm->update->setDefault(true);
//    fwUpdateCompleteDialogWidget->hide();
//    fwUpdateCompleteDialogForm->setupUi(fwUpdateCompleteDialogWidget);
//    fwUpdateCompleteDialogWidget->setGeometry(mainWindowWidth/2 - fwUpdateCompleteDialogWidget->width()/2, mainWindowHeight/2 - fwUpdateCompleteDialogWidget->height()/2, fwUpdateCompleteDialogWidget->width(), fwUpdateCompleteDialogWidget->height());
//    fwUpdateCompleteDialogForm->ok->setDefault(true);
//    fwProgressDialogWidget->height();
//    fwProgressDialogForm->setupUi(fwProgressDialogWidget);
//    fwProgressDialogWidget->setGeometry(mainWindowWidth/2 - fwProgressDialogWidget->width()/2, mainWindowHeight/2 - fwProgressDialogWidget->height()/2, fwProgressDialogWidget->width(), fwProgressDialogWidget->height());
//    fwProgressDialogForm->progressBar->setMaximum(0);

    qDebug() << "**** Init Menu Bar ****";
    slotInitMenuBar();

    qDebug() << "**** Connect Interfaces ****";
    slotConnectInterfaces();

    qDebug() << "**** Set Preset Menu ****";
    slotSetPresetMenu(0);

    //disable all context menus
    foreach(QWidget *widget, this->findChildren<QWidget *>())
    {
        widget->setContextMenuPolicy(Qt::NoContextMenu);
#ifdef Q_OS_MAC
        widget->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
        //tool tip on/off switch in help menu
        widget->installEventFilter(&toolTipEventFilter);
    }

    ui->centralWidget->installEventFilter(this);
    tabArea->installEventFilter(this);

//    //load fonts
//    QString droidFont;
//    QString futuraFont;
//    QString futuraBFont;
//    QString corbelFont;
//    QString corbelBFont;
//    QString fontPath = QCoreApplication::applicationDirPath();

//#if defined(Q_OS_MAC)
//    fontPath.remove(fontPath.length() - 5, fontPath.length());
//    droidFont = QString("%1Resources/DroidSansMono.ttf").arg(fontPath);
//    futuraFont = QString("%1Resources/FuturaPT-Book.otf").arg(fontPath);

//    QFontDatabase::addApplicationFont(droidFont);
//    QFontDatabase::addApplicationFont(futuraFont);

//#elif !defined(Q_OS_MAC)
//    droidFont = "./resources/DroidSansMono.ttf";
//    futuraFont = "./resources/futura-normal.ttf";
//    futuraBFont = "./resources/Futura-Bold.ttf";
//    corbelFont = "./resources/corbel.ttf";
//    corbelBFont = "./resources/corbelb.ttf";

//    QFontDatabase::addApplicationFont(droidFont);
//    QFontDatabase::addApplicationFont(futuraFont);
//    QFontDatabase::addApplicationFont(futuraBFont);
//    QFontDatabase::addApplicationFont(corbelFont);
//    QFontDatabase::addApplicationFont(corbelBFont);

//#else
//    droidFont = "./resources/DroidSansMono.ttf";
//    futuraFont = "./resources/FuturaPT-Book.otf";

//    QFontDatabase::addApplicationFont(droidFont);
//    QFontDatabase::addApplicationFont(futuraFont);
//#endif


    //the following was commented out because of JUCE assertion errors -- I may need to put it back in somewhere else
    //double check midi outputs - connection indicator wouldn't always initiate properly before this double check was placed here
    //slotShowConnection(false);
    //sysexManager->slotMidiOutputsChanged();

    //set state of preset stuff
    presetsSending = false;
    presetInterface->slotUpdateJSONPath();
    presetInterface->slotReadJSON();
    presetInterface->slotPopulatePresetMenu(ui->presetmenu);
    presetInterface->slotRecallPreset(0);
    globalPresetInterface->slotRecallSettings();
    setlistTab->slotRecallSetlist();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if(presetsSending)
    {
        //make a delay for quitting if the presets are currently sending -- prevents connection problems on windows -- allowing it for mac just in case
        QTime quitDelayTime = QTime::currentTime().addSecs(4);
        while(QTime::currentTime() < quitDelayTime)
        {
            QCoreApplication::processEvents();
        }
    }
    emit signalClosePorts();
    qDebug() << "closing...";
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    //this function allows you to click outside of a widget to make it lose focus
    if(e->type() == QEvent::MouseButtonPress)
    {
        foreach (QWidget *widget, this->findChildren<QWidget *>())
        {
            if(widget->hasFocus())
            {
                widget->clearFocus();
            }
        }
    }
    return QObject::eventFilter(obj,e);
}

void MainWindow::slotConnectInterfaces()
{
    //connect the preset interface to the preset menu
    connect(presetInterface, SIGNAL(signalPresetMenu(int)), this, SLOT(slotSetPresetMenu(int)));

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////// Preset Storage, Recall //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //--------------------------------------- Preset Recall

    //MainWindow -- display name
    connect(presetInterface, SIGNAL(signalRecallPreset(QVariantMap,QVariantMap)), this, SLOT(slotRecallPreset(QVariantMap,QVariantMap)));

    //---------- KeyTab
    connect(presetInterface, SIGNAL(signalRecallPreset(QVariantMap,QVariantMap)), keyTab, SLOT(slotRecallPreset(QVariantMap,QVariantMap)));
    //12Step Key representations
    for(int k = 0; k < 13; k++)
    {
        connect(presetInterface, SIGNAL(signalRecallPreset(QVariantMap,QVariantMap)), keyTab->keyEdit[k], SLOT(slotRecallPreset(QVariantMap,QVariantMap)));
    }

    //---------- MidiTab
    connect(presetInterface, SIGNAL(signalRecallPreset(QVariantMap,QVariantMap)), midiTab, SLOT(slotRecallPreset(QVariantMap,QVariantMap)));
    //Modlines
    for(int m = NUMBER_OF_MODLINES-1; m >= 0; m--)  //I'm recalling these in backwards order here so that the modline 1 source b selection can control the other modlines' source bs
    {
        connect(presetInterface, SIGNAL(signalRecallPreset(QVariantMap,QVariantMap)), midiTab->modline[m], SLOT(slotRecallPreset(QVariantMap,QVariantMap)));
    }

    //---------- Settings
    connect(globalPresetInterface, SIGNAL(signalRecallSettings(QVariantMap,QVariantMap)), settingsTab, SLOT(slotRecallPreset(QVariantMap,QVariantMap)));


    //--------------------------------------- Parameter Storage

    //MainWindow -- display name
    connect(this, SIGNAL(signalStoreValue(QString,QVariant,int)), presetInterface, SLOT(slotStoreValue(QString,QVariant,int)));
    connect(this, SIGNAL(signalCheckSavedState()), presetInterface, SLOT(slotCheckSaveState()));

    //---------- KeyTab
    //12Step Key representations
    for(int k = 0; k < 13; k++)
    {
        connect(keyTab->keyEdit[k], SIGNAL(signalStoreValue(QString,QVariant,int)), presetInterface, SLOT(slotStoreValue(QString,QVariant,int)));
        //save state
        connect(keyTab->keyEdit[k], SIGNAL(signalCheckSavedState()), presetInterface, SLOT(slotCheckSaveState()));
    }

    //---------- MidiTab
    connect(midiTab, SIGNAL(signalStoreValue(QString,QVariant,int)), presetInterface, SLOT(slotStoreValue(QString,QVariant,int)));
    connect(midiTab, SIGNAL(signalCheckSavedState()), presetInterface, SLOT(slotCheckSaveState()));
    //Modlines
    for(int m = 0; m < NUMBER_OF_MODLINES; m++)
    {
        connect(midiTab->modline[m], SIGNAL(signalStoreValue(QString,QVariant,int)), presetInterface, SLOT(slotStoreValue(QString,QVariant,int)));
        //save state
        connect(midiTab->modline[m], SIGNAL(signalCheckSavedState()), presetInterface, SLOT(slotCheckSaveState()));
    }

    //---------- Settings
    connect(settingsTab, SIGNAL(signalStoreValue(QString,QVariant)), globalPresetInterface, SLOT(slotStoreSettings(QString,QVariant)));
    connect(settingsTab, SIGNAL(signalWriteSettings()), globalPresetInterface, SLOT(slotWriteSettings()));

    connect(settingsTab, SIGNAL(signalWriteSettings()), this, SLOT(slotSendSettings()));

    //--------------------------------------- Save, Save As, Revert, Delete
    //save button
    connect(ui->save, SIGNAL(clicked()), presetInterface, SLOT(slotSavePreset()));
    connect(ui->revert, SIGNAL(clicked()), presetInterface, SLOT(slotRevertPreset()));

    //save indicator
    connect(presetInterface, SIGNAL(signalPresetDirty(bool)), this, SLOT(slotDisplaySaveState(bool)));

    //send indicator for global states

    //EB TODO: commented these two out for debug
    //connect(setlistTab, SIGNAL(signalSetlistDirty()), this, SLOT(slotShowGlobalDirtyStates()));
    //connect(settingsTab, SIGNAL(signalSettingsDirty()), this, SLOT(slotShowGlobalDirtyStates()));

    //save as
    connect(ui->saveas, SIGNAL(clicked()), disableWidget, SLOT(raise()));
    connect(ui->saveas, SIGNAL(clicked()), disableWidget, SLOT(show()));
    connect(ui->saveas, SIGNAL(clicked()), saveAsDialogWidget, SLOT(raise()));
    connect(ui->saveas, SIGNAL(clicked()), saveAsDialogWidget, SLOT(show()));
    connect(ui->saveas, SIGNAL(clicked()), saveAsDialogForm->name, SLOT(setFocus()));
    connect(saveAsDialogForm->cancel, SIGNAL(clicked()), saveAsDialogWidget, SLOT(close()));
    connect(saveAsDialogForm->cancel, SIGNAL(clicked()), disableWidget, SLOT(close()));
    connect(saveAsDialogForm->name, SIGNAL(textChanged(QString)), this, SLOT(slotValueChanged()));
    connect(saveAsDialogForm->save, SIGNAL(clicked()), this, SLOT(slotSaveAs()));
    connect(this, SIGNAL(signalSaveAs(QString)), presetInterface, SLOT(slotSavePresetAs(QString)));
    connect(presetInterface, SIGNAL(signalAddOrRemovePreset()), this, SLOT(slotPopulatePresetMenu()));
    connect(copyPasteHandler, SIGNAL(signalAddOrRemovePreset()), this, SLOT(slotPopulatePresetMenu()));
    connect(importExportHandler, SIGNAL(signalAddOrRemovePreset()), this, SLOT(slotPopulatePresetMenu()));
    connect(presetInterface, SIGNAL(signalPresetMenu(int)), this, SLOT(slotSetPresetMenu(int)));
    connect(copyPasteHandler, SIGNAL(signalPresetMenu(int)), this, SLOT(slotSetPresetMenu(int)));
    connect(importExportHandler, SIGNAL(signalPresetMenu(int)), this, SLOT(slotSetPresetMenu(int)));

    //delete
    connect(ui->deletepreset, SIGNAL(clicked()), disableWidget, SLOT(raise()));
    connect(ui->deletepreset, SIGNAL(clicked()), disableWidget, SLOT(show()));
    connect(ui->deletepreset, SIGNAL(clicked()), deleteDialogWidget, SLOT(raise()));
    connect(ui->deletepreset, SIGNAL(clicked()), deleteDialogWidget, SLOT(show()));
    connect(ui->deletepreset, SIGNAL(clicked()), deleteDialogForm->delete_2, SLOT(setFocus()));
    connect(deleteDialogForm->cancel, SIGNAL(clicked()), deleteDialogWidget, SLOT(close()));
    connect(deleteDialogForm->cancel, SIGNAL(clicked()), disableWidget, SLOT(close()));
    connect(deleteDialogForm->delete_2, SIGNAL(clicked()), presetInterface, SLOT(slotDeletePreset()));
    connect(deleteDialogForm->delete_2, SIGNAL(clicked()), this, SLOT(slotPopulatePresetMenu()));
    connect(deleteDialogForm->delete_2, SIGNAL(clicked()), deleteDialogWidget, SLOT(close()));
    connect(deleteDialogForm->delete_2, SIGNAL(clicked()), disableWidget, SLOT(close()));

    //--------------------------------------- Setlist
    connect(presetInterface, SIGNAL(signalPopulateSetlistMenus(QComboBox*)), setlistTab, SLOT(slotPopulateSetlistMenus(QComboBox*)));

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////// Menu Items /////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //About Screen
    connect(aboutDialogForm->ok, SIGNAL(clicked()), aboutDialogWidget, SLOT(close()));
    connect(aboutDialogForm->ok, SIGNAL(clicked()), disableWidget, SLOT(hide()));

    //copy/paste - update paste availability based on whether anything has been copied
    connect(copyPasteHandler, SIGNAL(signalUpdatePasteAvailability()), this, SLOT(slotUpdatePasteAvailability()));

    //Import Old Preset Dialogs
    connect(importExportHandler, SIGNAL(signalPathFound()), disableWidget, SLOT(show()));
    connect(importExportHandler, SIGNAL(signalPathFound()), importOldDialogWidget, SLOT(show()));
    connect(importExportHandler, SIGNAL(signalPathFound()), importOldDialogWidget, SLOT(raise()));
    connect(importExportHandler, SIGNAL(signalImportingComplete()), importOldDialogWidget, SLOT(hide()));
    connect(importExportHandler, SIGNAL(signalImportingComplete()), disableWidget, SLOT(hide()));
    connect(importExportHandler, SIGNAL(signalPathNotFound()), disableWidget, SLOT(show()));
    connect(importExportHandler, SIGNAL(signalPathNotFound()), importOldNotFoundDialogWidget, SLOT(show()));
    connect(importExportHandler, SIGNAL(signalPathNotFound()), importOldNotFoundDialogWidget, SLOT(raise()));
    connect(importExportHandler, SIGNAL(signalPathNotFound()), importOldNotFoundDialoglForm->ok, SLOT(setFocus()));
    connect(importOldNotFoundDialoglForm->ok, SIGNAL(clicked()), importOldNotFoundDialogWidget, SLOT(hide()));
    connect(importOldNotFoundDialoglForm->ok, SIGNAL(clicked()), disableWidget, SLOT(hide()));
    connect(importExportHandler, SIGNAL(signalImportingPresetNum(QString)), importOldFoundDialogForm->importMessage, SLOT(setText(QString)));


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////// Window Resizing //////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //--------------------------------------- MainWindow resizing based on which tab is selected
    connect(tabArea, SIGNAL(currentChanged(int)), midiTab, SLOT(slotTabView(int)));
    connect(tabArea, SIGNAL(currentChanged(int)), this, SLOT(slotTabSizing(int))); //this one should always be the last thing the tabArea signal changes

    //--------------------------------------- Showing or Hiding Modlines
    connect(presetInterface, SIGNAL(signalRecallPreset(QVariantMap,QVariantMap)), midiTab, SLOT(slotRecallShowingModlines(QVariantMap,QVariantMap)));
    connect(midiTab, SIGNAL(signalWindowHeight(int)), this, SLOT(slotMidiTabHeight(int)));

    for(int m = 0; m < NUMBER_OF_MODLINES; m++)
    {
        connect(midiTab, SIGNAL(signalDeleteModline(int,int)), midiTab->modline[m], SLOT(slotHideModline(int,int)));
    }

    // ----------------
    // MIDI Overhaul
    //----------------- Firmware Out of Date Warning, Update Progress Bar, Update Complete


    // connect firmware detection
    connect(TwelveStep, SIGNAL(signalFirmwareDetected(MidiDeviceManager*, bool)), this, SLOT(slotFirmwareDetected(MidiDeviceManager*, bool)));

    // connect firmware update window and midi device manager controls and messaging
    connect(fwUpdateWindow, SIGNAL(signalRequestFwUpdate()), TwelveStep, SLOT(slotRequestFirmwareUpdate()));                   // request fw
    connect(TwelveStep, SIGNAL(signalFwConsoleMessage(QString)), fwUpdateWindow, SLOT(slotAppendTextToConsole(QString)));      // messaging
    connect(TwelveStep, SIGNAL(signalFwProgress(int)), fwUpdateWindow, SLOT(slotUpdateProgressBar(int)));                      // console
    connect(TwelveStep, SIGNAL(signalFirmwareUpdateComplete(bool)), fwUpdateWindow, SLOT(slotFwUpdateComplete(bool)));         // Update Complete
    connect(fwUpdateWindow, SIGNAL(signalFwUpdateSuccess()), TwelveStep, SLOT(slotFirmwareUpdateReset()));                   // stop timeout timers
    connect(fwUpdateWindow, SIGNAL(signalFwUpdateSuccessCloseDialog(bool)), this, SLOT(slotFwUpdateSuccessCloseDialog(bool)));      // close fw dialog and connect
    //connect(TwelveStep, SIGNAL(signalRequestGlobals()), this, SLOT(slotSendGlobalsRequest()));                                 // request globals
    //connect(sysExEncDecode, SIGNAL(signalGlobalsReceivedDoFwUd()), TwelveStep, SLOT(slotRequestFirmwareUpdate()));                      // if fwupdate requested globals then alert that we've saved them
    //connect(sysExEncDecode, SIGNAL(signalGlobalsReceived()), this, SLOT(slotEnableGlobalsWindows()));                       // enable globals window when we receive this
    //connect(TwelveStep, SIGNAL(signalRestoreGlobals()), this, SLOT(slotEncodeGlobals()));                                      // restore the globals after fw update

    // handle device unexpectedly in bootloader mode
    connect(TwelveStep, SIGNAL(signalBootloaderMode(bool)), this, SLOT(slotBootloaderMode(bool)));

    // reset portlist after sending bootloader commands, catch changes to port names
    //connect(TwelveStep, SIGNAL(signalBeginBlTimer()), this, SLOT(slotRefreshConnection()));
    //connect(TwelveStep, SIGNAL(signalBeginFwTimer()), this, SLOT(slotRefreshConnection()));


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////// FW Updating Dialogs ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //connect(this, SIGNAL(signalClosePorts()), sysexManager, SLOT(slotClosePorts()));
    // eb todo

    // MIDI overhaul
    connect(TwelveStep, SIGNAL(signalConnected(bool)), this, SLOT(slotShowConnection(bool)));

    //connect(sysexManager, SIGNAL(signalConnected(bool)), this, SLOT(slotShowConnection(bool)));
    //connect(sysexManager, SIGNAL(signalFwVersion(QString)), fwoodDialogForm->found, SLOT(setText(QString)));
    //connect(sysexManager, SIGNAL(signalFwVersion(QString)), aboutDialogForm->found, SLOT(setText(QString)));

    //connect(sysexManager, SIGNAL(signalStartTimer(int)), sysexManager, SLOT(slotStartTimer(int)));
    //connect(sysexManager, SIGNAL(signalStopTimer()), sysexManager, SLOT(slotStopTimer()));

    //fw updating dialogs
    //connect(sysexManager, SIGNAL(signalOpenFwDialog()), disableWidget, SLOT(show()));
    //connect(sysexManager, SIGNAL(signalOpenFwDialog()), disableWidget, SLOT(raise()));
    //connect(sysexManager, SIGNAL(signalOpenFwDialog()), this, SLOT(slotSetFwUpdateMessage()));
    //connect(sysexManager, SIGNAL(signalOpenFwDialog()), fwoodDialogWidget, SLOT(show()));
    //connect(sysexManager, SIGNAL(signalOpenFwDialog()), fwoodDialogWidget, SLOT(raise()));
    //connect(sysexManager, SIGNAL(signalOpenFwDialog()), fwoodDialogForm->update, SLOT(setFocus()));
//    connect(fwoodDialogForm->cancel, SIGNAL(clicked()), disableWidget, SLOT(close()));
//    connect(fwoodDialogForm->cancel, SIGNAL(clicked()), fwoodDialogWidget, SLOT(close()));
    //connect(fwoodDialogForm->cancel, SIGNAL(clicked()), sysexManager, SLOT(slotCancelFwUpdate()));
//    connect(fwoodDialogForm->cancel, SIGNAL(clicked()), fwProgressDialogWidget, SLOT(close())); //technically this shouldn't happen, but in case of error this makes it so you don't get stuck

//#ifdef Q_OS_MAC
//    connect(fwoodDialogForm->update, SIGNAL(clicked()), fwoodDialogWidget, SLOT(close()));
//    connect(fwoodDialogForm->update, SIGNAL(clicked()), fwProgressDialogWidget, SLOT(show()));
//    connect(fwoodDialogForm->update, SIGNAL(clicked()), fwProgressDialogWidget, SLOT(raise()));
//    connect(fwoodDialogForm->update, SIGNAL(clicked()), this, SLOT(slotFirmwareUpdateDelay()));
//#else
//    connect(fwoodDialogForm->update, SIGNAL(clicked()), sysexManager, SLOT(slotUpdateFw()));
//    connect(sysexManager, SIGNAL(signalFwUpdateStarted()), fwoodDialogWidget, SLOT(close()));
//    connect(sysexManager, SIGNAL(signalFwUpdateStarted()), fwProgressDialogWidget, SLOT(show()));
//    connect(sysexManager, SIGNAL(signalFwUpdateStarted()), fwProgressDialogWidget, SLOT(raise()));
//#endif

    //connect(sysexManager, SIGNAL(signalFwUpdateFinished()), fwProgressDialogWidget, SLOT(close()));
    //connect(sysexManager, SIGNAL(signalFwUpdateFinished()), fwUpdateCompleteDialogWidget, SLOT(show()));
    //connect(sysexManager, SIGNAL(signalFwUpdateFinished()), fwUpdateCompleteDialogWidget, SLOT(raise()));
    //connect(sysexManager, SIGNAL(signalFwUpdateFinished()), fwUpdateCompleteDialogForm->ok, SLOT(setFocus()));
//    connect(fwUpdateCompleteDialogForm->ok, SIGNAL(clicked()), fwUpdateCompleteDialogWidget, SLOT(close()));
//    connect(fwUpdateCompleteDialogForm->ok, SIGNAL(clicked()), disableWidget, SLOT(close()));
    //connect(fwUpdateCompleteDialogForm->ok, SIGNAL(clicked()), sysexManager, SLOT(slotResetVariables()));


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////// Preset Updating //////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    connect(&imageFormatter.deviceManager, SIGNAL(sigSysex(unsigned char*, int)), TwelveStep, SLOT(slotSendSysEx(unsigned char*,int)));

    //send presets on update button click (clean up first)
    connect(ui->update, SIGNAL(clicked()), this, SLOT(slotCleanUpSetlist()));
}

void MainWindow::slotConnectElements()
{
    connect(ui->preset_displayname_1, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    connect(ui->preset_displayname_2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    connect(ui->preset_displayname_3, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    connect(ui->preset_displayname_4, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
}

void MainWindow::slotDisconnectElements()
{
    disconnect(ui->preset_displayname_1, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(ui->preset_displayname_2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(ui->preset_displayname_3, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(ui->preset_displayname_4, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
}

void MainWindow::slotValueChanged()
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();

        //display name
        if(sender->metaObject()->className() == QString("QComboBox"))
        {
            QComboBox *combobox = qobject_cast<QComboBox *>(sender);
            emit signalStoreValue(combobox->objectName(), combobox->currentText(), -1);
        }
        else if(sender->metaObject()->className() == QString("QLineEdit"))
        {
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender);
            if(lineEdit->text().isEmpty())
            {
                saveAsDialogForm->save->setEnabled(false);
            }
            else
            {
                saveAsDialogForm->save->setEnabled(true);
                saveAsDialogForm->save->setDefault(true);
            }
        }
    }
    emit signalCheckSavedState();
}

void MainWindow::slotRecallPreset(QVariantMap preset, QVariantMap)
{
    slotDisconnectElements();

    ui->preset_displayname_1->setCurrentIndex(ui->preset_displayname_1->findText(preset.value(QString("preset_displayname_1")).toString()));
    ui->preset_displayname_2->setCurrentIndex(ui->preset_displayname_1->findText(preset.value(QString("preset_displayname_2")).toString()));
    ui->preset_displayname_3->setCurrentIndex(ui->preset_displayname_1->findText(preset.value(QString("preset_displayname_3")).toString()));
    ui->preset_displayname_4->setCurrentIndex(ui->preset_displayname_1->findText(preset.value(QString("preset_displayname_4")).toString()));

    slotConnectElements();
}

void MainWindow::slotPopulatePresetMenu()
{
    presetInterface->slotPopulatePresetMenu(ui->presetmenu);

    if(QObject::sender()->objectName().contains("delete"))
    {
        setlistTab->slotRecallSetlist();
        setlistTab->slotPopulateSetlistAfterDelete(ui->presetmenu);
    }
}

void MainWindow::slotSetPresetMenu(int presetNum)
{
    ui->presetmenu->setCurrentIndex(presetNum);
    presetInterface->slotRecallPreset(presetNum);
    setlistTab->slotRecallSetlist();
}

void MainWindow::slotDisplaySaveState(bool dirty)
{
    if(dirty)
    {
        ui->save->setStyleSheet("QToolButton { background:red } QToolButton:pressed { background: rgb(230,0,134) }");

        bool inSetlist = false;

        QMapIterator<QString, QVariant> i(setlistTab->setlist);

        while(i.hasNext())
        {
            i.next();

            if(ui->presetmenu->currentText() == i.value().toString())
            {
                inSetlist = true;
            }
        }

        if(inSetlist)
        {
            ui->update->setText("Save and Send Setlist");
            isInSetlistAndIsDirty = true;
        }
        else
        {
            ui->update->setText("Send Setlist to 12 Step");
            isInSetlistAndIsDirty = false;
        }
        //qDebug() << "Dirty Preset";
    }
    else
    {
        ui->save->setStyleSheet("");
        ui->update->setText("Send Setlist to 12 Step");
        isInSetlistAndIsDirty = false;
        //qDebug() << "Clean Preset";
    }
}

void MainWindow::slotShowGlobalDirtyStates()
{
    qDebug() << "slotShowGlobalDirtyStates called";
    if(connected)
    {
        if(QObject::sender())
        {
            QObject *sender = QObject::sender();
            QString senderName = sender->objectName();

            qDebug() << "senderName: " << senderName;

            if(senderName.contains("Settings"))
            {
                tabArea->setTabText(3, "*Settings");
            }
            else
            {
                tabArea->setTabText(2, "*Setlist");
            }
        }
    }
}

void MainWindow::slotSaveAs()
{
    QString saveAsName = presetInterface->slotAppendSuffixToNewPresetName(saveAsDialogForm->name->text());

    emit signalSaveAs(saveAsName);
    saveAsDialogWidget->close();
    disableWidget->close();
    saveAsDialogForm->name->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////    Menu Bar   ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::slotInitMenuBar()
{
    qDebug() << "slotInitMenuBar called";
#ifdef Q_OS_MAC
    menubar = new QMenuBar(0);
#else
    menubar = new QMenuBar(this);
    menubar->setGeometry(0,0,this->width(),20);
#endif

    qDebug() << "File";
    //------------------------------------- File
    QMenu *file = new QMenu("File");
    file->setObjectName("FileMenu");
    menubar->addMenu(file);

    qDebug() << "Import/Export";
    //---------------- Import / Export
    QAction *exportPreset = new QAction("Export Preset", file);
    exportPreset->setObjectName("exportPreset");
    connect(exportPreset, SIGNAL(triggered()), importExportHandler, SLOT(slotExportPreset()));
    file->addAction(exportPreset);

    QAction *importPreset = new QAction("Import Preset", file);
    importPreset->setObjectName("importPreset");
    connect(importPreset, SIGNAL(triggered()), importExportHandler, SLOT(slotImportPreset()));
    file->addAction(importPreset);

    QAction *importOldPreset = new QAction("Import All Presets from V1.0", file);
    importOldPreset->setObjectName("importOldPresets");
    connect(importOldPreset, SIGNAL(triggered()), importExportHandler, SLOT(slotImportOldPreset()));
    file->addAction(importOldPreset);

    //------------------------------------ Edit
    QMenu *edit = new QMenu("Edit");
    edit->setObjectName("EditMenu");
    menubar->addMenu(edit);

    //---------------- Clear Preset
    clearPresetAct = new QAction("Clear Preset", edit);
    actionList.append(clearPresetAct);
    edit->addAction(clearPresetAct);
    connect(clearPresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotClearPreset()));

    //---------------- Copy / Paste
    copyPresetAct = new QAction("Copy Preset", edit);
    actionList.append(copyPresetAct);
    edit->addAction(copyPresetAct);
    copyPresetAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C));
    connect(copyPresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotCopyPreset()));

    pastePresetAct = new QAction("Paste Preset", edit);
    actionList.append(pastePresetAct);
    edit->addAction(pastePresetAct);
    pastePresetAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));
    connect(pastePresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotPastePreset()));
    pastePresetAct->setDisabled(true);

    pasteNewPresetAct = new QAction("Paste Preset to New", edit);
    actionList.append(pasteNewPresetAct);
    edit->addAction(pasteNewPresetAct);
    connect(pasteNewPresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotPasteNewPreset()));
    pasteNewPresetAct->setDisabled(true);

    //---------------- Setlist Extras
    QAction *clearSetlist = new QAction("Clear Setlist", edit);
    actionList.append(clearSetlist);
    edit->addAction(clearSetlist);
    connect(clearSetlist, SIGNAL(triggered()), setlistTab, SLOT(slotClearSetlist()));

    QAction *autoPopulateSetlist = new QAction("Auto Populate Setlist", edit);
    actionList.append(autoPopulateSetlist);
    edit->addAction(autoPopulateSetlist);
    connect(autoPopulateSetlist, SIGNAL(triggered()), this, SLOT(slotAutoPopulateSetlist()));

    qDebug() << "Hardware menu";
    //------------------------------------ Hardware
    QMenu *hardware = new QMenu("Hardware");
    hardware->setObjectName("HardwareMenu");
    menubar->addMenu(hardware);

    //reload firmware
    updateFirmwareAct = new QAction("Update/Reload Firmware...", hardware);
    actionList.append(updateFirmwareAct);
    connect(updateFirmwareAct, SIGNAL(triggered()), disableWidget, SLOT(raise()));
    connect(updateFirmwareAct, SIGNAL(triggered()), disableWidget, SLOT(show()));
    // EB Todo - debug
    connect(updateFirmwareAct, SIGNAL(triggered()), this, SLOT(slotForceFirmwareUpdate()));
    //connect(updateFirmwareAct, SIGNAL(triggered()), fwoodDialogWidget, SLOT(raise()));
    //connect(updateFirmwareAct, SIGNAL(triggered()), fwoodDialogWidget, SLOT(show()));
    hardware->addAction(updateFirmwareAct);
    updateFirmwareAct->setDisabled(true);

    qDebug() << "Help Menu";
    //------------------------------------ Help
    QMenu *help = new QMenu("Help");
    help->setObjectName("HelpMenu");
    menubar->addMenu(help);

    //about
    QAction *about = new QAction("About 12 Step Editor", help);
    actionList.append(about);
    connect(about, SIGNAL(triggered()), disableWidget, SLOT(raise()));
    connect(about, SIGNAL(triggered()), disableWidget, SLOT(show()));
    connect(about, SIGNAL(triggered()), aboutDialogWidget, SLOT(raise()));
    connect(about, SIGNAL(triggered()), aboutDialogWidget, SLOT(show()));
    connect(about, SIGNAL(triggered()), aboutDialogForm->ok, SLOT(setFocus()));
    connect(aboutDialogForm->ok, SIGNAL(clicked()), aboutDialogWidget, SLOT(hide()));
    connect(aboutDialogForm->ok, SIGNAL(clicked()), disableWidget, SLOT(hide()));
    help->addAction(about);

    //doc
    QAction *doc = new QAction("Documentation...", help);
    connect(doc, SIGNAL(triggered()), this, SLOT(slotOpenDoc()));
    actionList.append(doc);
    help->addAction(doc);
    help->addSeparator();

    //tooltips
    if(sessionSettings->contains("toolTipsEnabled"))
    {
        if(sessionSettings->value("toolTipsEnabled").toBool())
        {
            toolTipsEnable = new QAction("Hide Tool Tips", file);
            toolTipEventFilter.toolTipsOn = true;
        }
        else
        {
            toolTipsEnable = new QAction("Show Tool Tips", file);
            toolTipEventFilter.toolTipsOn = false;
        }
    }
    else
    {
        sessionSettings->setValue("toolTipsEnabled", true);
        toolTipsEnable = new QAction("Hide Tool Tips", file);
    }
    connect(toolTipsEnable, SIGNAL(triggered()), this, SLOT(slotEnableDisableToolTips()));
    help->addAction(toolTipsEnable);

    menubar->show();
}

void MainWindow::slotUpdatePasteAvailability()
{
    //enable and disable paste options depending on whether anything is copied
    if(copyPasteHandler->presetCopiedMap.size())
    {
        pastePresetAct->setDisabled(false);
        pasteNewPresetAct->setDisabled(false);
    }
}

void MainWindow::slotOpenDoc()
{
    QDesktopServices::openUrl(QUrl("http://files.keithmcmillen.com/downloads/12step/12_Step_Manual_V2.0.pdf"));
}

void MainWindow::slotEnableDisableToolTips()
{
    if(sessionSettings->value("toolTipsEnabled").toBool())
    {
        toolTipsEnable->setText("Show Tool Tips");
        sessionSettings->setValue("toolTipsEnabled", false);
        toolTipEventFilter.toolTipsOn = false;
    }
    else
    {
        toolTipsEnable->setText("Hide Tool Tips");
        sessionSettings->setValue("toolTipsEnabled", true);
        toolTipEventFilter.toolTipsOn = true;
    }
}

void MainWindow::slotAutoPopulateSetlist()
{
    setlistTab->slotAutoPopulateSetlist(ui->presetmenu);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Window Resizing Functions ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::slotTabSizing(int tabIndex)
{
    if(tabIndex == 0) //Keyboard Tab
    {
        this->setFixedSize(KEYTAB_WIDTH + TAB_X_POS*2, KEYTAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING);
        tabArea->setFixedHeight(KEYTAB_HEIGHT);
    }
    else if(tabIndex == 1) //MIDI Tab
    {
        presetInterface->slotRecallPreset(-1); // to re-adjust window size for the number of modlines showing
        this->setFixedSize(MIDITAB_WIDTH + TAB_X_POS*2, mainWindowHeight);
    }
    else if(tabIndex == 2) //Setlist Tab
    {
        this->setFixedSize(SETLISTTAB_WIDTH + TAB_X_POS*2, SETLISTTAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING);
        tabArea->setFixedHeight(SETLISTTAB_HEIGHT);
    }
    else if(tabIndex == 3) //Settings Tab
    {
        this->setFixedSize(SETTINGSTAB_WIDTH + TAB_X_POS*2, SETTINGSTAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING);
        tabArea->setFixedHeight(SETTINGSTAB_HEIGHT);
    }

    //set dialog box locations
    if(tabIndex != 3) // everything other than the settings tab
    {
        saveAsDialogWidget->setGeometry(mainWindowWidth/2 - SAVEASWIDGET_WIDTH/2, mainWindowHeight/2 - SAVEASWDIGET_HEIGHT/2, SAVEASWIDGET_WIDTH, SAVEASWDIGET_HEIGHT);
        deleteDialogWidget->setGeometry(mainWindowWidth/2 - DELETEWIDGET_WIDTH/2, mainWindowHeight/2 - DELETEWIDGET_HEIGHT/2, DELETEWIDGET_WIDTH, DELETEWIDGET_HEIGHT);
        aboutDialogWidget->setGeometry(mainWindowWidth/2 - ABOUTWIDGET_WIDTH/2, mainWindowHeight/2 - ABOUTWIDGET_HEIGHT/2, ABOUTWIDGET_WIDTH, ABOUTWIDGET_HEIGHT);
        importOldDialogWidget->setGeometry(mainWindowWidth/2 - importOldDialogWidget->width()/2, mainWindowHeight/2 - importOldDialogWidget->height()/2, importOldDialogWidget->width(), importOldDialogWidget->height());
        importOldNotFoundDialogWidget->setGeometry(mainWindowWidth/2 - importOldNotFoundDialogWidget->width()/2, mainWindowHeight/2 - importOldNotFoundDialogWidget->height()/2, importOldNotFoundDialogWidget->width(), importOldNotFoundDialogWidget->height());
        //fwoodDialogWidget->setGeometry(mainWindowWidth/2 - fwoodDialogWidget->width()/2, mainWindowHeight/2 - fwoodDialogWidget->height()/2, fwoodDialogWidget->width(), fwoodDialogWidget->height());
        //fwUpdateCompleteDialogWidget->setGeometry(mainWindowWidth/2 - fwUpdateCompleteDialogWidget->width()/2, mainWindowHeight/2 - fwUpdateCompleteDialogWidget->height()/2, fwUpdateCompleteDialogWidget->width(), fwUpdateCompleteDialogWidget->height());
        //fwProgressDialogWidget->setGeometry(mainWindowWidth/2 - fwProgressDialogWidget->width()/2, mainWindowHeight/2 - fwProgressDialogWidget->height()/2, fwProgressDialogWidget->width(), fwProgressDialogWidget->height());
    }
    else
    {
        saveAsDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - SAVEASWIDGET_WIDTH/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - SAVEASWDIGET_HEIGHT/2, SAVEASWIDGET_WIDTH, SAVEASWDIGET_HEIGHT);
        deleteDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - DELETEWIDGET_WIDTH/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - DELETEWIDGET_HEIGHT/2, DELETEWIDGET_WIDTH, DELETEWIDGET_HEIGHT);
        aboutDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - ABOUTWIDGET_WIDTH/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - ABOUTWIDGET_HEIGHT/2, ABOUTWIDGET_WIDTH, ABOUTWIDGET_HEIGHT);
        importOldDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - importOldDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - importOldDialogWidget->height()/2, importOldDialogWidget->width(), importOldDialogWidget->height());
        importOldNotFoundDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - importOldNotFoundDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - importOldNotFoundDialogWidget->height()/2, importOldNotFoundDialogWidget->width(), importOldNotFoundDialogWidget->height());
        //fwoodDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - fwoodDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - fwoodDialogWidget->height()/2, fwoodDialogWidget->width(), fwoodDialogWidget->height());
        //fwUpdateCompleteDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - fwUpdateCompleteDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - fwUpdateCompleteDialogWidget->height()/2, fwUpdateCompleteDialogWidget->width(), fwUpdateCompleteDialogWidget->height());
        //fwProgressDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - fwProgressDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - fwProgressDialogWidget->height()/2, fwProgressDialogWidget->width(), fwProgressDialogWidget->height());
    }
}

void MainWindow::slotMidiTabHeight(int modlinesShowing)
{
    int newMainWindowHeight = (MIDITAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING - ((MODLINE_HEIGHT + MODLINE_SPACING) * (NUMBER_OF_MODLINES - modlinesShowing)));
    this->setFixedHeight(newMainWindowHeight);
    tabArea->setFixedHeight(newMainWindowHeight - TAB_Y_POS - MAINWINDOW_BOTTOM_SPACING);
    midiTabAreaWidget->setFixedHeight(newMainWindowHeight - TAB_Y_POS - MAINWINDOW_BOTTOM_SPACING);
    mainWindowHeight = newMainWindowHeight;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// MIDI / Sysex Stuff ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::slotShowConnection(bool connection)
{
    connected = connection;

    //highlight the connected/not connected labels based on the connection state
    if(connected)
    {
#ifdef Q_OS_MAC
        ui->connected->setStyleSheet("QLabel {background:transparent;font:18px 'Futura PT'; color:rgba(0, 174, 237, 175);}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#else
        ui->connected->setStyleSheet("QLabel {background:transparent;font:18px 'Futura-Normal'; color:rgba(0, 174, 237, 175);}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#endif
        ui->connected->setText("IS CONNECTED");
        ui->update->setEnabled(true);
        updateFirmwareAct->setDisabled(false);
//        sysexManager->slotSendFwQuery();
        aboutDialogForm->found->setText(deviceFirmwareVersionString());
    }
    else
    {
#ifdef Q_OS_MAC
        ui->connected->setStyleSheet("QLabel {background:transparent;font:18px 'Futura PT'; color:rgba(207, 0, 18, 175);}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#else
        ui->connected->setStyleSheet("QLabel {background:transparent;font:18px 'Futura-Normal'; color:rgba(207, 0, 18, 175);}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#endif
        ui->connected->setText("IS NOT CONNECTED");
        ui->update->setEnabled(false);
        updateFirmwareAct->setDisabled(true);
        aboutDialogForm->found->setText("Not Connected");
    }
}

//#ifdef Q_OS_MAC
//void MainWindow::slotFirmwareUpdateDelay()
//{
//    //sysexManager->fwUpdateStarted = true;

//    //QTimer::singleShot(2000, sysexManager, SLOT(slotUpdateFw()));
//    QTimer *updateDelay = new QTimer(this);

//    //updateDelay->singleShot(2000, sysexManager, SLOT(slotUpdateFw()));
//}
//#endif

//void MainWindow::slotSetFwUpdateMessage()
//{
//    if(QObject::sender())
//    {
//        QObject *sender = QObject::sender();
//        QString senderName = sender->objectName();

//        if(senderName != "SysexManager")
//        {
//            fwoodDialogForm->oodLabel->setText("Load the latest firmware.");
//        }
//        else
//        {
//            fwoodDialogForm->oodLabel->setText("Your firmware is out of date.");
//        }
//    }
//}

void MainWindow::slotCleanUpSetlist()
{
    presetsSending = true;

    if(isInSetlistAndIsDirty) // if the current preset is in the setlist and the preset is dirty, we should save before sending
    {
        presetInterface->slotSavePreset();
    }

    //clean up the setlist
    setlistTab->slotCleanUpSetlist();

    slotSendPresets();

    if(sysexManager->connected)
    {
        tabArea->setTabText(2, "Setlist");
        tabArea->setTabText(3, "Settings");
    }
}

void MainWindow::slotSendPresets()
{
    //------- Do settings
    slotSendSettings();

    //------- Do image
    QVariantMap reduceSetlist; //This should contain actual preset maps

    QMapIterator<QString, QVariant>  i(setlistTab->setlist);

    while(i.hasNext())
    {
        i.next();

        QString setlistPresetName = i.value().toString();

        if(setlistPresetName != QString("[EMPTY]"))
        {
            for(int j = 0; j < presetInterface->slotGetNumPresetsInJson(); j++)
            {
                QString jsonString = presetInterface->jsonMasterMap.value(presetInterface->slotGetPresetStringFromInt(j)).toMap().value("preset_name").toString();

                if(jsonString == setlistPresetName)
                {
                    reduceSetlist.insert(i.key(), presetInterface->jsonMasterMap.value(presetInterface->slotGetPresetStringFromInt(j)));
                    //qDebug() << i.key() << presetInterface->jsonMasterMap.value(presetInterface->slotGetPresetStringFromInt(j)).toMap().value("preset_name").toString();
                }
            }
        }
    }

    ///The Image Formatter Function below does 3 Things:
    /// 1. Assigns json / qvariantmap values to our preset image struct
    /// 2. Uses imageFormatter's DeviceManager to create a SysEx message from our image struct
    /// 3. Emits a signal with a pointer to the (unsigned char*) image data and byte length

    imageFormatter.formatImage(reduceSetlist);

    //this delay keeps the preset sending boolean as true for 3 more seconds after presets are sent -- this prevents the user from quitting in the middle of a preset send
    QTime presetSendingTimeout = QTime::currentTime().addSecs(3);
    while(QTime::currentTime() < presetSendingTimeout)
    {
        QCoreApplication::processEvents();
    }
    presetsSending = false;
}

void MainWindow::slotSendSettings()
{
    ///This Function below does 3 Things:
    /// 1. Assigns json / qvariantmap values to our preset image struct
    /// 2. Uses imageFormatter's DeviceManager to create a SysEx message from our image struct
    /// 3. Emits a signal with a pointer to the (unsigned char*) image data and byte length

    imageFormatter.formatSettings(globalPresetInterface->settings.value("Global").toMap());
}

// MIDI OVERHAUL --------------------------------------
QString MainWindow::deviceBootloaderVersionString()
{
    return QString("Device Bootloader Version: %1.%2.%3\n\n")
            .arg(uchar(TwelveStep->devicebootloaderVersion.at(0)))
            .arg(uchar(TwelveStep->devicebootloaderVersion.at(1)))
            .arg(uchar(TwelveStep->devicebootloaderVersion.at(2)));
}


QString MainWindow::deviceFirmwareVersionString()
{
    if (!connected) return QString("Device not connected");

    return QString("Device Firmware Version: %1.%2.%3")
            .arg(uchar(TwelveStep->deviceFirmwareVersion.at(0)))
            .arg(uchar(TwelveStep->deviceFirmwareVersion.at(1)))
            .arg(uchar(TwelveStep->deviceFirmwareVersion.at(2)));
}

QString MainWindow::applicationFirmwareVersionString()
{
    return QString("Application Firmware Version: %1.%2.%3")
            .arg(uchar(thisFw.at(0)))
            .arg(uchar(thisFw.at(1)))
            .arg(uchar(thisFw.at(2)));
}
// END MIDI OVERHAUL --------------------------------------


// --------------------------------------------------------------------------------------
// ------ midi overhaul -----------------------------------------------------------------
// --------------------------------------------------------------------------------------

void MainWindow::slotMIDIPortChange(QString portName, uchar inOrOut, uchar messageType, int portNum)
{
    //qDebug() << "slotMIDIPortChange - " << kmiPorts->mType[messageType] << kmiPorts->inOut[inOrOut] << " portName:" << portName << " messageType: " << " portNum: " << portNum << "\n";

    switch (messageType)
    {
    case PORT_CONNECT:

        // update dropdown
        if (inOrOut == PORT_OUT && (portName != TWELVESTEP_OLD_OUT_P1 || portName == TWELVESTEP_OUT_P1)) // don't create feedback loop
        {
            midiTab->midiThru->addItem(portName); // update dropdown
            //slotFixDropDownWidth(midiTab->midiThru);

            if (portName == sessionSettings->value(MIDI_AUX_KEY).toString()) // if this port matches the last selected port
            {
                recallMidiAuxPortName = portName; // store name
                QTimer::singleShot(500, this, SLOT(slotRecallMIDIaux())); // wait, then set/update the port
            }

        }

        // **** TwelveStep connect *****************************************
        if ((portName == TWELVESTEP_OLD_IN_P1 || portName == TWELVESTEP_IN_P1) && inOrOut == PORT_IN)
        {
            TwelveStep->slotSetExpectedFW(thisFw);
            //qDebug() << "qn deviceName: " << TwelveStep->deviceName << " curfw is: " << TwelveStep->currentFwVer;
            TwelveStep->updatePortIn(portNum);
            fwUpdateWindow->slotAppendTextToConsole("\nTwelveStep Connected\n");
        }
        else if ((portName == TWELVESTEP_OLD_OUT_P1 || portName == TWELVESTEP_OUT_P1) && inOrOut == PORT_OUT)
        {
            TwelveStep->updatePortOut(portNum);
            TwelveStep->slotStartPolling("PORT_CONNECT"); // start polling when output port is added
        }

        break;
    case PORT_DISCONNECT:

        if (inOrOut == PORT_OUT)
        {
            // update dropdown
            if (midiTab->midiThru->currentText() == portName)
            {
                midiTab->midiThru->setCurrentIndex(0);
            }

            midiTab->midiThru->removeItem(midiTab->midiThru->findText(portName));
        }

        // **** TwelveStep disconnect **************************************
        if (portName == TWELVESTEP_OLD_IN_P1 || portName == TWELVESTEP_IN_P1)
        {
            // close ports and stop polling
            TwelveStep->slotCloseMidiIn(SIGNAL_SEND);
            TwelveStep->slotCloseMidiOut(SIGNAL_SEND);
            TwelveStep->slotStopPolling("PORT_DISCONNECT");
            if (inOrOut == PORT_IN) fwUpdateWindow->slotAppendTextToConsole("\nTwelveStep Disconnected\n");
        }

        break;
    case PORT_CHANGED:
        //qDebug() << " PORT CHANGED - name: " << portName << portName << " inOrOut: " << kmiPorts->inOut[inOrOut] << " messageType: " << kmiPorts->mType[messageType] << " portNum: " << portNum << "\n";

        // **** TwelveStep renumber ****************************************
        if ((portName == TWELVESTEP_OLD_IN_P1 || portName == TWELVESTEP_IN_P1) && inOrOut == PORT_IN)
        {
            TwelveStep->updatePortIn(portNum);
        }
        else if ((portName == TWELVESTEP_OLD_OUT_P1 || portName == TWELVESTEP_OUT_P1)&& inOrOut == PORT_OUT)
        {
            TwelveStep->updatePortOut(portNum);
        }

        break;
    default:
        break;
    }
}

// close and then reopen the TwelveStep ports
// this is needed when the bootloader and app port names do not match
void MainWindow::slotRefreshConnection()
{
    qDebug() << "slotRefreshConnection called";
//#ifdef Q_OS_MAC
    TwelveStep->slotResetConnections(TWELVESTEP_OLD_IN_P1, TWELVESTEP_OLD_IN_P1);
//#endif
}


void MainWindow::slotBootloaderMode(bool fwUpdateRequested)
{
    qDebug() << "slotBootloaderMode called - fwUpdateRequested: "<< fwUpdateRequested;
    if (!fwUpdateRequested)
    {
        QMessageBox msgBox;
        msgBox.setText("Your device is in bootloader mode. Click OK to attempt a firmware update.");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Ok)
        {
            slotForceFirmwareUpdate();
        }
    }
}

void MainWindow::slotFwUpdateSuccessCloseDialog(bool success)
{
    qDebug() << "slotFwUpdateSuccessCloseDialog called - success: " << success;

    if (success)
    {
        TwelveStep->fwUpdateRequested = false;

        slotUpdateMIDIaux();
        slotShowConnection(true);
        //slotEnableDisableMidiFunctions(true);
    }
    else
    {
        TwelveStep->slotFirmwareUpdateReset();
        slotShowConnection(false);
        //slotEnableDisableMidiFunctions(false);
    }
    disableWidget->hide(); // re-enable app and "ungrey" main window
}

void MainWindow::slotForceFirmwareUpdate()
{
    slotFirmwareDetected(TwelveStep, false); // act as if we received a firmware mismatch
}

void MainWindow::slotFirmwareDetected(MidiDeviceManager *thisMDM, bool matches)
{
    qDebug() << "slotFirmwareDetected called";
    if (matches)
    {
        qDebug() << "FirmwareMatch: " << thisMDM->PID << "name:" << thisMDM->deviceName;

        // EB TODO: this is a temporary cluge to make the editor work with old, non-bootloader firmware. Remove when fw1.0.0 is out
        TwelveStep->bootloaderMode = false;
    }
    else
    {
        qDebug() << "Firmware MisMatch: " << thisMDM->PID << "name:" << thisMDM->deviceName;

        // setup sysex connections to receive globals data
//        TwelveStep->disconnect(SIGNAL(signalRxSysExBA(QByteArray))); // disconnect to be safe
//        connect(TwelveStep, SIGNAL(signalRxSysExBA(QByteArray)), sysExDecode, SLOT(slotProcessSysEx(QByteArray)));

        fwUpdateWindow->slotClearText();
        fwUpdateWindow->slotAppendTextToConsole(deviceBootloaderVersionString());
        fwUpdateWindow->slotAppendTextToConsole(deviceFirmwareVersionString());

        fwUpdateWindow->show();

        // EB TODO: this is a temporary cluge to make the editor work with old, non-bootloader firmware. Remove when fw1.0.0 is out
        TwelveStep->bootloaderMode = true;
    }
}

// connect TwelveStep midi input to to midi aux out
void MainWindow::slotUpdateMIDIaux()
{
    qDebug() << "slotUpdateMIDIaux called - connected: " << connected;

    TwelveStep->disconnect(SIGNAL(signalRxMidi_raw(uchar, uchar, uchar, uchar)));

    if (!connected) return; // don't continue if we aren't connected

    sessionSettings->setValue(MIDI_AUX_KEY, midiTab->midiThru->currentText()); // store this setting for the next time we run the editor

    if (midiTab->midiThru->currentText() != "None")
    {
        qDebug() << "update aux port: " << midiTab->midiThru->currentText();


        //recallMidiAuxPortName = ""; // reset this, which is only set when the stored aux port connects

        // set and open the ports
        int thisOutPort = kmiPorts->getOutPortNumber(midiTab->midiThru->currentText());

        if (thisOutPort == -1)
        {
            qDebug() << "ERROR: Port \"" << midiTab->midiThru->currentText() << "\" does not exist, cannot open as MIDI aux port";
            return;
        }

        midiAuxOut->updatePortOut(thisOutPort);
        connect(TwelveStep, SIGNAL(signalRxMidi_raw(uchar, uchar, uchar, uchar)), midiAuxOut, SLOT(slotSendMIDI(uchar, uchar, uchar, uchar)));
    }
    else
    {
        qDebug() << "close port, don't update";
        midiAuxOut->slotCloseMidiOut(SIGNAL_NONE);
    }
}

void MainWindow::slotRecallMIDIaux()
{
    qDebug() << "slotRecallMIDIaux called, connected: " << connected << " recallMidiAuxPortName: " << recallMidiAuxPortName;
    if (!connected || recallMidiAuxPortName == "") return; // wait until connected to TwelveStep and the previously saved port

    midiTab->midiThru->setCurrentText(recallMidiAuxPortName);
    slotUpdateMIDIaux();
}

// --------------------------------------------------------------------------------------
// ------ end midi overhaul -------------------------------------------------------------
// --------------------------------------------------------------------------------------
