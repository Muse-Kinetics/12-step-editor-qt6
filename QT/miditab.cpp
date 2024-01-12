// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "miditab.h"
#include <QApplication>
#include <QLabel>
#include "globalVars.h"

MidiTab::MidiTab(QWidget *parent) :
    QWidget(parent),
    midiTabFormWidget(new QWidget(this)),
        midiTabForm(new Ui::midiTabForm)
{
    //---------------------------- set up Ui
    midiTabForm->setupUi(midiTabFormWidget);
    this->setGeometry(0, 0, MIDITAB_WIDTH, MIDITAB_HEIGHT);
    this->setObjectName("midiTab");
    //set main stylesheet for this tab here - if it needs to be different from the stylesheet for all tabs - for that see mainwindow.cpp

    //construct modlines within the tab widget
    for(int i = 0; i < NUMBER_OF_MODLINES; i++)
    {
#ifdef Q_OS_MAC
#else
        //QCoreApplication::processEvents();
#endif
        modline[i] = new Modlines(midiTabFormWidget, i);
        modline[i]->slotConnectElements();
    }

    thisTabSelected = false;

    //connect the add and subtract modline buttons
    connect(midiTabForm->addmodline, SIGNAL(clicked()), this, SLOT(slotAddOrSubtractModlines()));
    connect(midiTabForm->deletemodline, SIGNAL(clicked()), this, SLOT(slotAddOrSubtractModlines()));

    //source b enabling and disabling
    for(int i = 0; i < NUMBER_OF_MODLINES; i++)
    {
        connect(modline[0], SIGNAL(signalSourceBOff()), modline[i], SLOT(slotDisableSourceB()));
        connect(modline[0], SIGNAL(signalSourceBOn(int)), modline[i], SLOT(slotEnableSourceB(int)));
        connect(this, SIGNAL(signalSourceBOn(int)), modline[i], SLOT(slotEnableSourceB(int)));
        connect(this, SIGNAL(signalSourceBOff()), modline[i], SLOT(slotDisableSourceB()));
    }
    connect(modline[0], SIGNAL(signalSourceBOff()), this, SLOT(slotDisableSourceB()));
    connect(modline[0], SIGNAL(signalSourceBOn(int)), this, SLOT(slotEnableSourceB(int)));


    //connect the destination signals/slots
    for(int i = 1; i < NUMBER_OF_MODLINES; i++) // modline 0 should be hard set to "Note" so its destination menu shouldn't repopulate -- that's why I skipped it here
    {
        connect(modline[i], SIGNAL(signalUpdateDestinationsOnChange(int,QString)), this, SLOT(slotChangeDestMenus(int,QString)));
    }

    foreach (SelectAllSpinBox *spinbox, this->findChildren<SelectAllSpinBox *>())
    {
        spinbox->installEventFilter(this);
    }  
}

bool MidiTab::eventFilter(QObject *obj, QEvent *event)
{
    //this event filter makes it so you can delete the data from a spinbox, then press enter or leave focus and it will either turn off or revert to default

    SelectAllSpinBox *spinbox = qobject_cast<SelectAllSpinBox *>(obj);

    //find the "off" or default value for each spinbox
    int offValue;
    if(spinbox->objectName().contains("channel") || spinbox->objectName().contains("transpose") || spinbox->objectName().contains("offset"))
    {
        offValue = 0; //the voice a channel can't go to 0 but it will go to 1 instead which is fine
    }
    else if(spinbox->objectName().contains("bend_range"))
    {
        offValue = 2;
    }
    else
    {
        offValue = -1;
    }

    if(event->type() == QEvent::FocusOut && spinbox->text().isEmpty())
    {
        spinbox->setValue(offValue);
        qDebug() << "focus out" << spinbox->objectName();
    }
    else if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *me = (QKeyEvent*)event;

        if(me->key() == 16777220 && spinbox->text().isEmpty())
        {
            spinbox->setValue(offValue);
            qDebug() << "enter pressed" << spinbox->objectName();
        }
    }

    return QObject::eventFilter(obj,event);
}

void MidiTab::slotTabView(int tabIndex)
{
    if(tabIndex == 1)
    {
        thisTabSelected = true;
    }
    else
    {
        thisTabSelected = false;
    }
}

