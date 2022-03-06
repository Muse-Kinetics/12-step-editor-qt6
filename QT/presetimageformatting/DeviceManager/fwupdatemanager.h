// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef FWUPDATEMANAGER_H
#define FWUPDATEMANAGER_H

#include <QObject>

class FWupdateManager : public QObject
{
    Q_OBJECT
public:
    explicit FWupdateManager(QObject *parent = 0);
    bool getEmbeddedData(QByteArray &data);
    bool getEmbeddedStatus(int &buildNum,bool &intact,QString &version);
    void sendFW();

signals:
    void sigSysexTx(char *sysec,int len);
    void sigOpenFwDialog();
    void sigFirmwareGood();

public slots:
    void slotFirmwareStatus(int buildNum,bool firmwareStatusIntact,QString version);
    void slotUpdateFirmware();



};

#endif // FWUPDATEMANAGER_H
