// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>

#include <QList>
#include <QMap>
#include <QVariant>

enum {ssTYPE_NONE,ssTYPE_SLOT_PARENT,ssTYPE_SLOT_CHILD,ssTYPE_SLOT_NAME,ssTYPE_PAD_PARENT,ssTYPE_IDHEAD,ssTYPE_ID1,ssTYPE_ID2};

//! [0]
class TreeItem : public QObject
{
    Q_OBJECT

public:
    explicit TreeItem(const QVariant &data, TreeItem *parent = 0);
    explicit TreeItem(TreeItem *item, TreeItem *parent);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parent();
    void splitSharedPrefixChildren();
    QList<TreeItem *> *childrenOf(QString p);
    QString *childStringOf(QString p);
    QString toString(bool &ok);
    TreeItem *child(QString p);
    void dirty(int padNum,int state);
    void dirtyCheck();
    void clean(int padNum);
    void backup();
    void revert();
    QVariant data(QString pre);
    QString showData(QString title);
    QString showData(QVariant data,QString title);

    void ssConnect();
    void ssEmitValue();
    void ssEmitValues();
    void ssIdentify(QList<QVariant> &list);
    void setAccessInt(int *ptr) { m_accessInt = ptr;}
    void setAccessDouble(double *ptr) {m_accessDouble = ptr;}
    TreeItem *treeItemByType(int type);
    TreeItem *treeItemByTypeString(int type,QString string);
    TreeItem *treeItemByString(QString string);
    TreeItem *treeItemByStringChild(QString string);
    QStringList toStringList(QList<QVariant> list);
    void setParentItem(TreeItem *newParentItem) { parentItem = newParentItem;}
    int ssType;
    int ssInt1,ssInt2;
    int *m_accessInt;
    double *m_accessDouble;
//    bool modified;  // bool type here causes severe bug in msvc2008.  Change to int.
    int modified;
    static QString ssSlotNum;
    static QString ssSlotName;
    QList<TreeItem*> childItems;
    QVariant *itemData;
    TreeItem *parentItem;

private:
    QVariant itemDataBackup;

public slots:
    void slotToggle();
    void slotSetText(QString text);
    void slotSetInt(int newVal);
    void slotSetDouble(double newVal);

signals:
    void sigValueString(QString);
    void sigStringList(QList<QString>);
    void sigSceneListMap(QMap<int,QString> map);
    void sigValueInt(int value);
    void sigValueDouble(double value);
    void sigPadDirty(int,int);
};
//! [0]

#endif