void MidiTab::slotEnableUIfor12S2(bool is12s2)
{
    if (is12s2)
    {
        midiTabForm->settings_cv1->setDisabled(false);
        midiTabForm->settings_cv2->setDisabled(false);
    }
    else // 12s1, disable unused features
    {
        midiTabForm->settings_cv1->setDisabled(true);
        midiTabForm->settings_cv2->setDisabled(true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// Saving & Recalling /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MidiTab::slotConnectElements()
{
    //voice a
    connect(midiTabForm->voice_a_bank, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_a_bend_range, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_a_channel, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_a_programchange, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_a_transpose, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));

    //voice b
    connect(midiTabForm->voice_b_bank, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_b_bend_range, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_b_channel, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_b_programchange, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->voice_b_transpose, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));

    //settings
    connect(midiTabForm->settings_cv1, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->settings_cv2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->settings_key_safety_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    connect(midiTabForm->settings_note_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
}

void MidiTab::slotDisconnectElements()
{
    //voice a
    disconnect(midiTabForm->voice_a_bank, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_a_bend_range, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_a_channel, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_a_programchange, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_a_transpose, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));

    //voice b
    disconnect(midiTabForm->voice_b_bank, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_b_bend_range, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_b_channel, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_b_programchange, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->voice_b_transpose, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));

    //settings
    disconnect(midiTabForm->settings_cv1, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->settings_cv2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->settings_key_safety_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(midiTabForm->settings_note_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
}

void MidiTab::slotValueChanged()
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();
        QString jsonName = sender->objectName();
        QVariant value;

        if(sender->metaObject()->className() == QString("SelectAllSpinBox"))
        {
            SelectAllSpinBox* spinbox = qobject_cast<SelectAllSpinBox *>(sender);
            value = spinbox->value();

            if(spinbox->objectName() == "voice_b_channel")
            {
                if(value.toInt() == 0)
                {
                    emit signalSourceBOff();
                }
                else
                {
                    emit signalSourceBOn(value.toInt());
                }
            }
        }
        else if(sender->metaObject()->className() == QString("QComboBox"))
        {

            QComboBox* comboBox = qobject_cast<QComboBox *>(sender);
            value = comboBox->currentText(); // something something everything in json needs to be human text readable and we never use enums grumble grumble
            qDebug() << "MidiTab combobox: " << comboBox << " value: " << value;
        }
        emit signalStoreValue(jsonName, value, -1);
    }
    emit signalCheckSavedState();
}

void MidiTab::slotRecallPreset(QVariantMap preset, QVariantMap)
{
    slotDisconnectElements();

    //voice a
    midiTabForm->voice_a_bank->setValue(preset.value(midiTabForm->voice_a_bank->objectName()).toInt());
    midiTabForm->voice_a_bend_range->setValue(preset.value(midiTabForm->voice_a_bend_range->objectName()).toInt());
    midiTabForm->voice_a_channel->setValue(preset.value(midiTabForm->voice_a_channel->objectName()).toInt());
    midiTabForm->voice_a_programchange->setValue(preset.value(midiTabForm->voice_a_programchange->objectName()).toInt());
    midiTabForm->voice_a_transpose->setValue(preset.value(midiTabForm->voice_a_transpose->objectName()).toInt());

    //voice b
    midiTabForm->voice_b_bank->setValue(preset.value(midiTabForm->voice_b_bank->objectName()).toInt());
    midiTabForm->voice_b_bend_range->setValue(preset.value(midiTabForm->voice_b_bend_range->objectName()).toInt());
    midiTabForm->voice_b_channel->setValue(preset.value(midiTabForm->voice_b_channel->objectName()).toInt());
    midiTabForm->voice_b_programchange->setValue(preset.value(midiTabForm->voice_b_programchange->objectName()).toInt());
    midiTabForm->voice_b_transpose->setValue(preset.value(midiTabForm->voice_b_transpose->objectName()).toInt());

    //cv
    QString cv1 = preset.value("settings_cv1", "Default (Gate)").toString();
    midiTabForm->settings_cv1->setCurrentText(cv1);

    QString cv2 = preset.value("settings_cv2", "Default (Pitch)").toString();
    midiTabForm->settings_cv2->setCurrentText(cv2);

    //note mode
    QString noteMode = preset.value("settings_note_mode").toString();
    midiTabForm->settings_note_mode->setCurrentText(noteMode);

    //key safety
    QString keySafety = preset.value("settings_key_safety_mode").toString();

    qDebug() << "Recall preset - settings_key_safety_mode json value: " << keySafety;
    // fix old values
    if(keySafety == "SingleKey")
    {
        keySafety = "Single Key";
    }
    if(keySafety == "MultiKey")
    {
        keySafety = "Multi Key";
    }
    qDebug() << "Recall preset - settings_key_safety_mode fixed value: " << keySafety;

    midiTabForm->settings_key_safety_mode->setCurrentText(keySafety);

    slotConnectElements();


    //----------------------------- recall destination menus for all the modlines
    modlineDestinations.clear();

    for(int i = 1; i < NUMBER_OF_MODLINES; i++) //started at 1 to skip modline 1 -- its dest menu is hard coded
    {
        //get the recall text for each modline
        modlineDestinations.append(preset.value(QString("modline%1_destination").arg(i+1)).toString());
    }

    for(int i = 1; i < NUMBER_OF_MODLINES; i++) //started at 1 to skip modline 1 -- its dest menu is hard coded
    {
        slotRecallDestMenus(i);
    }

}

void MidiTab::slotDisableSourceB()
{
    midiTabForm->voice_b_channel->setValue(0);
}
void MidiTab::slotEnableSourceB(int val)
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();
        QString senderName = sender->objectName();

        if(senderName.contains("modline"))
        {
            if(midiTabForm->voice_b_channel->value() == 0)
            {
                midiTabForm->voice_b_channel->setValue(2);
                //qDebug() << "set channel to" << val << "from" << senderName;
            }
        }
        else
        {
            midiTabForm->voice_b_channel->setValue(val);
            //qDebug() << "set channel to" << val << "from" << senderName;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Modline Showing/Hiding ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MidiTab::slotWindowHeight(int modlinesShowing)
{
    numModlinesShowing = modlinesShowing;

    //this variable represets the amount of space that the modlines take up
    int modlineSpace = (MODLINE_HEIGHT + MODLINE_SPACING) * modlinesShowing;

    this->setFixedHeight(MIDITAB_HEIGHT - ((MODLINE_HEIGHT + MODLINE_SPACING) * (NUMBER_OF_MODLINES - modlinesShowing)));
    midiTabForm->addmodline->setGeometry(ADDMODLINE_X_POS, MODLINE_Y_POS + modlineSpace + ADDDELETE_SPACING, ADDDELETE_SQ_PXLS, ADDDELETE_SQ_PXLS);
    midiTabForm->deletemodline->setGeometry(DELMODLINE_X_POS, MODLINE_Y_POS + modlineSpace + ADDDELETE_SPACING, ADDDELETE_SQ_PXLS, ADDDELETE_SQ_PXLS);
    for(int i = 1; i < NUMBER_OF_MODLINES; i++)
    {
        if(modlinesShowing > i)
        {
            modline[i]->show();
        }
        else
        {
            modline[i]->hide();
        }
    }
}

void MidiTab::slotRecallShowingModlines(QVariantMap preset, QVariantMap)
{
    numModlinesShowing = 2;

    //first determine how many modlines should be showing based on which preset is recalled
    for(int i = 0; i < NUMBER_OF_MODLINES; i++)
    {
        QString modlineEnabledA = preset.value(QString("modline%1_source_a").arg(i+1)).toString();
        QString modlineEnabledB = preset.value(QString("modline%1_source_b").arg(i+1)).toString();

        if(i > 1 && (modlineEnabledA != "Off" || modlineEnabledB != "Off"))
        {
            numModlinesShowing = i+1;
        }
    }
    if(thisTabSelected == true)
    {
        slotWindowHeight(numModlinesShowing);
        emit signalWindowHeight(numModlinesShowing);
    }
    //qDebug() << QString("show %1 modlines").arg(numModlinesShowing);
}

void MidiTab::slotAddOrSubtractModlines()
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();

        if(sender == midiTabForm->addmodline)
        {
            numModlinesShowing++;
        }
        else if(sender == midiTabForm->deletemodline)
        {
            emit signalDeleteModline(numModlinesShowing, 0);
            numModlinesShowing--;
        }
    }

    if(numModlinesShowing > NUMBER_OF_MODLINES)
    {
        numModlinesShowing = NUMBER_OF_MODLINES;
    }
    else if(numModlinesShowing < 2)
    {
        numModlinesShowing = 2;
    }
    slotWindowHeight(numModlinesShowing);
    emit signalWindowHeight(numModlinesShowing);
    //qDebug() << QString("show %1 modlines").arg(numModlinesShowing);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Destination Menu Populations ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MidiTab::slotChangeDestMenus(int modNum, QString destination)
{
    modlineDestinations.replace(modNum-1, destination);

    for(int i = 1; i < NUMBER_OF_MODLINES; i++)
    {
        slotRecallDestMenus(i);
    }
}

void MidiTab::slotRecallDestMenus(int modNum)
{
    QStringList destMenuList;
    int recallTextIndex = modNum-1;

    //make up the list for this Modline
    QString recallText = modlineDestinations.at(recallTextIndex);

    destMenuList.append("Off");
    //handle Bend item
    if(recallText == "Bend")
    {
        destMenuList.append("Bend");
    }
    else if(!modlineDestinations.contains("Bend"))
    {
        destMenuList.append("Bend");
    }
    //handle CC items
    if(recallText == "CC")
    {
        destMenuList.append("CC");
    }
    else
    {
        int countCCs = 0;

        for(int i = 0; i < modlineDestinations.size(); i++)
        {
            if(modlineDestinations.at(i) == "CC")
            {
                countCCs++; //count ccs if the *other* dest menus are set to "CC"
            }
        }

        if(countCCs < 2)
        {
            destMenuList.append("CC");
        }
    }
    //handle Channel Pressure item
    if(recallText == "Channel Pressure")
    {
        destMenuList.append("Channel Pressure");
    }
    else if(!modlineDestinations.contains("Channel Pressure"))
    {
        destMenuList.append("Channel Pressure");
    }
    //handle Poly Aftertouch Item
    if(recallText == "Poly Aftertouch")
    {
        destMenuList.append("Poly Aftertouch");
    }
    else if(!modlineDestinations.contains("Poly Aftertouch"))
    {
        destMenuList.append("Poly Aftertouch");
    }

    modline[modNum]->slotRecallDestinations(destMenuList, recallText);
}
