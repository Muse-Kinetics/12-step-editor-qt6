// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "modlines.h"

Modlines::Modlines(QWidget *parent, int modlineInstanceNum) :
    QWidget(parent),
    modlineFormWidget(new QWidget(this)),
    modlineForm(new Ui::modlineForm)
{
    modlineInstance = modlineInstanceNum;

    this->setObjectName(QString("modline%1").arg(modlineInstance+1));

    //--------------------------- set up Ui
    modlineForm->setupUi(modlineFormWidget);
    this->setFixedSize(MODLINE_WIDTH, MODLINE_HEIGHT);
    this->setGeometry(MODLINE_X_POS, MODLINE_Y_POS + modlineInstance * (MODLINE_HEIGHT + MODLINE_SPACING), MODLINE_WIDTH, MODLINE_HEIGHT);

    //first modline should be fixed at note and only velocities should be selectable - source a should also never be off
    if(modlineInstanceNum == 0)
    {
        modlineForm->destination->clear();
        modlineForm->destination->addItem("Note");
        modlineForm->destination->setEnabled(false);
        modlineForm->destination->setFixedWidth(120);
        modlineForm->cc_number->hide();
        modlineForm->source_a->clear();
        //modlineForm->source_a->addItem("Off");
        modlineForm->source_a->addItem("Velocity");
        modlineForm->source_a->addItem("Velocity (Inverted)");
        modlineForm->source_b->clear();
        modlineForm->source_b->addItem("Off");
        modlineForm->source_b->addItem("Velocity");
        modlineForm->source_b->addItem("Velocity (Inverted)");
    }
}

void Modlines::slotConnectElements()
{
    foreach(QWidget* widget, modlineFormWidget->findChildren<QWidget *>())
    {
        //check object type
        if(widget->metaObject()->className() == QString("SelectAllSpinBox"))
        {
            SelectAllSpinBox* spinbox = qobject_cast<SelectAllSpinBox *>(widget);
            connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
        }
        else if(widget->metaObject()->className() == QString("QDoubleSpinBox"))
        {
            QDoubleSpinBox* doublespinbox = qobject_cast<QDoubleSpinBox *>(widget);
            connect(doublespinbox, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
        }
        else if(widget->metaObject()->className() == QString("QComboBox"))
        {
            QComboBox* combobox = qobject_cast<QComboBox *>(widget);
            connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
        }
    }
}

void Modlines::slotDisconnectElements()
{
    foreach(QWidget* widget, modlineFormWidget->findChildren<QWidget *>())
    {
        //check object type
        if(widget->metaObject()->className() == QString("SelectAllSpinBox"))
        {
            SelectAllSpinBox* spinbox = qobject_cast<SelectAllSpinBox *>(widget);
            disconnect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
        }
        else if(widget->metaObject()->className() == QString("QDoubleSpinBox"))
        {
            QDoubleSpinBox* doublespinbox = qobject_cast<QDoubleSpinBox *>(widget);
            disconnect(doublespinbox, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
        }
        else if(widget->metaObject()->className() == QString("QComboBox"))
        {
            QComboBox* combobox = qobject_cast<QComboBox *>(widget);
            disconnect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
        }
    }
}

void Modlines::slotValueChanged()
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
        }
        else if(sender->metaObject()->className() == QString("QDoubleSpinBox"))
        {
            QDoubleSpinBox* doublespinbox = qobject_cast<QDoubleSpinBox *>(sender);
            value = doublespinbox->value();
        }
        else if(sender->metaObject()->className() == QString("QComboBox"))
        {
            QComboBox* combobox = qobject_cast<QComboBox *>(sender);
            value = combobox->currentText();
            if(combobox->objectName() == "destination")
            {
                if(value == "CC") //set the message type combobox width depending on whether CC type is selected
                {
                    modlineForm->destination->setFixedWidth(MODLINE_TYPE_WIDTH_SHORT);
                    modlineForm->cc_number->setEnabled(true);
                }
                else
                {
                    modlineForm->destination->setFixedWidth(MODLINE_TYPE_WIDTH_LONG);
                    modlineForm->cc_number->setEnabled(false);
                }

                emit signalUpdateDestinationsOnChange(modlineInstance, combobox->currentText());
            }
            else if(modlineInstance == 0 && combobox->objectName() == "source_b")
            {
                if(value == "Off")
                {
                    emit signalSourceBOff();
                }
                else
                {
                    emit signalSourceBOn(-1);
                }
            }
        }

        emit signalStoreValue(QString("modline%1_").arg(modlineInstance+1) + jsonName, value, -1);

        //----------- disable modline if necessary
        if((jsonName == "source_a" || jsonName == "source_b") && value != "Off")
        {
            emit signalModlineEnabled(QString("modline%1_%2").arg(modlineInstance+1).arg(jsonName));
        }
    }
    emit signalCheckSavedState();
}

