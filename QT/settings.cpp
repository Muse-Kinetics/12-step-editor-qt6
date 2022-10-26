// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    settingsForm(new Ui::settingsForm),
    settingsWidget(new QWidget(this))
{
    saveSettingsTimeout = new QTimer(this);
    this->setObjectName("Settings");
    connect(saveSettingsTimeout, SIGNAL(timeout()), this, SLOT(slotSaveSettingsTimeout()));
    saveSettingsTimeoutTime = 0;

    settingsForm->setupUi(settingsWidget);
    this->setGeometry(0, 0, SETTINGSTAB_WIDTH, SETTINGSTAB_HEIGHT);
    //set main stylesheet for this tab here - if it needs to be different from the stylesheet for all tabs - for that see mainwindow.cpp

    qDebug() << "Instantiace settings.cpp";
    midiThru = settingsForm->midiThruCombo;
    qDebug() << "settings - midiThru: " << midiThru->objectName();


    slotConnectElements();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// SAVING & RECALLING //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Settings::slotConnectElements()
{
    connect(settingsForm->globalSensitivity, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(settingsForm->selectSensitivity, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));

    connect(settingsForm->resetGlobal, SIGNAL(clicked()), this, SLOT(slotResetGlobalSensitivity()));
    connect(settingsForm->resetSelect, SIGNAL(clicked()), this, SLOT(slotResetSelectSensitivity()));

    connect(settingsForm->midiVolume, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged()));
    connect(settingsForm->velocityOverride, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged()));

    // MIDI Thru dropdown
    connect(midiThru, SIGNAL(activated(int)), this, SIGNAL(signalUpdateMIDIaux()));
}

void Settings::slotDisconnectElements()
{
    disconnect(settingsForm->globalSensitivity, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(settingsForm->selectSensitivity, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));

    disconnect(settingsForm->resetGlobal, SIGNAL(clicked()), this, SLOT(slotResetGlobalSensitivity()));
    disconnect(settingsForm->resetSelect, SIGNAL(clicked()), this, SLOT(slotResetSelectSensitivity()));

    disconnect(settingsForm->midiVolume, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged()));
    disconnect(settingsForm->velocityOverride, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged()));
}

void Settings::slotValueChanged()
{
    if(!saveSettingsTimeout->isActive())
    {
        saveSettingsTimeout->start(1);
    }

    saveSettingsTimeoutTime = 0;

    //emit values to the preset file here
    if(QObject::sender() && !QObject::sender()->objectName().startsWith("qt_") && QObject::sender()->objectName().size())
    {
        QObject *sender = QObject::sender();
        QString senderClass = sender->metaObject()->className();
        QString jsonName;
        QVariant value;

        //sliders
        if(senderClass == "QSlider")
        {
            QSlider *slider = reinterpret_cast<QSlider *>(QObject::sender());
            jsonName = slider->objectName();
            double gain = slider->value() * 0.01;
            value = gain;

            qDebug() << jsonName << " value changed: " << value;
        }
        //checkboxes
        else if(senderClass == "QCheckBox")
        {
            QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(QObject::sender());
            jsonName = checkBox->objectName();
            if(checkBox->isChecked())
            {
                checkBox->setText("ENABLED");
                value = 1;
            }
            else
            {
                checkBox->setText("DISABLED");
                value = 0;
            }
        }
        emit signalStoreValue(jsonName, value);
    }

    emit signalSettingsDirty();
}

void Settings::slotRecallPreset(QVariantMap preset, QVariantMap)
{
    slotDisconnectElements();

    foreach (QWidget *widget, settingsWidget->findChildren<QWidget *>())
    {
        //check object type here
        if(widget->metaObject()->className() == QString("QSlider"))
        {
            QSlider *slider = qobject_cast<QSlider *>(widget);
            QString objectName = widget->objectName();
            int gain = preset.value(objectName).toDouble() * 100;
            slider->setValue(gain);
        }
        else if(widget->metaObject()->className() == QString("QCheckBox"))
        {
            QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget);
            QString objectName = widget->objectName();
            checkBox->setChecked(preset.value(objectName).toBool());
        }
    }

    slotConnectElements();
}

void Settings::slotSaveSettingsTimeout()
{
    saveSettingsTimeoutTime++;

    //if 0.5s have elapsed since last value was changed
    if(saveSettingsTimeoutTime > 500)
    {
        //save settings
        //slotWriteSettings();
        emit signalWriteSettings();

        saveSettingsTimeout->stop();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// RESET BUTTONS ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Settings::slotResetGlobalSensitivity()
{
    settingsForm->globalSensitivity->setValue(100);
}

void Settings::slotResetSelectSensitivity()
{
    settingsForm->selectSensitivity->setValue(100);
}

// wrappers for midiThru - overwrought workaround for access violation on windows

QString Settings::midiThru_currentText()
{
    return midiThru->currentText();
}

void Settings::midiThru_setCurrentIndex(int index)
{
    midiThru->setCurrentIndex(index);
}

void Settings::midiThru_setCurrentText(QString portName)
{
    midiThru->setCurrentText(portName);
}

void Settings::midiThru_addItem(QString portName)
{
    midiThru->addItem(portName);
}

void Settings::midiThru_removeItem(int index)
{
    midiThru->removeItem(index);
}
