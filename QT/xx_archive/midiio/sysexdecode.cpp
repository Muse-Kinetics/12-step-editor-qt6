// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "sysexdecode.h"
#include <QDebug>

SysExDecode::SysExDecode(QWidget *parent) :
    QWidget(parent)
{
}

void SysExDecode::slotProcessGlobals(QByteArray sysExMessageByteArray)
{
    char globalsHeader[] = {0x7E, 0x00, 0x20, 0x00};

    //----------------------------- Get header
    QByteArray header;

    for(int i = 1; i < 5; i++)
    {
        header.append(sysExMessageByteArray.at(i));
    }

    if(sysExMessageByteArray.indexOf(QByteArray(globalsHeader, 4)) == 1)
    {
        slotDecodeGlobals(sysExMessageByteArray);
    }
}

void SysExDecode::slotDecodeGlobals(QByteArray globalsByteArray)
{
    //qDebug() << "decode globals" << globalsByteArray;

    QByteArray perKeySensitivities;
    QByteArray table1;
    QByteArray table2;
    QByteArray cvTrims;
    QByteArray cvOffsets;

    for(int i = 0; i < globalsByteArray.length(); i++)
    {
        //qDebug() << "byte" << (unsigned int)globalsByteArray.at(i);
    }

    //An 8 byte packet of sysex chars
    unsigned char  buffer[8];
    int         numPackets = 42;


    //For each of the 42 packets...
    for(int i = 1; i <= numPackets; i++)
    {
        //Clear buffer
        //buffer.clear();

        //Load Buffer, starting with first packet, index 8
        for(int j =0; j < 8; j++)
        {

            //qDebug() << "index:" << j + (i*8) << (unsigned char)globalsByteArray.at(j + (i*8));
            //Use packet num i to get index into global Byte array
            buffer[j] = globalsByteArray.at(j + (i*8));
        }

        //Decode packet
        for(int j =0; j < 7; j++)
        {
            //If decode byte is 1
            if(buffer[7] & 0x01)
            {
                buffer[j] |= 0x80;
            }

            buffer[7] >>=1;

            //qDebug() << "packet " << i << "global index" << j + (i*8) << "packet index" << j << "val" << (unsigned int)buffer[j];

            int globalIndex = j + (i*8);
            //-------------------------------------------- Per Key Sensitivities
            if(globalIndex >= 8 && globalIndex <= 35)
            {
                perKeySensitivities.append(buffer[j]);
            }
            else if(globalIndex >= 36 && globalIndex <= 181)
            {
                table1.append(buffer[j]);
            }
            else if(globalIndex >= 182 && globalIndex <= 328)
            {
                table2.append(buffer[j]);
            }
            else if(globalIndex >= 329 && globalIndex <= 331)
            {
                cvTrims.append(buffer[j]);
            }
            else if(globalIndex >= 332 && globalIndex <= 338)
            {
                cvOffsets.append(buffer[j]);
                //qDebug() << "DECODE" << QString("0x%1").arg(buffer[j],0,16);

            }
        }
    }

    //Set Respective Globals UIs
    emit signalSendGlobals(QString("Per Key Sensitivities"), perKeySensitivities);
    emit signalSendGlobals("Table 1", table1);
    emit signalSendGlobals("Table 2", table2);
    emit signalSendGlobals("CV Trims", cvTrims);
    emit signalSendGlobals("CV Offsets", cvOffsets);

}

bool SysExDecode::slotIsGlobals(QByteArray ba)
{
    char globalsHeader[] = {0x7E, 0x00, 0x20, 0x00};

    if(ba.indexOf(QByteArray(globalsHeader, 4)) == 1 && ba.size() == 345)
    {
        return true;
    }

    return false;
}

void SysExDecode::slotProcessSysEx(QByteArray sysExMessageByteArray)
{
    qDebug() << "SysExDecode::slotProcessSysEx called - size: " << sysExMessageByteArray.size();

    if(slotIsGlobals(sysExMessageByteArray))
    {
        slotDecodeGlobals(sysExMessageByteArray);
    }
}


