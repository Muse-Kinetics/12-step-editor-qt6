// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef KEYEDIT_H
#define KEYEDIT_H

#include <QWidget>
#include <QtGui>
#include <QDebug>
#include <QVariant>

#include "definitions.h"
#include "selectallspinbox.h"

#ifdef Q_OS_MAC
#include "ui_keyEditForm.h"
#else
#include "ui_keyEditFormWin.h"
#endif

class KeyEdit : public QWidget
{
    Q_OBJECT
public:
    explicit KeyEdit(QWidget *parent = 0, int keyInstanceNum = 0);

    QWidget *keyEditFormWidget;

    int keyInstance;
    int previousKeyInstance;

    //this list is for keeping the key names
    QStringList keys;
    QStringList octaves;

    bool thisKeySelected;

signals:
    void signalStoreValue(QString name, QVariant value, int presetNum);
    void signalCheckSavedState();

    void signalSelectedKeyNum(int selectedKeyInstance);
    void signalSelectedKeyNotes(int keyInstance, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive);
    void signalChangeNoteSelection(int keyInstance, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive);

public slots:
    void slotConnectElements();
    void slotDisconnectElements();
    void slotValueChanged();
    void slotRecallPreset(QVariantMap, QVariantMap);

    void slotSelectKey();
    void slotDeselectKey();

    //Key Number to Name display
    QString slotSetKeyName(int num);
    void slotPopulateKeyNameLists();

    //Link full keyboard to 12Step representation keyboard
    void slotSelectKeysFromKeyboard(int selectedKey, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive);
    void slotChangeNoteSelection();

protected:
    bool eventFilter(QObject *, QEvent *);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::keyEditForm *keyEditForm;
};

#endif // KEYEDIT_H
