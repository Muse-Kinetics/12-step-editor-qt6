// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "mainwindow.h"
#include <QWidget>
#include <QCoreApplication>
#include "KMI_FwVersions.h"
#include "inc/KMI_Updates/kmi_updates.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),

    saveAsDialogForm(new Ui::saveAsDialogForm),
    deleteDialogForm(new Ui::deleteDialogForm),
    aboutDialogForm(new Ui::aboutDialogForm),
    importOldFoundDialogForm(new Ui::importOldFoundDialog),

    importOldNotFoundDialoglForm(new Ui::importOldNotFoundDialog),

    saveAsDialogWidget(new QWidget(this)),
    deleteDialogWidget(new QWidget(this)),
    aboutDialogWidget(new QWidget(this)),
    disableWidget(new QWidget(this)),
    importOldDialogWidget(new QWidget(this)),
    importOldNotFoundDialogWidget(new QWidget(this)),

    copyPasteHandler(new CopyPasteHandler(presetInterface, this)),
    importExportHandler(new ImportExportHandler(presetInterface, this))
{
    //plist stuff
    QCoreApplication::setApplicationName("12 Step Editor");
    QCoreApplication::setOrganizationName("KeithMcMillenInstruments");
    QCoreApplication::setOrganizationDomain("keithmcmillen.com");


    qDebug() << "System Locale: " << QLocale::system().name();

    // application version
    QString versionString = QString(APP_VERSION);

    // Split the version string by dots and assign values directly
    QStringList parts = versionString.split('.');

    for (int i = 0; i < 3 && i < parts.size(); ++i) {
        applicationVersion.append(static_cast<char>(parts[i].toInt()));
    }

    if (parts.size() > 3)
    {
        betaVersion = parts[3]; // assume this is a letter
    }
    else
    {
        betaVersion = "";
    }


    thisFw = QByteArray(reinterpret_cast<char*>(_fw_ver_12step), sizeof(_fw_ver_12step));


    // flag that the app has just started, avoid unsaved popup on open
    firstRunUnsavedFlag = true;


    // ******************************
    // KMI_Ports
    // ******************************

    // kmiPorts reports changes in MIDI i/o
    kmiPorts = new KMI_Ports(this);

    //qDebug() << "end connect";

    // ******************************
    // create KMI device handlers
    // ******************************

    TwelveStep = new MidiDeviceManager(this, PID_12STEP, "12Step", kmiPorts);

    // setup firmware image
    QString thisFwFile = QString(":/resources/firmware/12Step_Firmware_v%1.%2.%3.syx")
            .arg(uchar(thisFw.at(0)))
            .arg(uchar(thisFw.at(1)))
            .arg(uchar(thisFw.at(2)));


    if (!TwelveStep->slotOpenFirmwareFile(thisFwFile))
    {
        qDebug() << "ERROR - firmware file not found: " << thisFwFile;
        UserDialog firmwareError("ERROR - firmware file not found! Please re-install the application.", {"Exit"});
        firmwareError.exec();
        QCoreApplication::quit();
    }

    // setup MIDI aux output
    midiTHRU = new MidiDeviceManager(this, PID_AUX, "MIDI Thru", kmiPorts);

    // ******************************
    // end KMI_Ports and device handlers
    // ******************************

    // ******************************
    // check for updates and set default save locations
    // ******************************
    QString jsonVersionCheckURL = "https://files.keithmcmillen.com/products/12step/editor/softwareVersionCheck.json";
    checkUpdates = new KMI_Updates(this, "12step", sessionSettings, applicationVersion, jsonVersionCheckURL);

    // ******************************

    // ---- 12 Step
    mainWindowHeight = KEYTAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING;
    mainWindowWidth = KEYTAB_WIDTH + TAB_X_POS*2;

    ui->setupUi(this);


    // **********************************
    // Load Session Settings and File locations
    // **********************************

    // get session settings, make sure to pass this to objects that need them (ie preset interfaces looking for json files)
    sessionSettings = new QSettings(this); //session settings allow us to enable/disable tooltips -- see slotInitMenuBar

    // default file location
    const QString DEFAULT_DIR_KEY("default_dir");

    // test if this is a directory
    QFileInfo check_file(sessionSettings->value(DEFAULT_DIR_KEY, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());
    if (!check_file.exists() || !check_file.isDir() || sessionSettings->value(DEFAULT_DIR_KEY).toString().contains("Contents/MacOS"))
    {
        QString desktop = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
        qDebug() << "Desktop: " << desktop;
        sessionSettings->setValue(DEFAULT_DIR_KEY, desktop);     // if key doesn't exist, set it to desktop
    }

    qDebug() << "Default file save location: " << sessionSettings->value(DEFAULT_DIR_KEY).toString();

    // MIDI Overhaul and FW Update

    // Firmware update Window
    fwUpdateWindow = new fwUpdate(this, "12 Step", applicationFirmwareVersionString());

    // Troubleshooting Window
    troubleshootWindow = new troubleshoot(this, "12 Step", applicationFirmwareVersionString());

    troubleshootWindow->hide();

    connect(kmiPorts, SIGNAL(signalInputCount(int)), troubleshootWindow, SLOT(slotInputCount(int)));
    connect(kmiPorts, SIGNAL(signalOutputCount(int)), troubleshootWindow, SLOT(slotOutputCount(int)));
    connect(kmiPorts, SIGNAL(signalInputPort(QString,int)), troubleshootWindow, SLOT(slotInputPort(QString,int)));
    connect(kmiPorts, SIGNAL(signalOutputPort(QString,int)), troubleshootWindow, SLOT(slotOutputPort(QString,int)));

    // end MIDI Overhaul

    this->setWindowTitle("12 Step Editor");
    this->setGeometry(400, 35, mainWindowWidth, mainWindowHeight);
    this->setFixedSize(mainWindowWidth, mainWindowHeight);

    slotSetupStyleStringsAndFonts();

    //-------------------- Disable Widget
    qDebug() << "------------ [WIDGET SETUP] ---------------------------------------------------";
    disableWidget->hide();
    disableWidget->setGeometry(0,0,mainWindowWidth, mainWindowHeight);
    disableWidget->setStyleSheet("background: rgba(0,0,0,200);");

    //construct the tab widget and add the tabs to them
    tabArea = new QTabWidget(this);
    tabArea->setGeometry(TAB_X_POS, TAB_Y_POS, KEYTAB_WIDTH, KEYTAB_HEIGHT);
    tabArea->setObjectName("tabArea");
    tabArea->setStyleSheet(tabStyleString);


    // ------------ TABS ------------------------------------------------
    qDebug() << "------------ [KEY TAB SETUP] ---------------------------------------------------";
    keyTabAreaWidget = new QWidget(tabArea);
    tabArea->addTab(keyTabAreaWidget, QString("Note Entry"));  //move this when the keyboard tab is added
    keyTab = new KeyTab(keyTabAreaWidget);

    qDebug() << "------------ [MIDI TAB SETUP] ---------------------------------------------------";
    midiTabAreaWidget = new QWidget(tabArea);
    tabArea->addTab(midiTabAreaWidget, QString("MIDI Parameters"));

    midiTab = new MidiTab(midiTabAreaWidget);
    midiTab->slotConnectElements();

    qDebug() << "------------ [TOOLTIPS] ---------------------------------------------------";
    tabArea->setTabToolTip(0,"Set up Notes here.");
    tabArea->setTabToolTip(1,"Manage the MIDI output here.");
    tabArea->setTabToolTip(2,"Changes in the setlist tab will not be sent to the 12 Step until the “Send Setlist to 12 Step” button is clicked.");
    tabArea->setTabToolTip(3,"Changes in the settings tab will not be sent to the 12 Step until the “Send Setlist to 12 Step” button is clicked.");
    
    // ------------ END TABS ------------------------------------------------


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

    qDebug() << "**** Init Menu Bar ****";
    slotInitMenuBar();

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

    // delaying all interface connections and preset loading until after the window
    // has loaded. This allows us to show user dialogs if we can't find the preset
    // json files, among other things.
    QTimer::singleShot(0, this, &MainWindow::windowHasLoaded);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::windowHasLoaded()
{
    if (qApp->activeWindow()) {
        qDebug() << "Active window type:" << qApp->activeWindow()->metaObject()->className();
    } else {
        qDebug() << "No active window!";
        QTimer::singleShot(1000, this, &MainWindow::windowHasLoaded);
        return;
    }

    // locate presets, or fail
    if (slotCheckPresets())
    {


        qDebug() << "------------ [SETLIST TAB SETUP] ---------------------------------------------------";
        setlistTabAreaWidget = new QWidget(tabArea);
        tabArea->addTab(setlistTabAreaWidget, QString("Setlist"));

        setlistTab = new Setlist(setlistTabAreaWidget, sessionSettings);

        qDebug() << "------------ [SETTINGS TAB SETUP] ---------------------------------------------------";
        settingsTabAreaWidget = new QWidget(tabArea);
        tabArea->addTab(settingsTabAreaWidget, QString("Settings"));

        settingsTab = new Settings(settingsTabAreaWidget, sessionSettings);

        qDebug() << "------------ [MIDI THRU SETUP] ---------------------------------------------------";
        // MIDI thru dropdown
        // connect dropdowns and connection status to MIDI aux ports

        connect(settingsTab, SIGNAL(signalUpdateMIDIaux()), this, SLOT(slotUpdateMIDIaux()));
        connect(TwelveStep, SIGNAL(signalConnected(bool)), this, SLOT(slotUpdateMIDIaux()));

        // remember last selected MIDI aux port
        MIDI_AUX_KEY = "midi_aux_port";

        qDebug() << "connected aux port";

        presetInterface = new PresetInterface(this, sessionSettings);
        globalPresetInterface = new GlobalPresetInterface(this, sessionSettings);

        qDebug() << "**** Connect Interfaces ****";
        slotConnectInterfaces();

        qDebug() << "**** Set Preset Menu ****";
        slotSetPresetMenu(0);

        //set state of preset stuff
        presetsSending = false;
        presetInterface->slotUpdateJSONPath();
        presetInterface->slotReadJSON();
        presetInterface->slotPopulatePresetMenu(ui->presetmenu);
        presetInterface->slotRecallPreset(0);
        globalPresetInterface->slotRecallSettings();
        setlistTab->slotRecallSetlist();

        // start polling at 100ms intervals (do this after mainWindow setup is complete)
        qDebug() << "Start polling for MIDI ports";
        kmiPorts->devicePoller->start(100);

        // connect kmiPorts to our handler
        connect(kmiPorts, SIGNAL(signalPortUpdated(QString, uchar, uchar, int)), this, SLOT(slotMIDIPortChange(QString, uchar, uchar, int)));
    }
}

void MainWindow::slotOpenPresetDirectory()
{
    QString presetDir = sessionSettings->value("PRESET_DIR").toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(presetDir));
}

// search a directory for a list of files. If one doesn't exist, return it, otherwise return ""
QString locateFiles(QStringList filesToCheck, QString locationToSearch)
{
    for (const QString &fileName : filesToCheck)
    {
        QString sourceFile = locationToSearch + "/" + fileName;
        if (!QFile(sourceFile).exists() || QFile(sourceFile).size() < 100)
        {
            return fileName; // return missing file
        }
    }
    return ""; // success
}

bool MainWindow::slotCheckPresets()
{
    bool exitProgram = false;
    qDebug() << "slotCheckPresets called";

    // check preset directory
    if (!sessionSettings->contains("PRESET_DIR"))
    {
        sessionSettings->setValue("PRESET_DIR", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    QString sourceDir = ":/presets"; // default factory preset resource files location

    QString presetDir = sessionSettings->value("PRESET_DIR").toString(); // location of user presets, defaults to system appData directory
    QDir dir(presetDir);

    QStringList filesToCheck = {"12Step.json", "Blank.twelvesteppreset", "setlist.json", "settings.json"};

    QString missingFile = locateFiles(filesToCheck, presetDir);

    if (missingFile != "") // one of the files could not be found
    {
        QString popupMessage;
        bool pdValid = false;

        while (!pdValid && !exitProgram)
        {

            if (!sessionSettings->contains("APP_VERSION"))
            {
                // this is the first time we've run the app since adding checks for the preset directory
                popupMessage = "This is a new installation of the editor. Do you want to use the factory "
                               "default presets, or import presets from a previous editor install?";

                // writing this will change the error message if we loop back
                QList<QVariant> versionList;
                versionList << applicationVersion[0] << applicationVersion[1] << applicationVersion[2];
                sessionSettings->setValue("APP_VERSION", versionList);
            }
            else
            {
                // we've run the app before, but cannot find the presets directory or a specific file
                popupMessage = "ERROR: Could not locate preset file: \n \n"
                               + presetDir + "/" + missingFile + "\n \n"
                                "Do you want to use the factory default presets, or import presets from a previous editor install?";
            }

            UserDialog presetDialog(popupMessage, {"Factory Default", "Import Presets", "Exit Application"}, 500, 300);
            int pdResult = presetDialog.exec();


            switch(pdResult)
            {
                case 1: // locate old presets
                {
                    sourceDir = QFileDialog::getExistingDirectory(
                        this,                                                   // Parent widget
                        tr("Locate 12 Step Presets Directory"),                 // Dialog title
                        sessionSettings->value("default_dir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString() // starting directory
                    );

                    missingFile = locateFiles(filesToCheck, sourceDir); // check the new source directory for all of the files

                    if (missingFile != "")
                    {
                        UserDialog copyError("ERROR - Source file corrupt or does not exist: " + missingFile, {"Retry"});
                        copyError.exec();
                        break;
                    }
                    sessionSettings->setValue("default_dir", sourceDir); // update last directory viewed session setting
                }
                // source dir is legit, fall through
                case 0: // copy (factory or user) presets to appdata dir
                {
                    pdValid = true;
                    // Make sure the dest directory exists; create it if necessary
                    if (!dir.exists()) {
                        qDebug() << "Creating appData directory: " << presetDir;
                        dir.mkpath(".");
                    }
                    else
                    {
                        qDebug() << "Cleaning directory: " << presetDir;

                        // Get the list of all files in the directory
                        QStringList fileList = dir.entryList(QDir::Files);

                        // Iterate over the list and remove each file
                        for (const QString &file : fileList) {
                            dir.remove(file);
                        }
                    }

                    // copy files from source to dest
                    for (const QString &fileName : filesToCheck)
                    {
                        QString sourceFile = sourceDir + "/" + fileName;
                        QString destFile = presetDir + "/" + fileName;
                        QFile file(sourceFile);

                        if (!file.copy(destFile))
                        {
                            // We've checked that user files are valid and that the destination exists. If we get an error here, then the appdata dir is broken and we have to fail.
                            //pdValid = false;
                            qDebug() << "Copy failed - source: " << fileName << " dest: " << destFile << " error: " << file.errorString();
                            UserDialog copyError("Fatal Error: Could not copy " + fileName + " to (" + presetDir + "). Please double check that this directory exists, and that the application has permission to write to it.", {"EXIT"});
                            copyError.exec();
                            // exit application
                            //exitProgram = true;
                            std::exit(2);
                            QCoreApplication::exit(2);
                        }
                        else
                        {
                            int count = 10;
                            qDebug() << "Copied - source: " << fileName << " dest: " << destFile;
                            QFile::setPermissions(destFile, QFile::ReadUser | QFile::WriteUser);  // Set permissions

                            // this check is for slow filesystems, make sure the file is accessible
                            while (count-- > 0)
                            {
                                if (!QFile::exists(destFile))
                                {
                                    qDebug() << "File not accessible after copy: " << destFile << " attempt: " << count;
                                    if (count == 0)
                                    {
                                        UserDialog copyError("Fatal Error: Could not copy " + fileName + " to (" + presetDir + "). Please double check that this directory exists, and that the application has permission to write to it.", {"EXIT"});
                                        copyError.exec();
                                        std::exit(2);
                                        QCoreApplication::exit(2);
                                    }
                                    QThread::msleep(100);  // Add a delay
                                }
                                else
                                {
                                    count = 0;
                                }
                            }
                        }
                    }
                    break;
                }
                default:
                    // exit application
                    //exitProgram = true;
                    std::exit(2);
                    QCoreApplication::exit(2);
                    break;
            } // switch case
        } // while loop
    } // preset dir check

    if (exitProgram)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void MainWindow::slotSetupStyleStringsAndFonts()
{

    // ---- FONTS --------------------------
    qDebug() << "------------ [FONTS SETUP] ---------------------------------------------------";

    QString droidFont = ":/fonts/droid-sans/DroidSansMono.ttf";
    QString futuraFont = ":/fonts/futura/futura-normal.ttf";
    QString futuraBFont = ":/fonts/futura/Futura-Bold.ttf";
    QString futuraPTFont = ":/fonts/futura/FuturaPT-Book.otf";
    QString corbelFont = ":/fonts/corbel/corbel.ttf";
    QString corbelBFont = ":/fonts/corbel/corbelb.ttf";
    QString sourceFont = ":/fonts/source-sans-pro/SourceSansPro-Regular.otf";

    if (QFontDatabase::addApplicationFont(droidFont) == -1) qDebug() << "Could not load font: " << droidFont;
    if (QFontDatabase::addApplicationFont(futuraFont) == -1) qDebug() << "Could not load font: " << futuraFont;
    if (QFontDatabase::addApplicationFont(futuraPTFont) == -1) qDebug() << "Could not load font: " << futuraPTFont;
    if (QFontDatabase::addApplicationFont(futuraBFont) == -1) qDebug() << "Could not load font: " << futuraBFont;
    if (QFontDatabase::addApplicationFont(corbelFont) == -1) qDebug() << "Could not load font: " << corbelFont;
    if (QFontDatabase::addApplicationFont(corbelBFont) == -1) qDebug() << "Could not load font: " << corbelBFont;
    if (QFontDatabase::addApplicationFont(sourceFont) == -1) qDebug() << "Could not load font: " << sourceFont;

    // ---- end FONTS -------------------------

     qDebug() << "------------ [STYLESHEETS SETUP] ---------------------------------------------------";
    // general stylesheets
#ifdef Q_OS_MAC
    generalStylesFile = new QFile(":stylesheets/GeneralStyles.qss");

    dialogStylesFile = new QFile(":/stylesheets/appDialog_QuNexus.qss");
#else
    generalStylesFile = new QFile(":stylesheets/GeneralStylesWindows.qss");
    dialogStylesFile = new QFile(":/stylesheets/appDialog_QuNexus_WIN.qss");
#endif
    if (!generalStylesFile->open(QFile::ReadOnly))
    {
        qDebug() << "ERROR: could not open stylesheet: " << generalStylesFile->fileName();
    }
    else
    {
        generalStylesString = QLatin1String(generalStylesFile->readAll());
    }

    if (!dialogStylesFile->open(QFile::ReadOnly))
    {
        qDebug() << "ERROR: could not open stylesheet: " << dialogStylesFile->fileName();
    }
    else
    {
        dialogStylesString = QLatin1String(dialogStylesFile->readAll());
    }

    //StyleSheets for "grey" pushbuttons
    grayStyleFile = new QFile(":stylesheets/GrayButtonStyleSheet.qss");
    grayStyleFile->open(QFile::ReadOnly);
    grayStyleString = QLatin1String(grayStyleFile->readAll());

    // TABS
#ifdef Q_OS_MAC
    tabStyleString = QString("QTabWidget {}"
                           "QTabWidget::pane {border: 10px solid rgb(89, 89, 89);}"
                           "QTabWidget::tab-bar {left: 20px;}"
                           "QTabBar::tab {margin-left: 5; margin-right: 5; min-width: 100px; background-color: rgb(40, 40, 40); font: 14pt 'Futura PT'; color: rgb(242, 242, 242); border-left: 3px solid rgb(89, 89, 89); border-right: 3px solid rgb(89, 89, 89); border-top: 3px solid rgb(89, 89, 89); padding: 4px;}"
                           "QTabBar::tab:selected {background-color: rgb(89, 89, 89);}"
                           );
#else
    tabStyleString = QString("QTabWidget {}"
                           "QTabWidget::pane {border: 10px solid rgb(89, 89, 89);}"
                           "QTabWidget::tab-bar {left: 20px;}"
                           "QTabBar::tab {margin-left: 5; margin-right: 5; min-width: 100px; background-color: rgb(40, 40, 40); font: 10pt 'Corbel'; color: rgb(242, 242, 242); border-left: 3px solid rgb(89, 89, 89); border-right: 3px solid rgb(89, 89, 89); border-top: 3px solid rgb(89, 89, 89); padding: 4px;}"
                           "QTabBar::tab:selected {background-color: rgb(89, 89, 89);}"
                           );
#endif

}

void MainWindow::closeEvent(QCloseEvent *event)
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
    event->accept();
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
    connect(setlistTab, SIGNAL(signalSetlistDirty()), this, SLOT(slotShowGlobalDirtyStates()));
    connect(settingsTab, SIGNAL(signalSettingsDirty()), this, SLOT(slotShowGlobalDirtyStates()));

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

    // troubleshooter
    connect(ui->connected, SIGNAL(clicked()), this, SLOT(slotOpenTroubleshooting()));

    //--------------------------------------- Setlist
    connect(presetInterface, SIGNAL(signalPopulateSetlistMenus(QComboBox*)), setlistTab, SLOT(slotPopulateSetlistMenus(QComboBox*)));

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////// Menu Items /////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // File menu connections
    connect(exportPreset, SIGNAL(triggered()), importExportHandler, SLOT(slotExportPreset()));
    connect(importPreset, SIGNAL(triggered()), importExportHandler, SLOT(slotImportPreset()));
    connect(importOldPreset, SIGNAL(triggered()), importExportHandler, SLOT(slotImportOldPreset()));
    connect(openAppDataDir, SIGNAL(triggered()), this, SLOT(slotOpenPresetDirectory()));

    // Edit menu connections
    connect(clearPresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotClearPreset()));
    connect(copyPresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotCopyPreset()));
    connect(pastePresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotPastePreset()));
    connect(pasteNewPresetAct, SIGNAL(triggered()), copyPasteHandler, SLOT(slotPasteNewPreset()));
    connect(clearSetlist, SIGNAL(triggered()), setlistTab, SLOT(slotClearSetlist()));
    connect(autoPopulateSetlist, SIGNAL(triggered()), this, SLOT(slotAutoPopulateSetlist()));

    // Hardware menu connections
    connect(updateFirmwareAct, SIGNAL(triggered()), disableWidget, SLOT(raise()));
    connect(updateFirmwareAct, SIGNAL(triggered()), disableWidget, SLOT(show()));
    connect(updateFirmwareAct, SIGNAL(triggered()), this, SLOT(slotForceFirmwareUpdate()));

    // Help menu connections
    connect(about, SIGNAL(triggered()), disableWidget, SLOT(raise()));
    connect(about, SIGNAL(triggered()), disableWidget, SLOT(show()));
    connect(about, SIGNAL(triggered()), aboutDialogWidget, SLOT(raise()));
    connect(about, SIGNAL(triggered()), aboutDialogWidget, SLOT(show()));
    connect(about, SIGNAL(triggered()), aboutDialogForm->ok, SLOT(setFocus()));
    connect(aboutDialogForm->ok, SIGNAL(clicked()), aboutDialogWidget, SLOT(hide()));
    connect(aboutDialogForm->ok, SIGNAL(clicked()), disableWidget, SLOT(hide()));
    connect(doc, SIGNAL(triggered()), this, SLOT(slotOpenDoc()));
    connect(troubleShoot, SIGNAL(triggered()), this, SLOT(slotOpenTroubleshooting()));
    connect(toolTipsEnable, SIGNAL(triggered()), this, SLOT(slotEnableDisableToolTips()));

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

    // connect fwUpdate console messages to connection troubleshooter
    connect(TwelveStep, SIGNAL(signalFwConsoleMessage(QString)), troubleshootWindow, SLOT(slotAppendToStatusLog(QString)));
    connect(fwUpdateWindow, SIGNAL(signalRequestFwUpdate()), troubleshootWindow, SLOT(slotRequestFwUpdate()));
    connect(TwelveStep, SIGNAL(signalFirmwareUpdateComplete(bool)), troubleshootWindow, SLOT(slotFirmwareUpdated(bool)));

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
    menubar = new QMenuBar(this);

#ifndef Q_OS_MAC
    QFile menuStyleFile = QFile(":/stylesheets/menuBarWin.qss");
    QString menuStyleString;

    if (menuStyleFile.open(QFile::ReadOnly))
    {
        menuStyleString = QLatin1String(menuStyleFile.readAll());
        menubar->setStyleSheet(menuStyleString);
    }
    else
    {
        qDebug() << "ERROR - Could not find menubar stylesheet: " << menuStyleString;
    }

    menubar->setGeometry(0,0, this->width(), 25);
#endif

    //------------------------------------- File
    QMenu *file = new QMenu("File");
    file->setObjectName("FileMenu");
    menubar->addMenu(file);

    //---------------- Import / Export
    exportPreset = new QAction("Export Preset", file);
    exportPreset->setObjectName("exportPreset");
    file->addAction(exportPreset);

    importPreset = new QAction("Import Preset", file);
    importPreset->setObjectName("importPreset");
    file->addAction(importPreset);

    importOldPreset = new QAction("Import All Presets from V1.0", file);
    importOldPreset->setObjectName("importOldPresets");
    file->addAction(importOldPreset);

    openAppDataDir = new QAction("Open Editor Preset Directory", file);
    openAppDataDir->setObjectName("openAppDataDir");
    file->addAction(openAppDataDir);

    //------------------------------------ Edit
    QMenu *edit = new QMenu("Edit");
    edit->setObjectName("EditMenu");
    menubar->addMenu(edit);

    //---------------- Clear Preset
    clearPresetAct = new QAction("Clear Preset", edit);
    actionList.append(clearPresetAct);
    edit->addAction(clearPresetAct);

    //---------------- Copy / Paste
    copyPresetAct = new QAction("Copy Preset", edit);
    actionList.append(copyPresetAct);
    edit->addAction(copyPresetAct);
    copyPresetAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));

    pastePresetAct = new QAction("Paste Preset", edit);
    actionList.append(pastePresetAct);
    edit->addAction(pastePresetAct);
    pastePresetAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_V));
    pastePresetAct->setDisabled(true);

    pasteNewPresetAct = new QAction("Paste Preset to New", edit);
    actionList.append(pasteNewPresetAct);
    edit->addAction(pasteNewPresetAct);
    pasteNewPresetAct->setDisabled(true);

    //---------------- Setlist Extras
    clearSetlist = new QAction("Clear Setlist", edit);
    actionList.append(clearSetlist);
    edit->addAction(clearSetlist);

    autoPopulateSetlist = new QAction("Auto Populate Setlist", edit);
    actionList.append(autoPopulateSetlist);
    edit->addAction(autoPopulateSetlist);

    //------------------------------------ Hardware
    QMenu *hardware = new QMenu("Hardware");
    hardware->setObjectName("HardwareMenu");
    menubar->addMenu(hardware);

    //reload firmware
    updateFirmwareAct = new QAction("Update/Reload Firmware...", hardware);
    actionList.append(updateFirmwareAct);
    hardware->addAction(updateFirmwareAct);
    updateFirmwareAct->setDisabled(true);

    //------------------------------------ Help
    QMenu *help = new QMenu("Help");
    help->setObjectName("HelpMenu");
    menubar->addMenu(help);

    //about
    about = new QAction("About 12 Step Editor", help);
    actionList.append(about);
    help->addAction(about);

    //doc
    doc = new QAction("Documentation...", help);
    actionList.append(doc);
    help->addAction(doc);
    help->addSeparator();

    //troubleshooter
    troubleShoot = new QAction("Open Connection Troubleshooter", help);
    actionList.append(troubleShoot);
    help->addAction(troubleShoot);
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
    }
    else
    {
        saveAsDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - SAVEASWIDGET_WIDTH/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - SAVEASWDIGET_HEIGHT/2, SAVEASWIDGET_WIDTH, SAVEASWDIGET_HEIGHT);
        deleteDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - DELETEWIDGET_WIDTH/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - DELETEWIDGET_HEIGHT/2, DELETEWIDGET_WIDTH, DELETEWIDGET_HEIGHT);
        aboutDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - ABOUTWIDGET_WIDTH/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - ABOUTWIDGET_HEIGHT/2, ABOUTWIDGET_WIDTH, ABOUTWIDGET_HEIGHT);
        importOldDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - importOldDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - importOldDialogWidget->height()/2, importOldDialogWidget->width(), importOldDialogWidget->height());
        importOldNotFoundDialogWidget->setGeometry(SETTINGSTAB_WIDTH/2 - importOldNotFoundDialogWidget->width()/2, SETTINGSTAB_HEIGHT/2 + TAB_Y_POS/2 - importOldNotFoundDialogWidget->height()/2, importOldNotFoundDialogWidget->width(), importOldNotFoundDialogWidget->height());
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
        ui->connected->setStyleSheet("QPushButton {background:transparent;font:18px 'Futura PT'; color:rgba(0, 174, 237, 175);border-radius:0px;}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242)};"
                                     );
