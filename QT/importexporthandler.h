// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef IMPORTEXPORTHANDLER_H
#define IMPORTEXPORTHANDLER_H

#include <QWidget>
#include <QVariant>
#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QApplication>

#include "presetinterface.h"

class ImportExportHandler : public QWidget
{
    Q_OBJECT
public:
    explicit ImportExportHandler(PresetInterface *pi, QWidget *parent = 0);

    PresetInterface *presetInterface;

    QString presetName;
    QVariantMap importedOldPresetMap;
    QVariantMap importedNewPresetMap;

signals:
    void signalAddOrRemovePreset();
    void signalPresetMenu(int numPresets);

    //import old signals
    void signalImportingPresetNum(QString updatingMessage);
    void signalImportingComplete();
    void signalPathNotFound();
    void signalPathFound();

public slots:
    void slotImportPreset();
    void slotExportPreset();

    //import old slots
    void slotImportOldPreset();
    QVariantMap slotConvertPreset();
    QVariantMap slotNormalizePresetMap(QVariantMap normalizeThis);
    //QString slotListErrorCompensation(QList<QVariant> stringList);
    //int slotEmptyListCompensation(QString oldName, QList<QVariant> valueList);
    //QString slotGetNewTableValue(QString oldValue);
};

#endif // IMPORTEXPORTHANDLER_H
