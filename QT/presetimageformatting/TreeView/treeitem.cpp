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
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include <QStringList>

#include "treeitem.h"

//! [0]
TreeItem::TreeItem(const QVariant &data, TreeItem *parent) :
    QObject(0)

{
    setParentItem(parent);
    ssType = ssTYPE_NONE;

//    qDebug("%s",showData(data,"").toLatin1().data());

    itemData = new QVariant(data);
    ssInt1 = ssInt2 = 0;
    m_accessInt = 0;
    m_accessDouble = 0;

    modified = false;
}
TreeItem::TreeItem(TreeItem *item, TreeItem *parent) :
    QObject(0)
{
    setParentItem(parent);
    itemData = new QVariant(*item->itemData);
    itemDataBackup = item->itemDataBackup;
    ssType = item->ssType;
    ssInt1 = item->ssInt1;
    ssInt2 = item->ssInt2;
    m_accessInt = item->m_accessInt;
    m_accessDouble = item->m_accessDouble;
    modified = item->modified;

//    qDebug("copy TreeItem: %s",showData("").toLatin1().data());

    QList<TreeItem*>::iterator i;
    for (i = item->childItems.begin(); i != item->childItems.end(); ++i)
        childItems.append(new TreeItem(*i,this));


}

//! [0]

//! [1]
TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
    delete itemData;
}
//! [1]

//! [2]
void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}
//! [2]

//! [3]
TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}
//! [3]

//! [4]
int TreeItem::childCount() const
{
    return childItems.count();
}
//! [4]

//! [5]
int TreeItem::columnCount() const
{
//    return itemData->count();
    return 1;
}
//! [5]

//! [6]
QVariant TreeItem::data(int column) const
{
    Q_UNUSED(column);
    return *itemData;
//    return itemData->value(column);
}
//! [6]

//! [7]
TreeItem *TreeItem::parent()
{
    return parentItem;
}
//! [7]

//! [8]
int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
//! [8]




void TreeItem::splitSharedPrefixChildren()
{
    int i,r;
    for (i=0;i<childItems.count();i++)
    {
        TreeItem *it = childItems.at(i);
        switch(it->itemData->type())
        {
            default:
            break;
            case QVariant::String:
            {
                 QString str = it->itemData->toString();
                int len = str.indexOf(QString("::"));
                if (len!=-1)
                {
                    len+=2;
                    if (len!=str.length())
                    {
                         QString leftStr = str.left(len);
                        TreeItem *newTI = new TreeItem(QVariant(leftStr),this);
                        childItems.insert(i,newTI);
                        for(r=i+1;r<childItems.count();)
                        {
                            TreeItem *nextTI = childItems.at(r);
                            if (nextTI->itemData->type() == QVariant::String)
                            {
                                QString rstr = nextTI->itemData->toString();
                                if (rstr.startsWith(leftStr))
                                {
                                    QString rightStr = rstr.right(rstr.length()-len);
                                    delete nextTI->itemData;
                                    nextTI->itemData = new QVariant(rightStr);
                                    nextTI->parentItem = newTI;
                                    newTI->appendChild(nextTI);
                                    childItems.removeAt(r);
                                } else
                                    r++;

                            }
                        }
                    }
                }
                break;
            }
         }
        childItems.at(i)->splitSharedPrefixChildren();
    }

}

QList<TreeItem *> *TreeItem::childrenOf(QString p)
{
    QList<TreeItem *> *list = 0;

    int i;
    for (i=0;i<childItems.count();i++)
    {
        TreeItem *it = childItems.at(i);

        switch(it->itemData->type())
        {
            default:
                break;
            case QVariant::String:
            {
                QString str = it->itemData->toString();
                if (str==p)
                    return &it->childItems;
                break;
            }

        }
        list = it->childrenOf(p);
        if (list)
            return list;
    }
    return list;
}
QString *TreeItem::childStringOf(QString p)
{
    QList<TreeItem *> *list = childrenOf(p);
    if (list && list->count() == 1)
    {
            TreeItem *item = list->at(0);
            if (item->childCount()==0)
            {
                if (item->itemData->type() == QVariant::String)
                {
                    return new QString(item->itemData->toString());// returning new, someone must delete it.
                }
            }

    }
    return 0;
}
TreeItem *TreeItem::child(QString p)
{
    int i;

    if (itemData->type() == QVariant::String)
    {
        QString s = itemData->toString();
        if (s==p)
            return this;
    }
    for (i=0;i<childCount();i++)
    {
        TreeItem *results = childItems.at(i)->child(p);
        if (results)
            return results;
    }
    return 0;

}

