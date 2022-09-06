// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "keytab.h"

KeyTab::KeyTab(QWidget *parent) :
    QWidget(parent),
    keyTabFormWidget(new QWidget(this)),
    keyTabForm(new Ui::keyTabForm)
{
    //----------------------- set up Ui
    keyTabForm->setupUi(keyTabFormWidget);
    this->setGeometry(0, 0, KEYTAB_WIDTH, KEYTAB_HEIGHT);
    //set main stylesheet for this tab here - if it needs to be different from the stylesheet for all tabs - for that see mainwindow.cpp

    //construct the 12Step keyboard representation
    for(int i = 0; i < 5; i++)
    {
        keyNotes.append(-1); // initializing the list of notes as blank
    }
    currentKey = 0; //initializing the currentKey to 0 since the app will start with key 0 selected
    for(int i = 0; i < 13; i++)
    {
#ifdef Q_OS_MAC
#else
        QCoreApplication::processEvents();
#endif
        keyEdit[i] = new KeyEdit(keyTabFormWidget, i);
        keyEdit[i]->slotConnectElements();
        collectKeyNotes.append(keyNotes);
        connect(keyEdit[i], SIGNAL(signalSelectedKeyNum(int)), this, SLOT(slotReleaseSelectedKeys(int)));
        connect(keyEdit[i], SIGNAL(signalSelectedKeyNotes(int,int,int,int,int,int)), this, SLOT(slotSetKeyboardFromSelection(int,int,int,int,int,int)));
        connect(keyEdit[i], SIGNAL(signalChangeNoteSelection(int,int,int,int,int,int)), this, SLOT(slotSetKeyboardFromSelection(int,int,int,int,int,int)));
        connect(this, SIGNAL(signalSetSelectionsFromKeyboard(int,int,int,int,int,int)), keyEdit[i], SLOT(slotSelectKeysFromKeyboard(int,int,int,int,int,int)));
    }

    //construct the full keyboard toggles
    int blackKeyAdjustment = 0;
    for(int i = 0; i < KEYBOARD_TOTAL; i++)
    {
        if(i%12 == 1 || i%12 == 3 || i%12 == 6 || i%12 == 8 || i%12 == 10)  //if it's a black key
        {
            keyboardKey[i] = new QToolButton(keyTabFormWidget);
            keyboardKey[i]->setObjectName(QString("keyboardKey%1").arg(i));
            keyboardKey[i]->setStyleSheet("QToolButton{background:rgb(88,88,88);border:none;}"
                                          "QToolButton:checked{background:rgb(0,174,239);border:none;}"
                                          "QToolButton:focus{border:1px solid rgb(0,174,239)};");
            keyboardKey[i]->setCheckable(true);
            keyboardKey[i]->setGeometry(KEYBOARD_X_POS + ((KEYBOARD_KEY_WIDTH + KEYBOARD_SPACING) * (i-1)) + (KEYBOARD_KEY_WIDTH / 2) - (blackKeyAdjustment*(KEYBOARD_KEY_WIDTH + KEYBOARD_SPACING)),
                                        KEYBOARD_BLACK_Y_POS, KEYBOARD_KEY_WIDTH, KEYBOARD_KEY_HEIGHT);
            blackKeyAdjustment++;
            connect(keyboardKey[i], SIGNAL(toggled(bool)), this, SLOT(slotCountKeyboardSelections()));
            connect(keyboardKey[i], SIGNAL(clicked()), this, SLOT(slotSetSelectionsFromKeyboard()));
            keyboardKey[i]->setToolTip("Displays all notes set on the currently selected key. Active notes will appear blue.<p>Notes may be added or removed by clicking on the keys.");
        }
        else
        {
            keyboardKey[i] = new QToolButton(keyTabFormWidget);
            keyboardKey[i]->setObjectName(QString("keyboardKey%1").arg(i));
            keyboardKey[i]->setStyleSheet("QToolButton{background:white;border:none;}"
                                          "QToolButton:checked{background:rgb(0,174,239);border:none;}"
                                          "QToolButton:focus{border:1px solid rgb(0,174,239)};");
            keyboardKey[i]->setCheckable(true);
            keyboardKey[i]->setGeometry(KEYBOARD_X_POS + ((KEYBOARD_KEY_WIDTH + KEYBOARD_SPACING) * i) - (blackKeyAdjustment*(KEYBOARD_KEY_WIDTH + KEYBOARD_SPACING)),
                                        KEYBOARD_WHITE_Y_POS, KEYBOARD_KEY_WIDTH, KEYBOARD_KEY_HEIGHT);
            connect(keyboardKey[i], SIGNAL(toggled(bool)), this, SLOT(slotCountKeyboardSelections()));
            connect(keyboardKey[i], SIGNAL(clicked()), this, SLOT(slotSetSelectionsFromKeyboard()));
            keyboardKey[i]->setToolTip("Displays all notes set on the currently selected key. Active notes will appear blue.<p>Notes may be added or removed by clicking on the keys.");
        }
    }
    slotConnectButtons();
    slotUpdatePasteAvailability();
}

