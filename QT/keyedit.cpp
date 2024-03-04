// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "keyedit.h"

KeyEdit::KeyEdit(QWidget *parent, int keyInstanceNum) :
    QWidget(parent),
    keyEditFormWidget(new QWidget(this)),
    keyEditForm(new Ui::keyEditForm)
{
    previousKeyInstance = 0;
    keyInstance = keyInstanceNum;
    thisKeySelected = false;

    //object name should be in the same format as the json parameters
    if(keyInstance < 9)
    {
        this->setObjectName(QString("key0%1").arg(keyInstance+1));
    }
    else
    {
        this->setObjectName(QString("key%1").arg(keyInstance+1));
    }

    //-------------------------- set up Ui
    keyEditForm->setupUi(keyEditFormWidget);
    int blackKeyAdjustment = 0;
    if(keyInstance == 2 || keyInstance == 3)
    {
        blackKeyAdjustment = 1;
    }
    if(keyInstance == 4 || keyInstance == 5 || keyInstance == 6)
    {
        blackKeyAdjustment = 2;
    }
    else if(keyInstance == 7 || keyInstance == 8)
    {
        blackKeyAdjustment = 3;
    }
    else if(keyInstance == 9 || keyInstance == 10)
    {
        blackKeyAdjustment = 4;
    }
    else if(keyInstance > 10)
    {
        blackKeyAdjustment = 5;
    }

     // if it's a black key...
    if(keyInstance == 1 || keyInstance == 3 || keyInstance == 6 || keyInstance == 8 || keyInstance == 10)
    {
        this->setGeometry(KEYEDIT_X_POS + ((keyInstance - 1) * (KEYEDIT_WIDTH + KEYEDIT_SPACING)) + (KEYEDIT_WIDTH / 2) - (blackKeyAdjustment * (KEYEDIT_WIDTH + KEYEDIT_SPACING)),
                          KEYEDIT_BLACK_Y_POS, KEYEDIT_WIDTH, KEYEDIT_HEIGHT);
#ifdef Q_OS_MAC
        this->setStyleSheet("QSpinBox{font:9pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding-left: 2px}"
                            "QSpinBox:focus{border:none;outline:none;}"
                            "QLineEdit{font:9pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding:1px1px1px3px;}"
                            "QFrame{background-color:transparent;border:none;image:url(:/ui/ui_images/key_edit_background_black.svg)}"
                            );
#else
        this->setStyleSheet("QSpinBox{font:7pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding-left: 2px}"
                            "QSpinBox:focus{border:none;outline:none;}"
                            "QLineEdit{font:7pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding:1px1px1px3px;}"
                            "QFrame{background-color:transparent;border:none;image:url(:/ui/ui_images/key_edit_background_black.svg)}"
                            );
#endif
    }
    // if it's a white key...
    else
    {
        this->setGeometry(KEYEDIT_X_POS + (keyInstance * (KEYEDIT_WIDTH + KEYEDIT_SPACING)) - (blackKeyAdjustment * (KEYEDIT_WIDTH + KEYEDIT_SPACING)),
                          KEYEDIT_WHITE_Y_POS, KEYEDIT_WIDTH, KEYEDIT_HEIGHT);
#ifdef Q_OS_MAC
        this->setStyleSheet("QSpinBox{font:9pt'Droid Sans Mono';border:none;background-color:rgb(65,65,65);color:rgb(242, 242, 242);padding-left: 2px;}"
                            "QSpinBox:focus{border:none;outline:none;}"
                            "QLineEdit{font:9pt'Droid Sans Mono';border:none;background-color:rgb(65,65,65);color:rgb(242, 242, 242);padding:1px1px1px3px;}"
                            "QFrame{background-color:transparent;border:none;image:url(:/ui/ui_images/key_edit_background_white.svg)}"
                            );
#else
        this->setStyleSheet("QSpinBox{font:7pt'Droid Sans Mono';border:none;background-color:rgb(65,65,65);color:rgb(242, 242, 242);padding-left: 2px;}"
                            "QSpinBox:focus{border:none;outline:none;}"
                            "QLineEdit{font:7pt'Droid Sans Mono';border:none;background-color:rgb(65,65,65);color:rgb(242, 242, 242);padding:1px1px1px3px;}"
                            "QFrame{background-color:transparent;border:none;image:url(:/ui/ui_images/key_edit_background_white.svg)}"
                            );
#endif
    }

    slotPopulateKeyNameLists();

    if(keyInstance == 0)
    {
        slotSelectKey(); //key one should be selected upon initialization
    }

    keyEditForm->note1->installEventFilter(this);
    keyEditForm->note2->installEventFilter(this);
    keyEditForm->note3->installEventFilter(this);
    keyEditForm->note4->installEventFilter(this);
    keyEditForm->note5->installEventFilter(this);
    keyEditForm->lineEdit1->installEventFilter(this);
    keyEditForm->lineEdit2->installEventFilter(this);
    keyEditForm->lineEdit3->installEventFilter(this);
    keyEditForm->lineEdit4->installEventFilter(this);
    keyEditForm->lineEdit5->installEventFilter(this);
}

