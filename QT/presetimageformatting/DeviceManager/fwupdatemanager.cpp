// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <QFile>
#include "fwupdatemanager.h"
#include "sysexdata.h"

FWupdateManager::FWupdateManager(QObject *parent) :
    QObject(parent)
{
}

bool FWupdateManager::getEmbeddedData(QByteArray &data)
{
    QFile file(":/resources/12step.syx");
    if (file.exists())
    {
       file.open(QFile::ReadOnly);
       data = file.readAll();
       return true;
    }
    return false;
}

bool FWupdateManager::getEmbeddedStatus(int &buildNum,bool &intact,QString &version)
{
    QByteArray sysex;

    if (getEmbeddedData(sysex))
    {
        SysexData sysexData(sysex);

        sysexData.process();

        return sysexData.firmwareStatus(buildNum,intact,version);
     }
    return false;
}

void FWupdateManager::sendFW()
{
    QByteArray sysexData;

    if (getEmbeddedData(sysexData))
       emit sigSysexTx(sysexData.data(),sysexData.count());
}

void FWupdateManager::slotFirmwareStatus(int buildNum,bool firmwareStatusIntact,QString version)
{
    int embBuildNum;
    bool embIntact;
    QString embVersion;

    qDebug("FWupdateManager buildNum[%d] status[%s], version[%s]",buildNum, firmwareStatusIntact ? "Good" : "Bad",version.toLatin1().data());

    if (getEmbeddedStatus(embBuildNum,embIntact,embVersion))
    {
        if (buildNum != embBuildNum){
            emit sigOpenFwDialog();
        } else {
            emit sigFirmwareGood();
        }
            //sendFW();
    }
}

void FWupdateManager::slotUpdateFirmware(){
    sendFW();
}