void KeyTab::slotConnectButtons()
{
    connect(keyTabForm->copyKeyButton, SIGNAL(clicked()), this, SLOT(slotCopyKey()));
    connect(keyTabForm->copyKeyButton, SIGNAL(clicked()), this, SLOT(slotUpdatePasteAvailability()));
    connect(keyTabForm->pasteKeyButton, SIGNAL(clicked()), this, SLOT(slotPasteKey()));
    connect(keyTabForm->clearKeyButton, SIGNAL(clicked()), this, SLOT(slotClearKey()));
    connect(keyTabForm->clearAllKeysButton, SIGNAL(clicked()), this, SLOT(slotClearAllKeys()));
    connect(keyTabForm->transposeUpButton, SIGNAL(clicked()), this, SLOT(slotTransposeUp()));
    connect(keyTabForm->transposeDownButton, SIGNAL(clicked()), this, SLOT(slotTransposeDown()));
}


void KeyTab::slotRecallPreset(QVariantMap, QVariantMap)
{
    //after preset recall, reinitialize everything to start over
    for(int i = 0; i < 13; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            collectKeyNotes[i].replace(j, -1); //don't just clear them, replace them with off states
        }
    }
    for(int i = 0; i < KEYBOARD_TOTAL; i++)
    {
        keyboardKey[i]->setCheckable(true);
        keyboardKey[i]->setChecked(false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Keyboard Selection Tracking ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KeyTab::slotReleaseSelectedKeys(int selectedKeyNum)
{
    currentKey = selectedKeyNum;
    for(int i = 0; i < 13; i++)
    {
        if(i != selectedKeyNum)
        {
            keyEdit[i]->slotDeselectKey();
        }
    }
}

void KeyTab::slotSetKeyboardFromSelection(int keyInstance, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive)
{
    //clear everything first
    collectKeyNotes[keyInstance].clear();
    for(int i = 0; i < KEYBOARD_TOTAL; i++)
    {
        keyboardKey[i]->setCheckable(true);
        keyboardKey[i]->setChecked(false);
    }

    collectKeyNotes[keyInstance].append(noteOne);
    collectKeyNotes[keyInstance].append(noteTwo);
    collectKeyNotes[keyInstance].append(noteThree);
    collectKeyNotes[keyInstance].append(noteFour);
    collectKeyNotes[keyInstance].append(noteFive);

    for(int i = 0; i < 128; i++)
    {
        if(collectKeyNotes[keyInstance].contains(i))
        {
            keyboardKey[i]->setChecked(true);
        }
        else
        {
            keyboardKey[i]->setChecked(false);
        }
    }
}

void KeyTab::slotCountKeyboardSelections()
{
    int numberOfToggledKeys = 0;
    for(int i = 0; i < KEYBOARD_TOTAL; i++)
    {
        if(keyboardKey[i]->isChecked())
        {
            numberOfToggledKeys++;
        }
    }

    if(numberOfToggledKeys > 4)
    {
        for(int i = 0; i < KEYBOARD_TOTAL; i++)
        {
            if(!keyboardKey[i]->isChecked())
            {
                keyboardKey[i]->setCheckable(false);
            }
        }
    }
    else
    {
        for(int i = 0; i < KEYBOARD_TOTAL; i++)
        {
            keyboardKey[i]->setCheckable(true);
        }
    }
}

void KeyTab::slotSetSelectionsFromKeyboard()
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();
        int keyNumber = sender->objectName().mid(11).toInt();
        bool toggled = this->findChild<QToolButton *>(sender->objectName())->isChecked();

        if(collectKeyNotes[currentKey].contains(keyNumber) && toggled == false)
        {
            int indexOfMatchingNote = collectKeyNotes[currentKey].indexOf(keyNumber);
            collectKeyNotes[currentKey].replace(indexOfMatchingNote,-1);
        }
        else if(!collectKeyNotes[currentKey].contains(keyNumber) && toggled == true)
        {
            bool done = false;
            for(int i = 0; i < 5; i++)
            {
                if(collectKeyNotes[currentKey].at(i) == -1 && done == false)
                {
                    collectKeyNotes[currentKey].replace(i,keyNumber);
                    done = true;
                }
            }
        }

        slotReorderKeyNoteCollection();
        int noteOne = collectKeyNotes[currentKey].at(0);
        int noteTwo = collectKeyNotes[currentKey].at(1);
        int noteThree = collectKeyNotes[currentKey].at(2);
        int noteFour = collectKeyNotes[currentKey].at(3);
        int noteFive = collectKeyNotes[currentKey].at(4);

        emit signalSetSelectionsFromKeyboard(currentKey, noteOne, noteTwo, noteThree, noteFour, noteFive);
    }
}

