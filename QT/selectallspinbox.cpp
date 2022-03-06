// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "selectallspinbox.h"

SelectAllSpinBox::SelectAllSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
    selectOnMousePress = false;

    QLineEdit *editor = this->findChild<QLineEdit *>("qt_spinbox_lineedit");
    editor->installEventFilter(this);
}

void SelectAllSpinBox::keyPressEvent(QKeyEvent *e)
{
    if(this->specialValueText() == "off" && e->key() == Qt::Key_O)
    {
        this->setValue(-1);
        this->selectAll();
    }
    else
    {
        QSpinBox::keyPressEvent(e);
    }
}

void SelectAllSpinBox::focusInEvent(QFocusEvent *e)
{
    QSpinBox::focusInEvent(e);
    selectAll();
    selectOnMousePress = true;
}

bool SelectAllSpinBox::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QMouseEvent::MouseButtonPress)
    {
        if(selectOnMousePress)
        {
            this->selectAll();
            selectOnMousePress = false;
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}
