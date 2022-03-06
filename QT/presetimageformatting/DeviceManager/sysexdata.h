// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SYSEXDATA_H
#define SYSEXDATA_H

#include "12step.h"

#include <QByteArray>
#include <QList>
#include <QString>

#define SX_ENCODE_LEN   7

typedef struct {unsigned char manufacturer_id1,manufacturer_id2,manufacturer_id3,manufacturer_id4,product,format;} SYSEXDATA_STANDARD;
typedef union {
        unsigned char raw[1];
        struct {unsigned short length,crc;} PACK_INLINE fmt;
} PACK_INLINE TAILDATA;
typedef struct {unsigned short type;TAILDATA tail;} PACKET_PREAMBLE;

class SysexData
{
public:
    SysexData(QByteArray sysexRaw);
    bool valid();
    void packetSearchInit();
    bool packetNext();
    int packetType() { return packet_preamble.s.type;}
    void midi_sx_decode_put(unsigned char val);
    void sx_decode_init(void);
    unsigned char midi_sx_decode_get(unsigned char *val);
    bool decodeData(unsigned char *data);
    bool packetDecode();
    bool packetPreambleDecode();
    bool packetDataDecode();
    bool decodeShort(unsigned short *result);
    bool decodeTail();
    const void *packetPtr();
    void crc_init();
    void crc_byte(char val);
    void fixLEpacketPreamble();
    void process();
    bool firmwareStatus(int &buildNum,bool &intact,QString &version);

    QByteArray sysexData;
    int sysexIndex;
    int core_sx_state;
    struct {unsigned char index_in,index_out,buf[SX_ENCODE_LEN+1];} core_sx_decode;
    union {
            PACKET_PREAMBLE s;
            unsigned char raw[sizeof(PACKET_PREAMBLE)];
    } packet_preamble;
    unsigned short crc;

    QList<QByteArray> packets;
    QByteArray packetData;


    bool firmwareHeaderStatus;
    int firmwareHeaderBuildnum;
    bool firmwareHeaderIntact;
    QString firmwareHeaderVersion;

};

#endif // SYSEXDATA_H
