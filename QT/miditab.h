// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MIDITAB_H
#define MIDITAB_H

#include <QtGui>
#include <QButtonGroup>

#include "modlines.h"

#ifdef Q_OS_MAC
#include "ui_midiTabForm.h"
#else
#include "ui_midiTabFormWin.h"
#endif

namespace Ui {
class MidiTab;
}

class MidiTab : public QWidget
{
    Q_OBJECT

public:
    explicit MidiTab(QWidget *parent = 0);

    QWidget *midiTabFormWidget;

    Modlines *modline[NUMBER_OF_MODLINES];
    int numModlinesShowing;

    bool thisTabSelected;

    //button groups
    QButtonGroup keySafety;
    QButtonGroup noteMode;

    //current destination menu selections for each modline
    QStringList modlineDestinations;

    // midi thru dropdown
    QComboBox * midiThru;

signals:
    void signalStoreValue(QString name, QVariant value, int presetNum);
    void signalCheckSavedState();

    void signalSourceBOn(int val);
    void signalSourceBOff();

    void signalDeleteModline(int numModlinesShowing, int index);
    void signalWindowHeight(int numModlinesShowing);

    // update aux
    void signalUpdateMIDIaux();

public slots:
    void slotConnectElements();
    void slotDisconnectElements();

    void slotValueChanged();
    void slotChangeDestMenus(int modNum, QString destination);
    void slotRecallPreset(QVariantMap, QVariantMap);
    void slotRecallDestMenus(int modNum);

    void slotDisableSourceB();
    void slotEnableSourceB(int val);

    void slotTabView(int);

    //resizing functions for the add/subtract buttons
    void slotRecallShowingModlines(QVariantMap, QVariantMap);
    void slotAddOrSubtractModlines();
    void slotWindowHeight(int);

    // update aux
    void slotUpdateMIDIaux();

private:
    Ui::midiTabForm *midiTabForm;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MIDITAB_H