bool KeyEdit::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {
        slotSelectKey();

        return false;
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        slotSelectKey();

        if(obj->objectName().contains("lineEdit"))
        {
            //qDebug() << "you clicked:" << obj->objectName();
            QString matchingSpinBox = QString("note%1").arg(obj->objectName().at(8));

            this->findChild<QSpinBox *>(matchingSpinBox)->setFocus();
        }
        return false;
    }
    else
    {
        return QObject::eventFilter(obj,event);
    }
}

void KeyEdit::slotConnectElements()
{
    connect(keyEditForm->note1, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(keyEditForm->note2, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(keyEditForm->note3, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(keyEditForm->note4, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(keyEditForm->note5, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
}

void KeyEdit::slotDisconnectElements()
{
    disconnect(keyEditForm->note1, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(keyEditForm->note2, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(keyEditForm->note3, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(keyEditForm->note4, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    disconnect(keyEditForm->note5, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
}

void KeyEdit::slotValueChanged()
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
            //assign the correct note name to the line edit box
            QString matchingLineEdit = QString("lineEdit%1").arg(jsonName.at(4));
            this->findChild<QLineEdit *>(matchingLineEdit)->setText(slotSetKeyName(value.toInt()));
            slotChangeNoteSelection();
        }

        emit signalStoreValue(QString("%1_%2").arg(this->objectName()).arg(jsonName), value, -1);
    }
    emit signalCheckSavedState();
}

void KeyEdit::slotRecallPreset(QVariantMap preset, QVariantMap)
{
    slotDisconnectElements();

    keyEditForm->note1->setValue(preset.value(QString("%1_%2").arg(this->objectName()).arg(keyEditForm->note1->objectName())).toInt());
    keyEditForm->note2->setValue(preset.value(QString("%1_%2").arg(this->objectName()).arg(keyEditForm->note2->objectName())).toInt());
    keyEditForm->note3->setValue(preset.value(QString("%1_%2").arg(this->objectName()).arg(keyEditForm->note3->objectName())).toInt());
    keyEditForm->note4->setValue(preset.value(QString("%1_%2").arg(this->objectName()).arg(keyEditForm->note4->objectName())).toInt());
    keyEditForm->note5->setValue(preset.value(QString("%1_%2").arg(this->objectName()).arg(keyEditForm->note5->objectName())).toInt());

    keyEditForm->lineEdit1->setText(slotSetKeyName(keyEditForm->note1->value()));
    keyEditForm->lineEdit2->setText(slotSetKeyName(keyEditForm->note2->value()));
    keyEditForm->lineEdit3->setText(slotSetKeyName(keyEditForm->note3->value()));
    keyEditForm->lineEdit4->setText(slotSetKeyName(keyEditForm->note4->value()));
    keyEditForm->lineEdit5->setText(slotSetKeyName(keyEditForm->note5->value()));

    slotConnectElements();

    if(thisKeySelected)
    {
        slotSelectKey(); //re-select the key after preset recall
    }
}

void KeyEdit::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QPoint mousePressPoint = event->pos();

        if(mousePressPoint.x() < KEYEDIT_WIDTH && mousePressPoint.y() < KEYEDIT_HEIGHT)
        {
            slotSelectKey();
        }
    }
}

void KeyEdit::slotSelectKey()
{
    thisKeySelected = true;

    // set the currently selected key to blue
#ifdef Q_OS_MAC
    keyEditForm->frame->setStyleSheet("QSpinBox{font:9pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding-left: 2px}"
                                      "QSpinBox:focus{border:none;outline:none;}"
                                      "QLineEdit{font:9pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding:1px1px1px3px;}"
                                      "QFrame{background-color:transparent;border:none;image:url(:/ui/ui_images/key_edit_background_blue.svg)}"
                                      );
#else
    keyEditForm->frame->setStyleSheet("QSpinBox{font:7pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding-left: 2px}"
                                      "QSpinBox:focus{border:none;outline:none;}"
                                      "QLineEdit{font:7pt'Droid Sans Mono';border:none;background-color:rgb(242, 242, 242);color:rgb(65,65,65);padding:1px1px1px3px;}"
                                      "QFrame{background-color:transparent;border:none;image:url(:/ui/ui_images/key_edit_background_blue.svg)}"
                                      );
#endif
    emit signalSelectedKeyNum(keyInstance);

    int noteOne = keyEditForm->note1->value();
    int noteTwo = keyEditForm->note2->value();
    int noteThree = keyEditForm->note3->value();
    int noteFour = keyEditForm->note4->value();
    int noteFive = keyEditForm->note5->value();

    //qDebug() << QString("key %1 is selected: %2 %3 %4 %5 %6").arg(keyInstance).arg(noteOne).arg(noteTwo).arg(noteThree).arg(noteFour).arg(noteFive);
    emit signalSelectedKeyNotes(keyInstance, noteOne, noteTwo, noteThree, noteFour, noteFive);
}

void KeyEdit::slotDeselectKey()
{
    thisKeySelected = false;

    keyEditForm->frame->setStyleSheet("");
}

void KeyEdit::slotSelectKeysFromKeyboard(int selectedKey, int noteOne, int noteTwo, int noteThree, int noteFour, int noteFive)
{
    if(selectedKey == keyInstance)
    {
        keyEditForm->note1->setValue(noteOne);
        keyEditForm->note2->setValue(noteTwo);
        keyEditForm->note3->setValue(noteThree);
        keyEditForm->note4->setValue(noteFour);
        keyEditForm->note5->setValue(noteFive);
    }
}

void KeyEdit::slotChangeNoteSelection()
{
    int noteOne = keyEditForm->note1->value();
    int noteTwo = keyEditForm->note2->value();
    int noteThree = keyEditForm->note3->value();
    int noteFour = keyEditForm->note4->value();
    int noteFive = keyEditForm->note5->value();

    emit signalChangeNoteSelection(keyInstance, noteOne, noteTwo, noteThree, noteFour, noteFive);
}

QString KeyEdit::slotSetKeyName(int num)
{
    if(num == -1)
    {
        return "";
    }
    else
    {
        return QString("%1%2").arg(keys.at(num%12)).arg(octaves.at(num/12));
    }
}

void KeyEdit::slotPopulateKeyNameLists()
{
    keys.append("C");
    keys.append("C#");
    keys.append("D");
    keys.append("D#");
    keys.append("E");
    keys.append("F");
    keys.append("F#");
    keys.append("G");
    keys.append("G#");
    keys.append("A");
    keys.append("A#");
    keys.append("B");

    octaves.append("-2");
    octaves.append("-1");
    octaves.append("0");
    octaves.append("1");
    octaves.append("2");
    octaves.append("3");
    octaves.append("4");
    octaves.append("5");
    octaves.append("6");
    octaves.append("7");
    octaves.append("8");
}