void KeyTab::slotReorderKeyNoteCollection()
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();

        QList<int> newOrder;
        newOrder.append(collectKeyNotes[currentKey].at(0));
        newOrder.append(collectKeyNotes[currentKey].at(1));
        newOrder.append(collectKeyNotes[currentKey].at(2));
        newOrder.append(collectKeyNotes[currentKey].at(3));
        newOrder.append(collectKeyNotes[currentKey].at(4));

        //instead of just clearing them all, replace their values with the off state
        for(int i = 0; i < 5; i++)
        {
            collectKeyNotes[currentKey].replace(i, -1);
        }
        int countNotes = 0;
        for(int i = 0; i < 5; i++)
        {
            if(newOrder.at(i) != -1)
            {
                collectKeyNotes[currentKey].replace(countNotes, newOrder.at(i));
                countNotes++;
            }
        }
        //qDebug() << sender->objectName();
        if(!sender->objectName().contains("keyboardKey"))
        {
            int noteOne = collectKeyNotes[currentKey].at(0);
            int noteTwo = collectKeyNotes[currentKey].at(1);
            int noteThree = collectKeyNotes[currentKey].at(2);
            int noteFour = collectKeyNotes[currentKey].at(3);
            int noteFive = collectKeyNotes[currentKey].at(4);

            emit signalSetSelectionsFromKeyboard(currentKey, noteOne, noteTwo, noteThree, noteFour, noteFive);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Extra Note Entry Functions ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KeyTab::slotCopyKey()
{
    copiedKeyNotes.clear();

    for(int i = 0; i < 5; i++)
    {
        copiedKeyNotes.append(collectKeyNotes[currentKey].at(i));
    }
}

void KeyTab::slotPasteKey()
{
    for(int i = 0; i < 5; i++)
    {
        collectKeyNotes[currentKey].replace(i, copiedKeyNotes.at(i));
    }

    int noteOne = collectKeyNotes[currentKey].at(0);
    int noteTwo = collectKeyNotes[currentKey].at(1);
    int noteThree = collectKeyNotes[currentKey].at(2);
    int noteFour = collectKeyNotes[currentKey].at(3);
    int noteFive = collectKeyNotes[currentKey].at(4);

    emit signalSetSelectionsFromKeyboard(currentKey, noteOne, noteTwo, noteThree, noteFour, noteFive);
}

void KeyTab::slotUpdatePasteAvailability()
{
    if(copiedKeyNotes.isEmpty())
    {
        keyTabForm->pasteKeyButton->setEnabled(false);
    }
    else
    {
        keyTabForm->pasteKeyButton->setEnabled(true);
    }
}

void KeyTab::slotClearKey()
{
    for(int i = 0; i < 5; i++)
    {
        collectKeyNotes[currentKey].replace(i, -1);
    }

    emit signalSetSelectionsFromKeyboard(currentKey, -1, -1, -1, -1, -1);
}

void KeyTab::slotClearAllKeys()
{
    for(int i = 0; i < 13; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            collectKeyNotes[i].replace(j, -1);
        }
        emit signalSetSelectionsFromKeyboard(i, -1, -1, -1, -1, -1);
    }
}

void KeyTab::slotTransposeUp()
{
    for(int i = 0; i < 5; i++)
    {
        int transpose = collectKeyNotes[currentKey].at(i) + 1;

        if(transpose - 1 != -1)
        {
            if(transpose > 127)
            {
                collectKeyNotes[currentKey].replace(i, 127);  // any notes that try to transpose out of the range are clipped here
            }
            else
            {
                collectKeyNotes[currentKey].replace(i, transpose);
            }
        }
    }

    int noteOne = collectKeyNotes[currentKey].at(0);
    int noteTwo = collectKeyNotes[currentKey].at(1);
    int noteThree = collectKeyNotes[currentKey].at(2);
    int noteFour = collectKeyNotes[currentKey].at(3);
    int noteFive = collectKeyNotes[currentKey].at(4);

    emit signalSetSelectionsFromKeyboard(currentKey, noteOne, noteTwo, noteThree, noteFour, noteFive);
}

void KeyTab::slotTransposeDown()
{
    for(int i = 0; i < 5; i++)
    {
        int transpose = collectKeyNotes[currentKey].at(i) - 1;
        if(collectKeyNotes[currentKey].at(i) != -1)
        {
            if(transpose < 0)
            {
                collectKeyNotes[currentKey].replace(i, 0);  // any notes that try to transpose out of the range are clipped here
            }
            else
            {
                collectKeyNotes[currentKey].replace(i, transpose);
            }
        }
    }

    int noteOne = collectKeyNotes[currentKey].at(0);
    int noteTwo = collectKeyNotes[currentKey].at(1);
    int noteThree = collectKeyNotes[currentKey].at(2);
    int noteFour = collectKeyNotes[currentKey].at(3);
    int noteFive = collectKeyNotes[currentKey].at(4);

    emit signalSetSelectionsFromKeyboard(currentKey, noteOne, noteTwo, noteThree, noteFour, noteFive);
}
