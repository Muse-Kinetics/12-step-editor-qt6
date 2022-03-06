// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef KEYTAB_H
#define KEYTAB_H

//#include <QWidget>
//#include <QDebug>
//#include <QtGui>
//#include <QVariant>
#include <QToolButton>

#include "keyedit.h"
//#include "definitions.h"

#ifdef Q_OS_MAC
#include "ui_keyTabForm.h"
#else
#include "ui_keyTabFormWin.h"
#endif

namespace Ui {
class KeyTab;
}

class KeyTab : public QWidget
{
    Q_OBJECT

public:
    explicit KeyTab(QWidget *parent = 0);

    QWidget *keyTabFormWidget;

    KeyEdit *keyEdit[13];
    QToolButton *keyboardKey[KEYBOARD_TOTAL];

    int currentKey;
    QList<int> keyNotes;
    QList<QList<int> > collectKeyNotes;
    QList<int> copiedKeyNotes;

signals:
    void signalSetSelectionsFromKeyboard(int selectedKey, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive);

public slots:
    void slotConnectButtons();

    void slotRecallPreset(QVariantMap, QVariantMap);

    //link full keyboard to 12Step representation keyboard
    void slotReleaseSelectedKeys(int selectedKeyNum);
    void slotSetKeyboardFromSelection(int keyInstance, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive);
    void slotCountKeyboardSelections();
    void slotSetSelectionsFromKeyboard();
    void slotReorderKeyNoteCollection();

    //extra note entry functions
    void slotCopyKey();
    void slotPasteKey();
    void slotUpdatePasteAvailability();
    void slotClearKey();
    void slotClearAllKeys();
    void slotTransposeUp();
    void slotTransposeDown();

private:
    Ui::keyTabForm *keyTabForm;
};

#endif // KEYTAB_H
