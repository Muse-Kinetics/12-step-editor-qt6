// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "settings.h"
#include "midi.h"

Settings::Settings(QWidget *parent, QSettings *_sessionSettings) :
    QWidget(parent),
    settingsWidget(new QWidget(this)),
    settingsForm(new Ui::settingsForm)

{
    sessionSettings = _sessionSettings;

    this->setObjectName("Settings");

    saveSettingsTimeout = new QTimer(this);
    connect(saveSettingsTimeout, SIGNAL(timeout()), this, SLOT(slotSaveSettingsTimeout()));
    saveSettingsTimeoutTime = 0;

    settingsForm->setupUi(settingsWidget);
    this->setGeometry(0, 0, SETTINGSTAB_WIDTH, SETTINGSTAB_HEIGHT);
    //set main stylesheet for this tab here - if it needs to be different from the stylesheet for all tabs - for that see mainwindow.cpp

    qDebug() << "Instantiate settings.cpp";
    midiThru = settingsForm->midiThruCombo;
    qDebug() << "settings - midiThru: " << midiThru->objectName();

    progChgRxCh = settingsForm->progchgRXchannel;

    slotConnectElements();
    slotUpdateLabeLValues();
}

void Settings::slotEnableUIfor12S2(bool is12s2)
{
    _is12s2 = is12s2;
    if (is12s2)
    {
        settingsForm->backlightBrightness->setDisabled(false);
        settingsForm->backlightBrightness->setStyleSheet("");
    }
    else // 12s1, disable unused features
    {
        settingsForm->backlightBrightness->setDisabled(true);
        settingsForm->backlightBrightness->setValue(29);
        settingsForm->backlightBrightness->setStyleSheet("QSlider::sub-page:horizontal{background: rgb(170, 170, 170) }"
                                                         "QSlider::add-page:horizontal{background: rgb(170, 170, 170)}"
                                                         "QSlider::handle:horizontal{background: rgb(0,0,0); border: none; width: 0px;}");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// SAVING & RECALLING //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Settings::slotConnectElements()
{
    connect(settingsForm->globalSensitivity, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(settingsForm->selectSensitivity, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(settingsForm->backlightBrightness, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(settingsForm->progchgRXchannel, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));

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
    disconnect(settingsForm->backlightBrightness, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(settingsForm->progchgRXchannel, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));

    disconnect(settingsForm->resetGlobal, SIGNAL(clicked()), this, SLOT(slotResetGlobalSensitivity()));
    disconnect(settingsForm->resetSelect, SIGNAL(clicked()), this, SLOT(slotResetSelectSensitivity()));

    disconnect(settingsForm->midiVolume, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged()));
    disconnect(settingsForm->velocityOverride, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged()));
}

void Settings::slotUpdateLabeLValues()
{
    double sensitivity = settingsForm->globalSensitivity->value() / 5.0;
    double selectTime = (settingsForm->selectSensitivity->value() * 50.0) / 1000.0;
    double brightnessFloat = double(settingsForm->backlightBrightness->value() / 29.0) * 100.0;
    int brightness;

    if (_is12s2)
    {
        brightness = (int)brightnessFloat; // round % to a whole number
    }
    else
    {
        brightness = 100; // only show 100% for 12Step1
    }

    qDebug() << "settingsTab slotUpdateLabeLValues called - brightness: " << brightness;

    settingsForm->label_key_sensitivity->setText(QString("GLOBAL KEY SENSITIVITY - %1\%").arg(sensitivity));
    settingsForm->label_select->setText(QString("SELECT BUTTON HOLD TIME - %1 Seconds").arg(selectTime));
    settingsForm->label_backlight_brightness->setText(QString("BACKLIGHT BRIGHTNESS - %1\%").arg(brightness));
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
            if (jsonName == "globalSensitivity")
            {
                double gain = slider->value() * 0.01;
                value = gain;
            }
            else
            {
                value = slider->value();
            }



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
        //comboboxes
        else if(senderClass == "QComboBox")
        {
            QComboBox *comboBox = reinterpret_cast<QComboBox *>(QObject::sender());
            jsonName = comboBox->objectName();
            value = comboBox->currentIndex();

            if (jsonName == "progchgRXchannel" && comboBox->currentText() == "Disabled")
            {
                value = -1;
            }
        }

        qDebug() << jsonName << " value changed: " << value;
        emit signalStoreValue(jsonName, value);
    }

    //emit signalSettingsDirty();
    slotUpdateLabeLValues();
}

void Settings::slotRecallPreset(QVariantMap preset, QVariantMap)
{
    slotDisconnectElements();

    foreach (QWidget *widget, settingsWidget->findChildren<QWidget *>())
    {
        QString objectName = widget->objectName();

        //check object type here
        if(widget->metaObject()->className() == QString("QSlider"))
        {
            QSlider *slider = qobject_cast<QSlider *>(widget);

            int value;
            if (objectName == "globalSensitivity")
            {
                value = preset.value(objectName).toDouble() * 100;
            }
            else
            {
                value = preset.value(objectName).toUInt();
            }

            slider->setValue(value);
            qDebug() << "settings update from slotRecallPreset - objectName: " << objectName << " value: " << value;
        }
        else if(widget->metaObject()->className() == QString("QCheckBox"))
        {
            QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget);
            checkBox->setChecked(preset.value(objectName).toBool());
        }
        else if(widget->metaObject()->className() == QString("QComboBox"))
        {
            QComboBox *comboBox = qobject_cast<QComboBox *>(widget);
            int value = preset.value(objectName).toInt();

            if (objectName == "progchgRXchannel")
            {
                if (value == -1)
                {
                    comboBox->setCurrentText("Disabled");
                }
                else
                {
                    comboBox->setCurrentIndex(value);
                }
            }
        }
    }

    slotConnectElements();
    slotUpdateLabeLValues();
}

void Settings::slotSaveSettingsTimeout()
{
    saveSettingsTimeoutTime++;

    //if x ms have elapsed since last value was changed
    if(saveSettingsTimeoutTime > 10)
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
    settingsForm->selectSensitivity->setValue(10);
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

int Settings::midiThru_findItem(QString portName)
{
   return midiThru->findText(portName);
}
