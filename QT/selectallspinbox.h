// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SELECTALLSPINBOX_H
#define SELECTALLSPINBOX_H

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QDebug>
#include <QtGui>


class SelectAllSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SelectAllSpinBox(QWidget *parent = NULL);


signals:

public slots:

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    bool eventFilter(QObject *obj, QEvent *e);

    bool selectOnMousePress;

};

#endif // SELECTALLSPINBOX_H