#else
        ui->connected->setStyleSheet("QPushButton {background:transparent;font:18px 'Corbel'; color:rgba(0, 174, 237, 175);border-radius:0px;}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#endif
        ui->connected->setText("CONNECTED");
        ui->update->setEnabled(true);
        updateFirmwareAct->setDisabled(false);
//        sysexManager->slotSendFwQuery();
        aboutDialogForm->found->setText(deviceFirmwareVersionString());
        troubleshootWindow->slotConnected(true);
    }
    else
    {
#ifdef Q_OS_MAC
        ui->connected->setStyleSheet("QPushButton {background:transparent;font:18px 'Futura PT'; color:rgba(207, 0, 18, 175);border-radius:0px;}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#else
        ui->connected->setStyleSheet("background:transparent;font:18px 'Corbel'; color:rgba(207, 0, 18, 175);border-radius:0px;"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}"
                                     );
#endif
        ui->connected->setText("NOT CONNECTED");
        ui->update->setEnabled(false);
        updateFirmwareAct->setDisabled(true);
        aboutDialogForm->found->setText("Not Connected");
        troubleshootWindow->slotConnected(false);
    }
}

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

    if(TwelveStep->connected)
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
            settingsTab->midiThru_addItem(portName); // update dropdown

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

            troubleshootWindow->slotDetected();
            ui->connected->setText("Detected");
            ui->connected->setStyleSheet("QPushButton {border: 1px solid rgb(67,67,67); background:rgb(255,125,0); border-radius:0px;}"
                                         "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}");

            if (!TwelveStep->slotUpdatePortIn(portNum))
            {
                UserDialog portError((QString("ERROR: MIDI input port \"%1\"\nis currently being used by another program or process!").arg(portName)), {"Ok"});
                portError.exec();
            }
            else
            {
                fwUpdateWindow->slotAppendTextToConsole("\nTwelveStep Connected\n");
            }

        }
        else if ((portName == TWELVESTEP_OLD_OUT_P1 || portName == TWELVESTEP_OUT_P1) && inOrOut == PORT_OUT)
        {
            if (!TwelveStep->slotUpdatePortOut(portNum))
            {
                UserDialog portError((QString("ERROR: MIDI output port \"%1\"\nis currently being used by another program or process!").arg(portName)), {"Ok"});
                portError.exec();
            }
            else
            {
                TwelveStep->pollingStatus = true; // start polling when output port is added
            }
        }
        break;
    case PORT_DISCONNECT:

        if (inOrOut == PORT_OUT)
        {
            // update dropdown
            if (settingsTab->midiThru_currentText() == portName)
            {
                settingsTab->midiThru_setCurrentIndex(0);
            }

            // make sure this port is actually in the aux dropdown
            int findPortIndex = settingsTab->midiThru_findItem(portName);

            if (findPortIndex != -1)
            {
                settingsTab->midiThru_removeItem(findPortIndex);
            }

        }

        // **** TwelveStep disconnect **************************************
        if (inOrOut == PORT_IN && portName == TwelveStep->portName_in)
        {
            // close ports and stop polling
            TwelveStep->slotCloseMidiIn(SIGNAL_SEND);
            fwUpdateWindow->slotAppendTextToConsole("\nTwelveStep Disconnected\n");
            ui->connected->setText("Not Connected");
            ui->connected->setStyleSheet("QPushButton {border: 1px solid rgb(67,67,67); background: rgb(100,100,100); border-radius:0px;}"
                                         "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}");
        }
        else if (inOrOut == PORT_OUT && portName == TwelveStep->portName_out)
        {
            TwelveStep->slotCloseMidiOut(SIGNAL_SEND);
            TwelveStep->pollingStatus = false;
        }
        else if (inOrOut == PORT_OUT && (portName == TWELVESTEP_OUT_P1 || portName == TWELVESTEP_OLD_OUT_P1) )
        {
            troubleshootWindow->slotConnected(false);
            ui->connected->setText("Not Connected");
            ui->connected->setStyleSheet("QPushButton {border: 1px solid rgb(67,67,67); background: rgb(100,100,100); border-radius:0px;}"
                                         "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}");
        }
        break;
    case PORT_CHANGED:
        //qDebug() << " PORT CHANGED - name: " << portName << portName << " inOrOut: " << kmiPorts->inOut[inOrOut] << " messageType: " << kmiPorts->mType[messageType] << " portNum: " << portNum << "\n";

        // **** TwelveStep renumber ****************************************
        if ((portName == TWELVESTEP_OLD_IN_P1 || portName == TWELVESTEP_IN_P1) && inOrOut == PORT_IN)
        {
            TwelveStep->slotUpdatePortIn(portNum);
        }
        else if ((portName == TWELVESTEP_OLD_OUT_P1 || portName == TWELVESTEP_OUT_P1)&& inOrOut == PORT_OUT)
        {
            TwelveStep->slotUpdatePortOut(portNum);
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
    //TwelveStep->slotResetConnections(TWELVESTEP_OLD_IN_P1, TWELVESTEP_OLD_IN_P1);
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
        slotUpdateMIDIaux();
        slotShowConnection(true);
    }
    else
    {
        TwelveStep->slotFirmwareUpdateReset();
        slotShowConnection(false);
    }
    disableWidget->hide(); // re-enable app and "ungrey" main window
}

