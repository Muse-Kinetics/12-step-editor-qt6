// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// -------------------------------------- MainWindow
//initial window width is KEYTAB_WIDTH + TAB_X_POS*2 -- key tab is the first and initial tab
//initial window height is KEYTAB_HEIGHT + TAB_Y_POS + MAINWINDOW_BOTTOM_SPACING -- key tab is the first and initial tab

#define MAINWINDOW_BOTTOM_SPACING   8  //the amount of space under the tab area -- it makes sense to make this the same as the TAB_X_POS -- but I wanted to make it changeable separately just in case

#define TAB_X_POS                   15 //this amount will add the same amount of space to the right so the tab is centered

#ifdef Q_OS_MAC
#define TAB_Y_POS                   130
#else
#define TAB_Y_POS                   145
#endif

//dialogs
#define SAVEASWDIGET_HEIGHT         123
#define SAVEASWIDGET_WIDTH          150
#define DELETEWIDGET_HEIGHT         74
#define DELETEWIDGET_WIDTH          316
#define ABOUTWIDGET_HEIGHT          200
#define ABOUTWIDGET_WIDTH           300

//fw query
//#define CURRENT_FW_QUERY_RESPONSE   "28 41 76 60 64 104 71 71 0 0 50 56 "
//#define FW_VERSION                  "28"


// -------------------------------------- Keyboard Tab
#define KEYTAB_WIDTH                745 //I recommend making these 2 values match the keyTabForm.ui dimentions
#define KEYTAB_HEIGHT               500 // was 525

//full keyboard placement
#define KEYBOARD_TOTAL              128 //total number of keys that are created
#define KEYBOARD_KEY_WIDTH          7
#define KEYBOARD_KEY_HEIGHT         50
#define KEYBOARD_SPACING            2   //blank space between adjacent keys
#define KEYBOARD_X_POS              25
#define KEYBOARD_BLACK_Y_POS        325
#define KEYBOARD_WHITE_Y_POS        380


// -------------------------------------- 12Step Key Representations
#define KEYEDIT_WIDTH               58  //I recommend making these 2 values match the frame dimentions within the keyEditForm.ui
#define KEYEDIT_HEIGHT              133

//12Step key representation placement
#define KEYEDIT_SPACING             6   //space between adjacent keys
#define KEYEDIT_X_POS               50
#define KEYEDIT_BLACK_Y_POS         20
#define KEYEDIT_WHITE_Y_POS         165


// -------------------------------------- MIDI Tab
//positioning the tab and window heights
#define MIDITAB_WIDTH               745 //I recommend making these 2 values match the largest possible dimentions of the midiTabForm.ui (all modlines showing)
#define MIDITAB_HEIGHT              540

//positioning the add/delete modline buttons
#define ADDMODLINE_X_POS            23
#define DELMODLINE_X_POS            51
#define ADDDELETE_SQ_PXLS           22 //I'm assuming these will be perfect squares
#define ADDDELETE_SPACING           7  //refers to spacing between the bottom modline and the buttons


// -------------------------------------- Modlines
#define NUMBER_OF_MODLINES          6

#define MODLINE_WIDTH               686
#define MODLINE_HEIGHT              26
#define MODLINE_X_POS               23
#define MODLINE_Y_POS               235 //the MIDITAB_HEIGHT often has to be adjusted with this parameter -- if anything appears cut off after building
#define MODLINE_SPACING             10

//message type has dynamic width for selecting CC type and showing the CC number box
#define MODLINE_TYPE_WIDTH_SHORT    70
#define MODLINE_TYPE_WIDTH_LONG     120


// -------------------------------------- Setlist
#define SETLISTTAB_WIDTH            745
#ifdef Q_OS_MAC
#define SETLISTTAB_HEIGHT           545
#else
#define SETLISTTAB_HEIGHT           525
#endif
#define SETLIST_MENU_TOTAL          64


// -------------------------------------- Settings
#define SETTINGSTAB_WIDTH           745
#define SETTINGSTAB_HEIGHT          300


#endif // DEFINITIONS_H
