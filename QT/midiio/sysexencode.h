// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SYSEXENCODE_H
#define SYSEXENCODE_H

#include <QVariant>
#include <QtWidgets> // was QT Gui

class SysExEncode : public QWidget
{
    Q_OBJECT

public:
    explicit SysExEncode(QWidget *parent = 0);

    QByteArray presetByteArray;

    ///////////////////////////////////////
    ///////////_midi_sx_encode ////////////
    ///////////////////////////////////////
    //------ _midi_sx_encode vars
    unsigned int size;
    unsigned char midi_hi_bits;
    unsigned char midi_hi_count;
    unsigned int crc;

    unsigned int SX_PACKET_START;
    unsigned int SX_ENCODE_LEN;

    QMap<QString, unsigned char> sx_ident;

    unsigned char sysex_start;
    unsigned char sysex_end;

    //------ _midi_sx_encode funcs
    void d2h(QVariant d);
    void sysex_out(unsigned char ch);
    void midi_buffer_put_core(unsigned char ch);
    void midi_chunk_init();
    void midi_sx_encode_char(unsigned char ch);
    void midi_sx_encode_int(int i);
    void midi_sx_encode_crc_char(unsigned char ch);
    void midi_sx_encode_crc_int(int i);

    void midi_sx_flush();
    void send_sx_stop();

    void crc_byte(unsigned char ch);
    void crc_init();
    void lineInit();


    ///////////////////////////////////////
    /////////////_structList //////////////
    ///////////////////////////////////////
    //------ _strucList vars
    QStringList keyLayerC;
    QStringList keyLayerJSON;
    QMap<QString, unsigned char> dictionary; //holds key,value pairs to convert strings to specified ints

    ///////////////////////////////////////
    /////////////QuNexusSysex//////////////
    ///////////////////////////////////////
    //----- QuNexusSysEx Vars
    unsigned int PRESET_LENGTH;  //<<------------------------------  DON'T FORGET TO CHANGE THIS WHEN PRESETS GET CHANGED
    unsigned char PRESET_START;
    unsigned int PRESET_LENGTH_MSB;
    unsigned int PRESET_LENGTH_LSB;
    unsigned int sumByte;

    //---- QuNexusSysEx Funcs
    void encode(unsigned char ch);
    void encodeInt(int i);

signals:
    void signalEncodedPreset(QString, QByteArray,  QString);
    void signalEncodedGlobal(QString type, QByteArray bytes, QString);

public slots:
    void slotSysExEncodePreset(QVariantMap preset, unsigned char presetNumChar);
    void slotSysExEncodeGlobal(QString global, QList<int> vals);



};



#endif // SYSEXENCODE_H