void Modlines::slotRecallPreset(QVariantMap preset, QVariantMap)
{
    slotDisconnectElements();

    modlineForm->cc_number->setValue(preset.value(QString("modline%1_cc_number").arg(modlineInstance+1)).toInt());
    modlineForm->source_a->setCurrentIndex(modlineForm->source_a->findText(preset.value(QString("modline%1_source_a").arg(modlineInstance+1)).toString()));
    modlineForm->source_b->setCurrentIndex(modlineForm->source_b->findText(preset.value(QString("modline%1_source_b").arg(modlineInstance+1)).toString()));
    if(modlineInstance == 0 && modlineForm->source_b->currentText() == "Off")
    {
        sourceBColumnEnable = false;
        emit signalSourceBOff();
    }
    else if(modlineInstance == 0 && modlineForm->source_b->currentText() != "Off")
    {
        sourceBColumnEnable = true;
        emit signalSourceBOn(-1);
    }
    modlineForm->gain->setValue(preset.value(QString("modline%1_gain").arg(modlineInstance+1)).toDouble());
    modlineForm->offset->setValue(preset.value(QString("modline%1_offset").arg(modlineInstance+1)).toInt());
    modlineForm->table->setCurrentIndex(modlineForm->table->findText(preset.value(QString("modline%1_table").arg(modlineInstance+1)).toString()));
    modlineForm->min->setValue(preset.value(QString("modline%1_min").arg(modlineInstance+1)).toInt());
    modlineForm->max->setValue(preset.value(QString("modline%1_max").arg(modlineInstance+1)).toInt());

    slotConnectElements();
}

void Modlines::slotRecallDestinations(QStringList destMenuList, QString recallText)
{
    slotDisconnectElements();

    //----------------------- first populate the menus
    modlineForm->destination->clear();
    for(int i = 0; i < destMenuList.size(); i++)
    {
        modlineForm->destination->addItem(destMenuList.at(i));
    }

    //----------------------- then recall the menu text
    modlineForm->destination->setCurrentIndex(modlineForm->destination->findText(recallText));

    //set the message type combobox width depending on whether CC type is selected
    if(modlineForm->destination->currentText() == "CC")
    {
        modlineForm->destination->setFixedWidth(MODLINE_TYPE_WIDTH_SHORT);
        modlineForm->cc_number->setEnabled(true);
    }
    else
    {
        modlineForm->destination->setFixedWidth(MODLINE_TYPE_WIDTH_LONG);
        modlineForm->cc_number->setEnabled(false);
    }

    slotConnectElements();

    //set the source a menus to a value if the destination is turned on by slotValueChanged
    if(QObject::sender())
    {
        //qDebug() << "value changed does this";

        QString selectedDest = modlineForm->destination->currentText();

        if(selectedDest == "Bend" && modlineForm->source_a->currentText() == "Off" && modlineForm->source_b->currentText() == "Off")
        {
            modlineForm->source_a->setCurrentText("Tilt");
        }
        else if(selectedDest == "Off")
        {
            modlineForm->source_a->setCurrentText("Off");
            //modlineForm->source_a->setEnabled(false);
            modlineForm->source_b->setCurrentText("Off");
            //sourceBRowEnable = false;
            //modlineForm->source_b->setEnabled(false);
        }
        else if(modlineForm->source_a->currentText() == "Off" && modlineForm->source_b->currentText() == "Off")
        {
            modlineForm->source_a->setCurrentText("Pressure");
        }

        /*if(selectedDest != "Off")
        {
            modlineForm->source_a->setEnabled(true);
            sourceBRowEnable = true;
            if(sourceBColumnEnable && sourceBRowEnable)
            {
                modlineForm->source_b->setEnabled(true);
            }
        }*/
    }
    else
    {
        //qDebug() << "preset recall does this";
    }

    if(modlineForm->destination->currentText() == "Off")
    {
        modlineForm->source_a->setCurrentText("Off");
        modlineForm->source_a->setEnabled(false);
        sourceBRowEnable = false;
        modlineForm->source_b->setCurrentText("Off");
        modlineForm->source_b->setEnabled(false);
    }
    else
    {
        modlineForm->source_a->setEnabled(true);
        sourceBRowEnable = true;
        if(sourceBColumnEnable && sourceBRowEnable)
        {
            modlineForm->source_b->setEnabled(true);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Modline Showing/Hiding //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Modlines::slotHideModline(int num, int index)
{
    if(modlineInstance == num-1 && modlineInstance > 1)
    {
        modlineForm->destination->setCurrentIndex(index);
        emit signalStoreValue(QString("modline%1_destination").arg(modlineInstance+1), modlineForm->destination->currentText(), -1);
        emit signalCheckSavedState();
    }
}

void Modlines::slotDisableSourceB()
{
    modlineForm->source_b->setCurrentText("Off");
    if(modlineInstance != 0)
    {
        //qDebug() << "disable source b modline" << modlineInstance;
        sourceBColumnEnable = false;
        modlineForm->source_b->setEnabled(false);
    }
}
void Modlines::slotEnableSourceB(int)
{
    if(QObject::sender())
    {
        QObject *sender = QObject::sender();
        QString senderName = sender->objectName();

        if(senderName == "midiTab")
        {
            if(modlineInstance == 0 && modlineForm->source_b->currentText() == "Off")
            {
                modlineForm->source_b->setCurrentText("Velocity");
            }
        }
    }

    if(modlineInstance != 0)
    {
        sourceBColumnEnable = true;
        if(sourceBColumnEnable && sourceBRowEnable)
        {
            modlineForm->source_b->setEnabled(true);
        }
    }
}
