// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "softsteptreeview.h"
#include "parser.h"
using namespace QJson;

SoftStepTreeView::SoftStepTreeView(const QStringList &arguments,QWidget *parent) :
    QTreeView(parent)
{
    initialized = 0;
    if (arguments.count()>1)
    {
        QFile file (arguments.at(1));
        if (file.exists())
        {
            setupFromFile(file);
            initialized = 1;
        }
    }
}

SoftStepTreeView::SoftStepTreeView(QFile &file,QWidget *parent) :
    QTreeView(parent)
{
    setupFromFile(file);


}

SoftStepTreeView::SoftStepTreeView(const QVariant &qdata,QWidget *parent) :
    QTreeView(parent)
{
    setupFromData(qdata);
}
void SoftStepTreeView::setupFromFile(QFile &file)
{
    Parser parser;
    bool ok;
    QVariant data = parser.parse (&file, &ok);
    if (!ok) {
    qCritical("%s:%i - Error: %s", file.fileName().toAscii().data(), parser.errorLine(), qPrintable(parser.errorString()));
    return;
    }
    else {
//        qDebug() << "json object successfully converted to:";
//        qDebug() << data;
    }

    setupFromData(data);

}

void SoftStepTreeView::splitSharedPrefixChildren()
{
    if (initialized)
        model->splitSharedPrefixChildren();
}
void SoftStepTreeView::setupFromData(const QVariant &data)
{
    model = new TreeModel(data);

    view.setModel(model);
    view.setWindowTitle(QObject::tr("new Simple Tree Model"));
    view.show();
}

