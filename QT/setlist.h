// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SETLIST_H
#define SETLIST_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QtGui>
#include <QVariant>

#include "definitions.h"

#ifdef Q_OS_MAC
#include "ui_setlistForm.h"
#else
#include "ui_setlistFormWin.h"
#endif

class Setlist : public QWidget
{
    Q_OBJECT

public:
    explicit Setlist(QWidget *parent = 0);

    QWidget* setlistWidget;

    QList<QComboBox *> menus;
    QList<QCheckBox *> checkBoxes;

    QString jsonPath;
    QFile *jsonFile;
    QVariantMap setlist;

    bool eventFilter(QObject *, QEvent *);

    bool repopulating;

    Ui::setlistForm *setlistForm;

signals:

    void signalSetlistDirty();

public slots:
    void slotInitComponents();
    void slotUpdateJSONPath();
    void slotReadSetlist();
    void slotRecallSetlist();
    void slotMenuChanged(int menuNum);
    void slotCompileSetlist();
    void slotWriteSetlist();
    void slotCleanUpSetlist();
    void slotPopulateSetlistMenus(QComboBox *presetMenu);

    void slotPopulateSetlistAfterDelete(QComboBox *presetMenu);
    void slotClearSetlist();
    void slotAutoPopulateSetlist(QComboBox *presetMenu);

//private:
//    Ui::setlistForm *setlistForm;
};

#endif // SETLIST_H
