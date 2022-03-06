// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef	SYSEX_INCLUDED
#define	SYSEX_INCLUDED

#include "12step.h"


#define	MAX_SYSEX_SIZE	50000
#define MIDI_SX_START   0xF0
#define MIDI_SX_STOP    0xF7
//#define NULL    0

enum {
        BLOCK_TYPE_REQUEST_FW_VERSION,
        BLOCK_TYPE_REQUEST_FW,
        BLOCK_TYPE_ALPHANUMERIC,
        BLOCK_TYPE_LED,
        BLOCK_TYPE_EL,
        BLOCK_TYPE_FW_HEADER,
        BLOCK_TYPE_FW_BLOCK_HEADER,
        BLOCK_TYPE_FW_DATA,
        BLOCK_TYPE_PAD,
        BLOCK_TYPE_STANDALONE,
        BLOCK_TYPE_SEGMENT_MASK,
        BLOCK_TYPE_PEDAL,
        BLOCK_TYPE_DEBUG_MESSAGE,
        PACKET_TYPE_COUNT
};

#define	LE_short(val) (((val & 0xFF) << 8) + ( ((val) >> 8) & 0xFF))
//#define	LE_int(val)  (LE_short(((int)val)>>16) | LE_short((int)val & 0xffff))
#define	LE_int(val)  ( LE_short((int) (val) & 0xffff)<<16 | LE_short((int)(val)>>16) )


union FW_STATUS {unsigned char part[4]; unsigned int whole;} PACK_INLINE;


struct FW_BLOCK_HEADER {unsigned char block_num;unsigned short length;} PACK_INLINE;
struct FW_HEADER {
        struct {unsigned char bank,block_num_last;unsigned short buildnum,length,crc;union FW_STATUS fw_status;} PACK_INLINE fixed;
        char versionString[20];
};

struct SYSEX_DATA {
        union {
                struct FW_HEADER fw_header;
                unsigned char debug_msg[200];
        } u;
};


typedef struct {unsigned char manufacturer_id1,manufacturer_id2,manufacturer_id3,manufacturer_id4,product,format;} SYSEX_STANDARD;

typedef struct {
        void *data_header_ptr;
        unsigned char data_header_len,(*open)(void);
        void (*datum)(unsigned char schar);
        void (*close)(unsigned char success);} SYSEX_HANDLER;

typedef union {
        unsigned char raw[1];
        struct {unsigned short length,crc;} PACK_INLINE fmt;
} PACK_INLINE TAIL;

typedef struct {unsigned char index;
        unsigned char *header;
        unsigned char packet_count;
//	MIDI_SOURCE xdata *source;
        //	struct {void *dest;unsigned int count,index;} datums;
        const SYSEX_HANDLER *sysex_handler;
        TAIL tail;
} PACKET_DATA_INFO;
typedef union {
        struct FW_BLOCK_HEADER fw_block_header;
//	struct PAD_PACKET_FORM pad_packet;
} PACKET_DATA;

enum {SX_PACKET_DISPLAY=2,SX_PACKET_LED,SX_PACKET_EL,SX_PACKET_PAD=8,SX_PACKET_STANDALONE,SX_PACKET_SEGMENT,SX_PACKET_PEDAL};


#ifdef __cplusplus
 extern "C" {
 #endif

void midi_sx_header(void);
void midi_sx_close(void);
void midi_sx_packet(unsigned short packet_type,void *source,unsigned short length);
void midi_sx_packet_preamble(unsigned short packet_type,unsigned short length);
long midi_sx_length(void);
long midi_sx_byte(int index);
unsigned char *midi_sx_buffer(void);
void midi_sx_packet_data(void *source,unsigned short length);
void midi_sx_packet_data_close(unsigned short length);
void midi_sx_flush(void);
unsigned char null_open(void);
void null_datum(unsigned char val);
void null_close(unsigned char success);
void fw_header_close(unsigned char success);
void debug_msg_close(unsigned char success);
void sx_init(void);
void sysex_completion_flag_clear(void);
int fw_status_get(int *buildNum,int *error,char *version);
unsigned char *get_sysex_debug(void);
void midi_sx_data_crc(void *data,unsigned short length);
void sx_process(unsigned char sx_char);
unsigned char *midi_sysex_data();
int midi_sysex_len();

#ifdef __cplusplus
}
#endif

#endif



