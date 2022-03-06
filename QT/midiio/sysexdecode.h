// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SYSEXDECODE_H
#define SYSEXDECODE_H

#include <QWidget>

class SysExDecode : public QWidget
{
    Q_OBJECT
public:
    explicit SysExDecode(QWidget *parent = 0);
    
signals:
    void signalSendGlobals(QString, QByteArray);
    
public slots:
    void slotProcessGlobals(QByteArray sysExMessageByteArray);
    void slotDecodeGlobals(QByteArray globalsByteArray);
    bool slotIsGlobals(QByteArray ba);
    void slotProcessSysEx(QByteArray sysExMessageByteArray);
};

#endif // SYSEXDECODE_H
