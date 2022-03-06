// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "sysexdata.h"
#include "sysex.h"

#define	LE_short(val) (((val & 0xFF) << 8) + ( ((val) >> 8) & 0xFF))
//#define	LE_int(val)  (LE_short(((int)val)>>16) | LE_short((int)val & 0xffff))
#define	LE_int(val)  ( LE_short((int) (val) & 0xffff)<<16 | LE_short((int)(val)>>16) )

SysexData::SysexData(QByteArray sysexRaw)
{
//    int i;
    sysexData = sysexRaw;

    qDebug("SysexData[%d]",sysexData.count());
    packetSearchInit();

//    for (i=0;i<sysexData.count();i++)
//    {
//        qDebug("sysexData[%d] %02x",i,sysexData.at(i));
//    }
}
bool SysexData::valid()
{
    SYSEXDATA_STANDARD const *s;

    s = (SYSEXDATA_STANDARD *) (sysexData.constData() + 1);

    if (    s->manufacturer_id1 == 0x00 &&
            s->manufacturer_id2 == 0x01 &&
            s->manufacturer_id3 == 0x55 &&
            s->manufacturer_id4 == 0x7A &&
            s->product == 20)
        return true;

    return false;

}
void SysexData::packetSearchInit()
{
    sysexIndex = 20;
    sx_decode_init();
}
void SysexData::crc_init()
{
    crc = 0xFFFF;
}

void SysexData::crc_byte(char val)   {
        unsigned short temp;
        unsigned short quick;

        temp = (crc >> 8) ^ val;
        crc <<= 8;
        quick = temp ^ (temp >> 4);
        crc ^= quick;
        quick <<= 5;
        crc ^= quick;
        quick <<= 7;
        crc ^= quick;
}
bool SysexData::decodeShort(unsigned short *result)
{
    unsigned char high,low;
    unsigned short ret;

    if (!decodeData(&high))
        return false;

    crc_byte(high);

    if (!decodeData(&low))
        return false;

    crc_byte(low);

    ret = high * 256 + low;

    *result = ret;
    return true;
}
bool SysexData::decodeTail()
{
    unsigned int i;
    unsigned char data;
    for (i=sizeof(PACKET_PREAMBLE)-sizeof(TAILDATA);i<sizeof(PACKET_PREAMBLE);i++)
    {
        if (!decodeData(&data))
            return false;

        if (i< (sizeof(PACKET_PREAMBLE) - sizeof(packet_preamble.s.tail.fmt.crc)))
            crc_byte(data);

        packet_preamble.raw[i] = data;
    }
    return true;

}

bool SysexData::packetNext()
{

    for (;sysexIndex < sysexData.count();sysexIndex++)
    {
         switch (sysexData.at(sysexIndex))
        {
            case 0:
                break;
            case 1:
                sysexIndex++;
            return packetDecode();
                break;
            case 0xF7:
            default:
                return false;// not expected
                break;
        }
    }

    return false;
}
bool SysexData::decodeData(unsigned char *data)
{
    for(;;)
    {
        int count;

        if (midi_sx_decode_get(data))
            return true;

        count = sysexData.count();

        if (sysexIndex < count)
            midi_sx_decode_put(sysexData.at(sysexIndex++));
        else
            return false;
     }
}
void SysexData::fixLEpacketPreamble()
{
    packet_preamble.s.type = LE_short(packet_preamble.s.type);
    packet_preamble.s.tail.fmt.length = LE_short(packet_preamble.s.tail.fmt.length);
    packet_preamble.s.tail.fmt.crc = LE_short(packet_preamble.s.tail.fmt.crc);
}

bool SysexData::packetPreambleDecode()
{

    sx_decode_init();

    crc_init();

    if (!decodeShort(&packet_preamble.s.type))
        return false;

    if (!decodeTail())
        return false;

    fixLEpacketPreamble();


    if (crc==packet_preamble.s.tail.fmt.crc)
        return true;

    return false;
}
bool SysexData::packetDataDecode()
{
     unsigned char data;

     packetData.clear();

    crc_init();

    if (packet_preamble.s.tail.fmt.length)
        packet_preamble.s.tail.fmt.length -= 4;
    else
        return false;

    while(packet_preamble.s.tail.fmt.length)
    {
        if (!decodeData(&data))
            return false;

        crc_byte(data);

        packetData.append(data);
        packet_preamble.s.tail.fmt.length--;
    }


    decodeTail();

    fixLEpacketPreamble();

    if (crc == packet_preamble.s.tail.fmt.crc)
            return true;

    return false;
}

bool SysexData::packetDecode()
{

    if (packetPreambleDecode())
    {
        packets.clear();
        while(packetDataDecode()){
            packets.append(packetData);
        }
    } else
        return false;

    return true;

}
const void * SysexData::packetPtr()
{
    return packets.at(0).data();
}

void SysexData::midi_sx_decode_put(unsigned char val) {
        core_sx_decode.buf[core_sx_decode.index_in++] = val;
}
void SysexData::sx_decode_init(void) {
        core_sx_decode.index_in = core_sx_decode.index_out = 0;
}

unsigned char SysexData::midi_sx_decode_get(unsigned char *val) {
        if (core_sx_decode.index_in==SX_ENCODE_LEN+1) {
                *val = core_sx_decode.buf[core_sx_decode.index_out++];
                if (core_sx_decode.buf[SX_ENCODE_LEN] & 1)
                        *val |= 0x80;
                core_sx_decode.buf[SX_ENCODE_LEN] >>=1;
                if (core_sx_decode.index_out==SX_ENCODE_LEN) {
                        sx_decode_init();
                }
                return 1;
        }

        return 0;
}

void SysexData::process()
{

    if (valid())
    {
        firmwareHeaderStatus = false;

        if (packetNext())
        {
            switch (packetType())
            {
                case BLOCK_TYPE_FW_HEADER:
                {
                    struct FW_HEADER *fw = (struct FW_HEADER *) packetPtr();
                    firmwareHeaderStatus = true;
                    firmwareHeaderBuildnum = LE_short(fw->fixed.buildnum);
                    firmwareHeaderIntact = (fw->fixed.fw_status.part[0]=='G') && (fw->fixed.fw_status.part[1]=='G');
                    firmwareHeaderVersion = QString(fw->versionString);

                    break;
                }
                default:
                    qDebug("slotSysexRx: unknown type[%d]",packetType());
                    break;
            }

        }

    }

}
bool SysexData::firmwareStatus(int &buildNum,bool &intact,QString &version)
{
    buildNum = firmwareHeaderBuildnum;
    intact = firmwareHeaderIntact;
    version.clear();
    version = firmwareHeaderVersion;
    return firmwareHeaderStatus;
}