QString TreeItem::toString(bool &ok)
{
    if (itemData->type() != QVariant::String)
    {
        ok = false;
        return QString("");
    }

    ok = true;

    return QString(itemData->toString());
}

QString TreeItem::ssSlotNum;
QString TreeItem::ssSlotName;

void TreeItem::ssConnect()
{

}
QStringList TreeItem::toStringList(QList<QVariant> list)
{
    QStringList sList;

    QList<QVariant>::iterator i;

    for (i=list.begin();i!=list.end();i++)
    {
        switch((*i).type())
        {
            case QVariant::String:
                sList.append((*i).toString());
                break;
            case QVariant::Int:
                {
                    QString sval;
                    sval.sprintf("%d",(*i).toInt());
                    sList.append(sval);
                }
                break;
        case QVariant::LongLong:
        {
            QString sval;

            sval.sprintf("%ld",(long int) itemData->toLongLong());
            sList.append(sval);
        }
            break;
        case QVariant::ULongLong:
        {
            QString sval;

            sval.sprintf("%ld",(long int) itemData->toULongLong());
            sList.append(sval);
        }
            break;
        case QVariant::Double:
        {
            QString sval;
            sval.sprintf("%f",itemData->toDouble());
            sList.append(sval);
        }
            break;
            default:
                break;

        }
    }
    return sList;
}

void TreeItem::ssEmitValue()
{
    switch(ssType)
    {
        case ssTYPE_IDHEAD:
    {
        QMap<int, QString> map;
        QStringList list;
        int i,j;

        for (i=0;i<childCount();i++)
        {
            QList<QVariant> idList;
            ssSlotNum = "";
            ssSlotName = "";
            childItems.at(i)->ssIdentify(idList);

            QStringList sList = toStringList(idList);

            qDebug("%d: %s",sList.at(0).toInt(),sList.join(" ").toLatin1().data());

            map.insert(sList.at(0).toInt(),sList.join(" "));// put into a map so we can get back sorted list

  //          qDebug("ssEmitValues: ssTYPE_SLOT_PARENT found [%s %s]",ssSlotNum.toLatin1().data(),ssSlotName.toLatin1().data());
        }
        emit sigStringList(map.values());
        emit sigSceneListMap(map);
    }
    break;
    case ssTYPE_NONE:
        switch(itemData->type())
        {
            case QVariant::String:
            {
//                qDebug("Str[%s]",itemData->toString().toLatin1().data());
                emit sigValueString(itemData->toString());
            }
            break;

            case QVariant::LongLong:
//            qDebug("LongLong[%ld]",(long int) itemData->toLongLong());
            if(m_accessInt)
                *m_accessInt = itemData->toLongLong();
                emit sigValueInt((int) itemData->toLongLong());
                break;

            case QVariant::ULongLong:
//            qDebug("uLongLong[%ld]",(long int) itemData->toULongLong());
            if (m_accessInt)
                *m_accessInt = itemData->toULongLong();
                emit sigValueInt(itemData->toULongLong());
                break;
            case QVariant::Double:
//            qDebug("double[%f]",itemData->toDouble());
            if (m_accessDouble)
                *m_accessDouble = itemData->toDouble();
                emit sigValueDouble(itemData->toDouble());
                break;

            case QVariant::Int:
            {
//                qDebug("Int[%d]",itemData->toInt());
            if (m_accessInt)
                *m_accessInt = itemData->toInt();
                emit sigValueInt(itemData->toInt());
            }
            break;
        case QVariant::List:
            break;

            default:
            qDebug("unknown qvariant type[%d]",itemData->type());
            break;
        }
    }

}

