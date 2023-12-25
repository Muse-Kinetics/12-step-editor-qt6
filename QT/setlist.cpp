// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "setlist.h"

Setlist::Setlist(QWidget *parent, QSettings *_sessionSettings) :
    QWidget(parent),
    setlistForm(new Ui::setlistForm),
    setlistWidget(new QWidget(this))
{
    setlistForm->setupUi(setlistWidget);
    this->setGeometry(0, 0, SETLISTTAB_WIDTH, SETLISTTAB_HEIGHT);
    //set main stylesheet for this tab here - if it needs to be different from the stylesheet for all tabs - for that see mainwindow.cpp

    sessionSettings = _sessionSettings;
    slotUpdateJSONPath();
    slotReadSetlist();
    slotInitComponents();

    repopulating = false;
}

void Setlist::slotInitComponents()
{
    foreach(QWidget *widget, setlistWidget->findChildren<QWidget *>())
    {
        if(widget->objectName().contains("enable"))
        {
            QCheckBox *checkBox = reinterpret_cast<QCheckBox *>(widget);
            checkBoxes.append(checkBox);
            checkBox->installEventFilter(this);
        }
        else if(widget->objectName().contains("menu"))
        {
            QComboBox *comboBox = reinterpret_cast<QComboBox *>(widget);
            connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMenuChanged(int)));
        }
    }

    //order the menus in a list of comboboxes
    for(int i = 0; i < SETLIST_MENU_TOTAL; i++)
    {
        QString matchObjectName = QString("setlistmenu%1").arg(i);
        menus.append(this->findChild<QComboBox *>(matchObjectName));
    }
}

void Setlist::slotUpdateJSONPath()
{
    jsonPath = sessionSettings->value("PRESET_DIR").toString(); //get bundle path
    jsonPath.append("/setlist.json");
    qDebug() << "setlist.json path: " << jsonPath;
}

void Setlist::slotReadSetlist()
{
    //load json into QFile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QByteArray setlistByteArray = jsonFile->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(setlistByteArray);
        setlist = jsonDoc.toVariant().toMap();
    }
    else
    {
        qDebug() << "Setlist JSON: " << jsonPath;
        qDebug() << jsonFile->errorString();
        qFatal("Setlist Not Found");
    }

    jsonFile->close();
}

void Setlist::slotRecallSetlist()
{
    foreach (QComboBox* combobox, setlistWidget->findChildren<QComboBox *>())
    {
        disconnect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMenuChanged(int)));

        QString comboboxName = combobox->objectName();
        QString menuNum = comboboxName.mid(11);
        combobox->setCurrentIndex(combobox->findText(setlist.value(QString("%1").arg(menuNum)).toString()));

        connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMenuChanged(int)));
    }
    foreach (QCheckBox* checkbox, setlistWidget->findChildren<QCheckBox *>())
    {
        int matchingNum = checkbox->objectName().remove("enable").toInt();
        QString matchingName = QString("setlistmenu%1").arg(matchingNum);
        if(this->findChild<QComboBox *>(matchingName)->currentIndex() > 0)
        {
            checkbox->setChecked(true);
        }
        else
        {
            checkbox->setChecked(false);
        }
    }
}

void Setlist::slotPopulateSetlistAfterDelete(QComboBox *presetMenu)
{
    foreach (QComboBox *combobox, setlistWidget->findChildren<QComboBox *>())
    {
        disconnect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMenuChanged(int)));

        if(combobox->currentText() != "[EMPTY]")
        {
            bool itemExists = false;

            for(int i = 0; i < presetMenu->count(); i++)
            {
                if(presetMenu->itemText(i) == combobox->currentText())
                {
                    itemExists = true;
                }
            }

            if(!itemExists)
            {
                combobox->setCurrentText("[EMPTY]");
            }
        }

        connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMenuChanged(int)));
    }
}

