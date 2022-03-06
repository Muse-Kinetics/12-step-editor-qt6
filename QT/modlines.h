// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef MODLINES_H
#define MODLINES_H

#include <QWidget>
#include <QDebug>
#include <QVariant>

#include "definitions.h"
#include "selectallspinbox.h"

#ifdef Q_OS_MAC
#include "ui_modlineForm.h"
#else
#include "ui_modlineFormWin.h"
#endif //Q_OS_MAC

class Modlines : public QWidget
{
    Q_OBJECT
public:
    explicit Modlines(QWidget *parent = 0, int modlineInstanceNum = 0);

    QWidget* modlineFormWidget;

    int modlineInstance;

    bool sourceBColumnEnable;
    bool sourceBRowEnable;

signals:
    void signalStoreValue(QString name, QVariant value, int presetNum);
    void signalCheckSavedState();
    void signalModlineEnabled(QString parameterName);
    void signalUpdateDestinationsOnChange(int modlineInstance, QString selectedDest);

    void signalSourceBOff();
    void signalSourceBOn(int);

public slots:
    void slotConnectElements();
    void slotDisconnectElements();
    void slotValueChanged();
    void slotRecallPreset(QVariantMap, QVariantMap);
    void slotRecallDestinations(QStringList destMenuList, QString recallText);

    void slotHideModline(int num, int index);

    void slotDisableSourceB();
    void slotEnableSourceB(int);

private:
    Ui::modlineForm* modlineForm;
};

#endif // MODLINES_H
