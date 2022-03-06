// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SOFTSTEPTREEVIEW_H
#define SOFTSTEPTREEVIEW_H

#include <QTreeView>
#include <QFile>

#include "treemodel.h"

class SoftStepTreeView : public QTreeView
{
    Q_OBJECT
public:

    explicit SoftStepTreeView(const QStringList &arguments,QWidget *parent = 0);
    explicit SoftStepTreeView(QFile &file,QWidget *parent = 0);
    explicit SoftStepTreeView(const QVariant &qdata,QWidget *parent = 0);
    void splitSharedPrefixChildren();
    void setupFromData(const QVariant &data);
    void setupFromFile(QFile &file);
    TreeModel *model;
    QTreeView view;
    int initialized;

signals:

public slots:

};

#endif // SOFTSTEPTREEVIEW_H