void TreeItem::ssEmitValues()
{
    int i;

    ssEmitValue();

    for (i=0;i<childCount();i++)
        childItems.at(i)->ssEmitValues();

}
void TreeItem::ssIdentify(QList<QVariant> &ssID)
{
    int i;

    if (ssType == ssTYPE_ID1 || ssType == ssTYPE_ID2)
        ssID.append(*itemData);

    for (i=0;i<childCount();i++)
        childItems.at(i)->ssIdentify(ssID);

}
TreeItem *TreeItem::treeItemByType(int type)
{
    TreeItem *item = 0;
    int i;

    if (ssType==type)
        return this;


    for (i=0;i<childCount();i++)
    {
        item = childItems.at(i)->treeItemByType(type);
        if (item)
            break;
    }

    return item;
}
TreeItem *TreeItem::treeItemByTypeString(int type,QString string)
{
    TreeItem *item = 0;
    int i;

    if (ssType==type && itemData->toString()==string)
        return this;


    for (i=0;i<childCount();i++)
    {
        item = childItems.at(i)->treeItemByTypeString(type,string);
        if (item)
            break;
    }

    return item;
}
TreeItem *TreeItem::treeItemByString(QString string)
{
    TreeItem *item = 0;
    int i;

//    qDebug("%s == %s",itemData->toString().toLatin1().data(),string.toLatin1().data());

    if (itemData->toString()==string)
        return this;


    for (i=0;i<childCount();i++)
    {
        item = childItems.at(i)->treeItemByString(string);
        if (item)
            break;
    }

    return item;
}
TreeItem *TreeItem::treeItemByStringChild(QString string)
{
    TreeItem *item = treeItemByString(string);

    if (item && item->childCount())
        item = item->childItems.at(0);
    else
        return 0;

    return item;

}
void TreeItem::dirty(int padNum,int state)
{
//    qDebug("dirty: [%s]",showData("").toLatin1().data());

    switch(ssType)
    {
        case ssTYPE_IDHEAD:
        if (state){
            if(++ssInt2==1)
                emit sigPadDirty(padNum,1);
//                parent()->dirty(ssInt1,1);
        }
        else
        {
            if (!--ssInt2)
                emit sigPadDirty(padNum,0);
 //               parent()->dirty(ssInt1,0);
        }

 //       qDebug("TreeItem::dirty PAD_PARENT [%d][%d]",padNum,ssInt2);
        break;
    case ssTYPE_SLOT_PARENT:
//        qDebug("TreeItem::dirty SLOT_PARENT [%d][%d]",padNum,state);
            emit sigPadDirty(padNum,state);
            break;
        default:
        if (parentItem){
  //          qDebug("Treeitem;:dirty [%d][%d]",padNum,state);
            parentItem->dirty(padNum,state);
        }
    }
}

void TreeItem::dirtyCheck()
{
    bool equal;
    switch(itemData->type())
    {
        case QVariant::String:
 //       qDebug("compare[%s][%s]",itemData->toString().toLatin1().data(),itemDataBackup.toString().toLatin1().data());
            equal = itemData->toString() == itemDataBackup.toString();break;
        case QVariant::LongLong:
             equal = itemData->toLongLong() == itemDataBackup.toLongLong();break;
        case QVariant::ULongLong:
            equal = itemData->toULongLong() == itemDataBackup.toULongLong();break;
        case QVariant::Double:
            equal = itemData->toDouble() == itemDataBackup.toDouble();break;
        case QVariant::Int:
            equal = itemData->toInt() == itemDataBackup.toInt();break;
        case QVariant::List:break;

        default:
        qDebug("slotToggle: unknown qvariant type[%d]",itemData->type());
        break;
    }

//    qDebug("equal[%d]",equal);

    if (equal)
    {
        if (modified)
        {
            modified = false;
            dirty(0,false);
        }
    } else
    {
        if (!modified)
        {
            modified = true;
            dirty(0,true);
        }
    }
}

void TreeItem::slotToggle()
{
    switch(itemData->type())
    {
        case QVariant::String:break;
        case QVariant::LongLong:
            itemData->setValue((qlonglong) (!itemData->toLongLong()));
            emit sigValueInt((int) itemData->toLongLong());
            dirtyCheck();
            break;
        case QVariant::ULongLong:
              itemData->setValue(((qulonglong) !itemData->toULongLong()));
             emit sigValueInt(itemData->toULongLong());
            dirtyCheck();
            break;
        case QVariant::Double:
            itemData->setValue(((double) !itemData->toDouble()));
            emit sigValueDouble(itemData->toDouble());
            dirtyCheck();
            break;
        case QVariant::Int:
            itemData->setValue(((int) !itemData->toInt()));
            emit sigValueInt(itemData->toInt());
            dirtyCheck();
            break;
        case QVariant::List:break;

        default:
        qDebug("slotToggle: unknown qvariant type[%d]",itemData->type());
        break;
    }
}
void TreeItem::slotSetText(QString text)
{
    switch(itemData->type())
    {
        case QVariant::String:
        itemData->setValue(text);
        emit sigValueString(text);
//        qDebug("delete me %s",text.toLatin1().data());
        dirtyCheck();
            break;
        default:
        qDebug("slotSetText: unknown qvariant type[%d]",itemData->type());
            break;
    }
}
void TreeItem::slotSetInt(int newVal)
{
//    qDebug("slotSetInt: %d",newVal);
    switch(itemData->type())
    {
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::ULongLong:
           itemData->setValue(newVal);
            emit sigValueInt(newVal);
            dirtyCheck();
            break;
        case QVariant::Double:
            itemData->setValue((double)newVal);
            emit sigValueDouble((double)newVal);
            dirtyCheck();
            break;
        default:
            qDebug("slotSetInt: unknown qvariant type[%d]",itemData->type());
            break;
    }

}