void MainWindow::slotForceFirmwareUpdate()
{
    if (!TwelveStep->port_out_open) return;

    slotFirmwareDetected(TwelveStep, false); // act as if we received a firmware mismatch
    forceFirmwareUpdate = true;
}

void MainWindow::slotFirmwareDetected(MidiDeviceManager *thisMDM, bool matches)
{
    qDebug() << "slotFirmwareDetected called";

    troubleshootWindow->slotSetDevVersion(deviceFirmwareVersionString(), deviceBootloaderVersionString());
    if (forceFirmwareUpdate)
    {
        troubleshootWindow->slotRequestFwUpdate();
        forceFirmwareUpdate = false;
    }

    if (matches)
    {
        qDebug() << "FirmwareMatch: " << thisMDM->PID << "name:" << thisMDM->deviceName;

        // EB TODO: this is a temporary cluge to make the editor work with old, non-bootloader firmware. Remove when fw1.0.0 is out
        TwelveStep->bootloaderMode = false;
    }
    else
    {
        qDebug() << "Firmware MisMatch: " << thisMDM->PID << "name:" << thisMDM->deviceName;

        ui->connected->setText("FIRMWARE OUT OF DATE");
        ui->connected->setStyleSheet("QPushButton {border: 1px solid rgb(67,67,67); background:rgb(255,0,0); border-radius:0px;}"
                                     "QToolTip {font: 10pt 'Futura'; color: rgb(242, 242, 242);}");


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

    sessionSettings->setValue(MIDI_AUX_KEY, settingsTab->midiThru_currentText()); // store this setting for the next time we run the editor

    if (settingsTab->midiThru_currentText() != "None")
    {
        qDebug() << "update aux port: " << settingsTab->midiThru_currentText();


        //recallMidiAuxPortName = ""; // reset this, which is only set when the stored aux port connects

        // set and open the ports
        int thisOutPort = kmiPorts->getOutPortNumber(settingsTab->midiThru_currentText());

        if (thisOutPort == -1)
        {
            qDebug() << "ERROR: Port \"" << settingsTab->midiThru_currentText() << "\" does not exist, cannot open as MIDI aux port";
            return;
        }

        midiTHRU->slotUpdatePortOut(thisOutPort);
        connect(TwelveStep, SIGNAL(signalRxMidi_raw(uchar,uchar,uchar,uchar)), midiTHRU, SLOT(slotSendMIDI(uchar,uchar,uchar,uchar)));
    }
    else
    {
        qDebug() << "close port, don't update";
        midiTHRU->slotCloseMidiOut(SIGNAL_NONE);
    }
}

void MainWindow::slotRecallMIDIaux()
{
    qDebug() << "slotRecallMIDIaux called, connected: " << connected << " recallMidiAuxPortName: " << recallMidiAuxPortName;
    if (!connected || recallMidiAuxPortName == "") return; // wait until connected to TwelveStep and the previously saved port

    settingsTab->midiThru_setCurrentText(recallMidiAuxPortName);
    slotUpdateMIDIaux();
}

void MainWindow::slotOpenTroubleshooting()
{
    troubleshootWindow->show();
    troubleshootWindow->slotScrollTroubleUp();
}

// --------------------------------------------------------------------------------------
// ------ end midi overhaul -------------------------------------------------------------
// --------------------------------------------------------------------------------------