void Setlist::slotMenuChanged(int menuNum)
{
    Q_UNUSED(menuNum);
    QComboBox *menu = (QComboBox*)QObject::sender();
    int i = menu->objectName().remove("setlistmenu").toInt();
    QCheckBox *checkBox = setlistWidget->findChild<QCheckBox *>(QString("enable%1").arg(i));

    //qDebug() << "setlist menu changed" << QString("%1").arg(i) << menu->currentText();

    if(menu->currentIndex() == 0)
    {
        checkBox->setChecked(false);
    }
    else
    {
        checkBox->setChecked(true);
    }

    if(!repopulating)
    {
        slotCompileSetlist();
    }
}

void Setlist::slotCompileSetlist()
{
    //clears the setlist read from json
    setlist.clear();

    //iterate through the setlist window's menus
    for(int i = 0; i < menus.size(); i++)
    {
        //compiles setlist from contents of setlist window menus
        setlist.insert(QString("%1").arg(i), menus.at(i)->currentText());
    }
    slotWriteSetlist();
}

void Setlist::slotWriteSetlist()
{
    //qDebug() << "slotWriteSetlist called";

    //load json into qfile
    QFile *jsonFile = new QFile(jsonPath);

    if(jsonFile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //serialize json, write to file
        QJsonDocument jsonDoc = QJsonDocument::fromVariant(setlist);
        QByteArray ba = jsonDoc.toJson();

        jsonFile->resize(0);
        jsonFile->write(ba);
    }
    else
    {
        qDebug() << "Setlist not found on write";
    }
    jsonFile->close();

    emit signalSetlistDirty();
}

void Setlist::slotCleanUpSetlist()
{
    QStringList setlistItems;

    //collect setlist items in order, add them to a list, then clear each menu
    for(int i = 0; i < 64; i++)
    {
        QComboBox *menu = this->findChild<QComboBox *>(QString("setlistmenu%1").arg(i));
        if(menu->currentText() != "[EMPTY]")
        {
            setlistItems.append(menu->currentText());
            menu->setCurrentIndex(0); //clear the item initially after adding the text to the list
        }
    }

    //repopulate the setlist from the list so there are no gaps
    for(int i = 0; i < setlistItems.size(); i++)
    {
        QComboBox *menu = this->findChild<QComboBox *>(QString("setlistmenu%1").arg(i));
        menu->setCurrentText(setlistItems.at(i));
    }
}

void Setlist::slotPopulateSetlistMenus(QComboBox *presetMenu)
{
    //add items to setlist menu

    repopulating = true;

    //iterate through menus
    for(int m = 0; m < menus.size(); m++)
    {
        //clear current menu
        menus.at(m)->clear();

        //populate off item
        menus.at(m)->addItem("[EMPTY]");

        for(int i = 0; i <presetMenu->count(); i++)
        {
            menus.at(m)->addItem(presetMenu->itemText(i), 0);
        }
    }
    repopulating = false;
}

void Setlist::slotClearSetlist()
{
    foreach (QComboBox *combobox, setlistWidget->findChildren<QComboBox *>())
    {
        combobox->setCurrentText("[EMPTY]");
    }
}

void Setlist::slotAutoPopulateSetlist(QComboBox *presetMenu)
{
    int countTo;

    //if the preset menu has less that 64 (the number of setlist slots), then we want to only iterate through the number of preset menu items
    //this prevents crashing if we're trying to iterate through more slots than their are of either the setlist comboboxes or the preset menu items
    if(presetMenu->count() <= 64)
    {
        countTo = presetMenu->count();
    }
    else
    {
        countTo = 64;
    }

    for(int i = 0; i < countTo; i++)
    {
        QString setlistItemText = presetMenu->itemText(i);
        QComboBox *menu = setlistWidget->findChild<QComboBox *>(QString("setlistmenu%1").arg(i));

        menu->setCurrentText(setlistItemText);
    }
}

bool Setlist::eventFilter(QObject *obj, QEvent *event)
{
    if((event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) && obj->objectName().contains("enable"))
    {
        QCheckBox *checkBox = (QCheckBox*)obj;
        int i = checkBox->objectName().remove("enable").toInt();

        QComboBox *menu = setlistWidget->findChild<QComboBox *>(QString("setlistmenu%1").arg(i));

        if(checkBox->isChecked())
        {
            menu->setCurrentIndex(0);
            checkBox->setChecked(false);
        }
        return true;
    }
    return false;
}
