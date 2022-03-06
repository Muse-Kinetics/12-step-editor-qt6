// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "tooltipeventfilter.h"

ToolTipEventFilter::ToolTipEventFilter(QObject *parent) :
    QObject(parent)
{
    toolTipsOn = true;
}

bool ToolTipEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    //QWidget *widget = reinterpret_cast<QWidget *>(obj);

    if(event->type() == QEvent::ToolTip && !toolTipsOn)
    {
        return true;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}