void TreeItem::slotSetDouble(double newVal)
{

    qDebug("TreeItem::slotSetDouble: %f",newVal);

    switch(itemData->type())
    {
        case QVariant::Int:
        case QVariant::ULongLong:
           itemData->setValue(newVal);
            emit sigValueInt(newVal);
            dirtyCheck();
            break;

    case QVariant::Double:
       itemData->setValue(newVal);
        emit sigValueDouble(newVal);
        dirtyCheck();
        break;

        default:
            qDebug("slotSetDouble: unknown qvariant type[%d]",itemData->type());
            break;
    }

}


void TreeItem::backup()
{
    int i;

    itemDataBackup = *itemData;

    if (modified)
    {
        dirtyCheck();
    }

    for (i=0;i<childCount();i++)
        childItems.at(i)->backup();
}
void TreeItem::revert()
{
    int i;
    if (modified)
    {
        delete itemData;
        itemData = new QVariant(itemDataBackup);
        dirtyCheck();
        ssEmitValues();
    }
    for (i=0;i<childCount();i++)
        childItems.at(i)->revert();
}

QVariant TreeItem::data(QString pre)
{
    QList<QVariant> list;
    QMap<QString,QVariant> map;

//    pre.sprintf("%s",pre.toLatin1().data()+6);

//    pre.sprintf("%6d:%s    ",debug_limit,pre.toLatin1().data());

 //   if (++debug_limit>100000)
   //     return map;

    int i;
    for (i=0;i<childCount();i++)
    {
        TreeItem *child = childItems.at(i);
        if (child)
        {
            if (child->childCount())
            {
                if (child->itemData->type()==QVariant::String){
  //                  qDebug("%smap.insert key[%s]",pre.toLatin1().data(),child->itemData->toString().toLatin1().data());
                    map.insert(child->itemData->toString(),child->data(pre));
                }
                else {
                    qDebug("TreeItem::data() error: key not string [%s]",child->showData("").toLatin1().data());

                }
            }
            else
            {
//                qDebug("%slist.append[%s]",pre.toLatin1().data(),child->showData("").toLatin1().data());
                list.append(*child->itemData);
            }
        }
    }

    if (list.count())
    {
        if (map.count())
            list.append(map);
        return list;
    }
    return map;
}

QString TreeItem::showData(QVariant data,QString title)
{
    QString rString;

    title.sprintf("%s[%d]",title.toLatin1().data(),data.type());

    switch(data.type())
    {
        case QVariant::Map:
                rString.sprintf("%s:Map",title.toLatin1().data());
                break;
        case QVariant::List:
            qDebug("%s:List",title.toLatin1().data());
            break;
        default:
            switch(data.type())
            {

                case QVariant::String:rString.sprintf("%s:%s",title.toLatin1().data(),data.toString().toLatin1().data());break;
                case QVariant::LongLong: rString.sprintf("%s:%ld",title.toLatin1().data(),(long)data.toLongLong());break;
                case QVariant::ULongLong:rString.sprintf("%s:%ld",title.toLatin1().data(),(long) data.toULongLong()); break;
                case QVariant::Double:rString.sprintf("%s:%f",title.toLatin1().data(),(float) data.toDouble());break;
                case QVariant::Int:rString.sprintf("%s:%d",title.toLatin1().data(),data.toInt());break;
                default:
                    rString.sprintf("%s:unknown type[%d]",title.toLatin1().data(),data.type());
                    break;
            }
            break;
     }
    return rString;

}

QString TreeItem::showData(QString title)
{
    return showData(*itemData,title);

}

