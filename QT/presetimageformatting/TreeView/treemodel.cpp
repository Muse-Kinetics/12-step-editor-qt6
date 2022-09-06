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

/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"


//! [0]
TreeModel::TreeModel(const QVariant &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "root1";
    rootItem = new TreeItem(rootData);
    QString rootTitle("root");
    setupModelData(data,rootItem,rootTitle,QString(""));
}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]

int debug_count = 0;
void stop_here(void)
{
}
void TreeModel::setupModelData(const QVariant &data,TreeItem *parent,QString &pre, QString key)
{
    Q_UNUSED(key);
    int preLen = pre.length();

    pre.append(" ");
//    pre.append(key);

    switch(data.typeId())
    {
        case QMetaType::QVariantMap:
           {
                QMapIterator<QString, QVariant> i(data.toMap());
                while (i.hasNext()) {
                     i.next();

                     QVariant newData(i.key());
                     //QVariant newChild = i.value();
 //                    showData(newData,"Map:");
                     TreeItem *newParent = new TreeItem(newData,parent);
                     parent->appendChild(newParent);
                    setupModelData(i.value(),newParent,pre,i.key());
                }
            }
            break;
    case QMetaType::QVariantList:
        {
            QList<QVariant> list = data.toList();
            QList<QVariant>::Iterator it = list.begin();
            while( it != list.end() ) {
                setupModelData( *it,parent,pre,QString(""));
                ++it;
            }
        }
        break;
    default:
#ifdef UNUSED
        switch(data.type())
        {

            case QVariant::String:qDebug() << pre << " " << data.toString();break;
            case QVariant::LongLong: qDebug() << pre << " " << data.toLongLong();break;
            case QVariant::ULongLong:qDebug() << pre << " " << data.toULongLong(); break;
            case QVariant::Double:qDebug() << pre << " " << data.toDouble();break;
            default:
            qDebug("unknown type[%d]",data.type());
            break;
        }
#endif
 //       showData(data,"endNode:");
        parent->appendChild(new TreeItem(data,parent));
        break;
    }

    pre.fill(QChar(' '),preLen);


}

void TreeModel::splitSharedPrefixChildren()
{
    rootItem->splitSharedPrefixChildren();
}

QList<TreeItem *> *TreeModel::childrenOf(TreeItem *root,QString p)
{
    if (root)
        return root->childrenOf(p);
    else
        return rootItem->childrenOf(p);
}
QString *TreeModel::childStringOf(TreeItem *root,QString p)
{
    if (root)
        return root->childStringOf(p);
    else
        return rootItem->childStringOf(p);
}
TreeItem *TreeModel::child(TreeItem *root,QString p)
{
    if (root)
        return root->child(p);
    else
        return rootItem->child(p);
}

void TreeModel::showData(const QVariant &data,QString title)
{
    switch(data.typeId())
    {
    case QMetaType::QVariantMap:
        qDebug("%s:Map",title.toLatin1().data());
        break;
    case QMetaType::QVariantList:
        qDebug("%s:List",title.toLatin1().data());
        break;
    default:
        switch(data.typeId())
        {

        case QMetaType::QString:qDebug() << title.toLatin1() << ":"  << data.toString();break;
        case QMetaType::LongLong: qDebug() << title.toLatin1() << ":" << data.toLongLong();break;
        case QMetaType::ULongLong:qDebug() << title.toLatin1() << ":" << data.toULongLong(); break;
        case QMetaType::Double:qDebug() << title.toLatin1() << ":" << data.toDouble();break;
            default:
            qDebug("%s:unknown type[%d]",title.toLatin1().data(),data.typeId());
            break;
        }

           break;
    }

}
void TreeModel::ssEmitValues()
{
    rootItem->ssEmitValues();
}
TreeItem *TreeModel::treeItemByType(int type){
    return rootItem->treeItemByType(type);

}
TreeItem *TreeModel::treeItemByTypeString(int type,QString string)
{
    return rootItem->treeItemByTypeString(type,string);
}
void TreeModel::backup()
{
    rootItem->backup();

}
void TreeModel::revert()
{
    rootItem->revert();
}
QVariant TreeModel::data()
{
    return rootItem->data("      ");
}
TreeItem *TreeModel::root()
{
    return rootItem;
}


