// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

//#include <FireLog/FireLog.h>
#include <QDebug>
#include <string.h>
#include <stdio.h>
#include <QTextStream>

#include "sysex.h"
#include "midi.h"
#include "KMI_SysexMessages.h"


#define SX_ENCODE_LEN	0x07
#define SX_PACKET_START 0x01

int sysex_rx_completion_type = 0;

unsigned short crc;
int midi_hi_bits,midi_hi_count,message_len;

unsigned char message[MAX_SYSEX_SIZE];

unsigned char *midi_sysex_data()
{
    return message; //Our message to send
}
int midi_sysex_len()
{
    return message_len;
}


void crc_byte(char val)   {
	unsigned short temp;
	unsigned short quick;

    //qDebug() << "crc - val: " << Qt::hex << (unsigned char)val << " crc-pre: " << Qt::hex << (unsigned int)crc;

	temp = (crc >> 8) ^ val;
	crc <<= 8;
	quick = temp ^ (temp >> 4);
	crc ^= quick;
	quick <<= 5;
	crc ^= quick;
	quick <<= 7;
	crc ^= quick; 
    //qDebug() << "crc - val: " << Qt::hex << (unsigned char)val << " crc-post: " << Qt::hex << (unsigned int)crc;
}


void midi_chunk_init(void) {
	midi_hi_bits = midi_hi_count = 0;
}

void midi_buffer_put_core(unsigned char val){
    //qDebug() << "put_core - val: " << Qt::hex << val << " crc: " << (unsigned int)crc;
    message[message_len] = val;
	if (message_len < (MAX_SYSEX_SIZE-1))
		message_len++;
}

void midi_sx_encode_char(unsigned char val) {
	midi_hi_bits |= (val & 0x80);
	midi_hi_bits >>= 1;
	midi_buffer_put_core(val & 0x7f);
//        qDebug("mbpc %02x",val & 0x7f);
	if (++midi_hi_count == SX_ENCODE_LEN) {
		midi_hi_count = 0;
        //qDebug() << "hiBit Byte";
		midi_buffer_put_core(midi_hi_bits);
//	qDebug("mbpc %02x (hi_bits)",midi_hi_bits);
	}
}
void midi_sx_encode_crc_char(unsigned char val) {
    //qDebug() << "encode: " << Qt::hex << val;
	crc_byte(val);
	midi_sx_encode_char(val);
}
void midi_sx_encode_crc_int(unsigned short val) {
	midi_sx_encode_crc_char(val>>8);
	midi_sx_encode_crc_char(val);
}
void midi_sx_encode_int(unsigned short val) {
	midi_sx_encode_char(val>>8);
	midi_sx_encode_char(val);
}
void midi_sx_flush(void) {
	while(midi_hi_count)
		midi_sx_encode_char(0);
}

#define	TAIL_LEN	4 // length[msb/lsb], crc[msb/lsb]

void midi_sx_packet_preamble(unsigned short packet_type,unsigned short length) {
    crc = 0xffff; // init crc

    midi_buffer_put_core(0x01); // indicate we are about to begin encoding
    midi_chunk_init(); // being 8bit->7bit encoding
    midi_sx_encode_crc_int(packet_type); // 12step2 this became two chars for category/type
	midi_sx_encode_crc_int(length + TAIL_LEN);
	midi_sx_encode_int(crc);

}

extern "C" int midi_sx_data_addr;
int midi_sx_data_addr = 0;
void midi_sx_data_crc(void *data,unsigned short length) {
	int i;
//	qDebug("midi_sx_data_crc: length[%d] data[%02x,%02x,%02x,%02x,%02x,%02x]",length,
//		 length > 0 ? ((unsigned char *) data)[0]:0xff,
//		 length > 1 ? ((unsigned char *) data)[1]:0xff,
//		 length > 2 ? ((unsigned char *) data)[2]:0xff,
//		 length > 3 ? ((unsigned char *) data)[3]:0xff,
//		 length > 4 ? ((unsigned char *) data)[4]:0xff,
//		 length > 5 ? ((unsigned char *) data)[5]:0xff
//		 );
//	qDebug("midi_sx_data_crc length[%d]",length);
//	qDebug("midi_sx_data_addr[%d]",midi_sx_data_addr);
	for(i=0;i<length;i++)
	{
//		qDebug("%3d: %02x %c",midi_sx_data_addr,((unsigned char *) data)[i],isprint(((unsigned char *) data)[i]) ? ((unsigned char *) data)[i] : '.');
		midi_sx_data_addr++;
		midi_sx_encode_crc_char( ((unsigned char *) data)[i]);
//		qDebug("0x%04x, // %4d %02x",crc,i,((unsigned char *) data)[i]);
	}
}

void midi_buffer_put_nulls(int count) {
	while(count--)
		midi_buffer_put_core(0);
}
void midi_sx_header(void) {
	message_len = 0;
	midi_buffer_put_core(MIDI_SX_START);
    midi_buffer_put_core(kmi_id_1);
    midi_buffer_put_core(kmi_id_2);
    midi_buffer_put_core(kmi_id_3);
    midi_buffer_put_core(0); // MIDI PID MSB
    midi_buffer_put_core(PID_12STEP1); // MIDI PID LSB
	midi_buffer_put_core(0x00); // format
	midi_buffer_put_nulls(10);
}

void midi_sx_close(void) {
	midi_buffer_put_core(MIDI_SX_STOP);
}
void midi_sx_packet_data(void *source,unsigned short length) {
	crc = 0xffff;
	midi_sx_data_crc(source,length);
}

// if we are sending multiple data packets, then we encode the length of the next packet
void midi_sx_packet_data_close(unsigned short length) {
    midi_sx_encode_crc_int(length ? length + TAIL_LEN : 0); // no more packets then encode 0
	midi_sx_encode_int(crc);
}

void midi_sx_packet(unsigned short packet_type,void *source,unsigned short length) {
	midi_sx_packet_preamble(packet_type,length);
	midi_sx_packet_data(source,length);
	midi_sx_packet_data_close(0);    // length 0 to indicate no more data packets
	midi_sx_flush();
}

long midi_sx_length(void) {
	return message_len;
}

long midi_sx_byte(int index) {
	return message[index];
}

unsigned char *midi_sx_buffer(void) {
        return (unsigned char *)message;
}

unsigned int packet_crc;
unsigned char sx_packet_opened;

union CORE_SX {
	SYSEX_STANDARD standard_header;
	unsigned char raw[sizeof(SYSEX_STANDARD)];
};

union CORE_SX core_sx;



PACKET_DATA_INFO packet_data_info;
PACKET_DATA pd;


struct SYSEX_DATA sysex_data;

const SYSEX_HANDLER sysex_handlers[] = {
        {0,0,0,0,NULL},// 0 request_fw_version,
        {0,0,0,0,NULL},// 1 request_fw_update,
        {0,0,0,0,NULL},// 2 digit display
        {0,0,0,0,NULL},// 3 leds
        {0,0,0,0,NULL},// 4 EL
        {&sysex_data,sizeof(struct SYSEX_DATA),null_open,null_datum,fw_header_close},// 5
        {0,0,null_open,null_datum,null_close},// 6 EL
        {0,0,null_open,null_datum,null_close},// 7 EL
        {0,0,0,0,NULL},// 8 EL
        {0,0,0,0,NULL},// 9 EL
        {0,0,0,0,NULL},// 10 EL
        {0,0,0,0,NULL},// 11 EL
        {&sysex_data,sizeof(struct SYSEX_DATA),null_open,null_datum,debug_msg_close},// 12

};


void lcd_putchar(unsigned char chr);

unsigned int core_sx_count;

enum {CORE_SX_START,CORE_SX_HEADER,CORE_SX_PACKET_START_SEARCH,CORE_SX_PACKET_PREAMBLE,CORE_SX_PACKET_DATA};

unsigned char core_sx_state;
struct CORE_SX_DECODE {unsigned char index_in,index_out,buf[SX_ENCODE_LEN+1];};
struct CORE_SX_DECODE core_sx_decode;

void null_datum(unsigned char val) {
    Q_UNUSED(val);
}
unsigned char null_open(void) {
	return 1;
}
void null_close(unsigned char success) {
    Q_UNUSED(success);
}

struct SFW_STATUS {int valid,buildnum,error;char version[20];};
struct SFW_STATUS fw_status = {0,0,0,{0}};


#ifdef UNUSED // VERSION H
struct FW_HEADER {
	struct {unsigned char bank,block_num_last;unsigned short buildnum,length,crc;union FW_STATUS fw_status;} PACK_INLINE fixed;
	char versionString[20];
};
#endif

struct FW_HEADER_G {
	struct {unsigned char bank,block_num_last;unsigned short buildnum,length,crc;} PACK_INLINE fixed;
	char versionString[20];
} PACK_INLINE;

void debug_msg_close(unsigned char success) {
    Q_UNUSED(success);
//	int i;
//	for(i=0;i<10;i++)
//		qDebug("debug_msg: [%p] [%02x] [%c]",&sysex_data,sysex_data.u.debug_msg[i],sysex_data.u.debug_msg[i]);
	sysex_rx_completion_type = BLOCK_TYPE_DEBUG_MESSAGE;
}


void fw_header_close(unsigned char success) {
//	qDebug("fw_header_close[%d]",success);
	if (success)
	{
		sysex_rx_completion_type = BLOCK_TYPE_FW_HEADER;
		fw_status.buildnum = LE_short(sysex_data.u.fw_header.fixed.buildnum);
		
		{
		
			strncpy(fw_status.version,sysex_data.u.fw_header.versionString,sizeof(fw_status.version)-1);
		
			if (sysex_data.u.fw_header.fixed.fw_status.part[0] == 'B')
				fw_status.error = 1;
			else
				if (sysex_data.u.fw_header.fixed.fw_status.part[1] == 'B')
					fw_status.error = 2;
				else
					if (sysex_data.u.fw_header.fixed.fw_status.part[2] == 'B')
						fw_status.error = 3;
					else
						if (sysex_data.u.fw_header.fixed.fw_status.part[3] == 'B')
							fw_status.error = 4;
						else
							fw_status.error = 0;
				
//			qDebug("found fw_header: build[%d] version[%s] fw_status[%d %d %d %d]",
//				LE_short(fw_header.fixed.buildnum),fw_header.versionString,
//				 fw_header.fixed.fw_status.part[0],
//				 fw_header.fixed.fw_status.part[1],
//				 fw_header.fixed.fw_status.part[2],
//				 fw_header.fixed.fw_status.part[3]
//				 );
		}
	}
}
void sysex_completion_flag_clear(void){
	sysex_rx_completion_type = 0;
}
int fw_status_get(int *buildNum,int *error,char *version)
{
	if (sysex_rx_completion_type)
	{
		*buildNum = fw_status.buildnum;
		*error = fw_status.error;
		strncpy(version, fw_status.version,sizeof(fw_status.version)-1);
	}
	
	return sysex_rx_completion_type;
}

void core_sx_init(void) {
	core_sx_count = 0;// not one of ours or we would have processed it by now
	core_sx_state = CORE_SX_HEADER;
}

typedef struct {unsigned short type;TAIL tail;} PACK_INLINE PACKET_PREAMBLE;
union UPACKET_PREAMBLE {
	PACKET_PREAMBLE s;
	unsigned char raw[sizeof(PACKET_PREAMBLE)];
} PACK_INLINE;

union UPACKET_PREAMBLE packet_preamble;

void midi_sx_decode_put(unsigned char val) {
	core_sx_decode.buf[core_sx_decode.index_in++] = val;
}
void sx_decode_init(void) {
	core_sx_decode.index_in = core_sx_decode.index_out = 0;
}

unsigned char midi_sx_decode_get(unsigned char *val) {
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
void core_sx_set_packet_search(void) {
	core_sx_state = CORE_SX_PACKET_START_SEARCH;
//	qDebug("CORE_SX_PACKET_START_SEARCH");
}
void core_sx_set_ignore(void) {
	sx_decode_init();
	core_sx_state = CORE_SX_HEADER;
	core_sx_count=sizeof(SYSEX_STANDARD)+1;
}
void crc_init(void) {
	crc = 0xffff;
}
void packet_data_init(TAIL *tail) {
	core_sx_state = CORE_SX_PACKET_DATA;
	packet_data_info.index = 0;
		
	packet_preamble.s.tail.fmt.length = LE_short(tail->fmt.length);
//	qDebug("pdi expecting %d chars",packet_preamble.s.tail.fmt.length);
	crc_init();
	
        packet_data_info.header = (unsigned char *) packet_data_info.sysex_handler->data_header_ptr;
	
//	qDebug("pdi header address %p",packet_data_info.header);

	packet_data_info.packet_count = 0;
//	if (packet_data_info.sysex_handler->data_header_len) {
//		sx_packet_opened = 1;// open to read in data header before calling .open()
//	}
//	else
//		if (packet_data_info.sysex_handler->open)
//			sx_packet_opened = (*packet_data_info.sysex_handler->open)();
//		else
//			(*packet_data_info.sysex_handler->close)(1);
	
	if (!packet_preamble.s.tail.fmt.length)
		core_sx_set_packet_search();
}


void packet_data_process(void) {
	unsigned char sx_char;
	
	while(midi_sx_decode_get(&sx_char)) {
//		qDebug("pdp[%d][%02x]",packet_preamble.s.tail.fmt.length,sx_char);
//		continue;
		
		if (packet_preamble.s.tail.fmt.length-- > sizeof(short))
			crc_byte(sx_char); 
		
		if (packet_preamble.s.tail.fmt.length < sizeof(TAIL))
		{
			packet_data_info.tail.raw[sizeof(TAIL)-1-packet_preamble.s.tail.fmt.length] = sx_char;
			if (!packet_preamble.s.tail.fmt.length)
			{
//				qDebug("pdp crc[%04x] crc[%04x]",crc,LE_short(packet_data_info.tail.fmt.crc));

				if (crc==LE_short(packet_data_info.tail.fmt.crc))
				{
					if (packet_data_info.sysex_handler->close)
					{
//						qDebug("calling close");
						(*packet_data_info.sysex_handler->close)(1);//FUNCTIONCALL
					}
					
					if (LE_short(packet_data_info.tail.fmt.length))
						packet_data_init(&packet_data_info.tail);
					else
					{
						core_sx_set_packet_search();
						return;
					}
				} else
				{
					core_sx_set_ignore();
					if (packet_data_info.sysex_handler->close)
						(*packet_data_info.sysex_handler->close)(0);//FUNCTIONCALL
				}
				packet_data_info.packet_count++;
			}
		} else{
			if (packet_data_info.sysex_handler->data_header_len)
			{
//qDebug("pdi[%d][%d][%p] [%x]",packet_data_info.index,packet_preamble.s.tail.fmt.length,packet_data_info.header,sx_char);
				packet_data_info.header[packet_data_info.index++] = sx_char;
//				qDebug("pdi after[%x]",packet_data_info.header[packet_data_info.index-1]);
				
//				if (packet_data_info.index == sysex_handlers[LE_short(packet_preamble.s.type)].data_header_len)
				if (packet_data_info.index == packet_preamble.s.tail.fmt.length)
				{
					if (packet_data_info.sysex_handler->open)
						sx_packet_opened = (*packet_data_info.sysex_handler->open)();
				}
			}
			else
				if (packet_data_info.sysex_handler->datum)
					(*packet_data_info.sysex_handler->datum)(sx_char);//FUNCTIONCALL
		}
	}
}

void core_sx_packet_init(void) {
	sx_decode_init();
	core_sx_count = 0;
	core_sx_state = CORE_SX_PACKET_PREAMBLE;
	crc_init();
}

void packet_data_handler(const SYSEX_HANDLER *handler){
	packet_data_info.sysex_handler = handler;
}

void sx_init(void) {
	core_sx_state = CORE_SX_START;
}
//int debug_sx_process_count = 0;
void sx_process(unsigned char sx_char) {
	
//if (debug_sx_process_count>260)
//	return;
	
//	qDebug("%5d: sx_process[%x] state[%x]",debug_sx_process_count++,sx_char,core_sx_state);
	

//	qDebug("%02x: %d",sx_char,core_sx_state);
	
		if (sx_char & 0x80)
			sx_init();
	
		if (sx_char==MIDI_SX_START) {
//			second_stage = 0;
			core_sx_init();
		} else
			if (sx_char==MIDI_SX_STOP) {
				sx_init();
				return;
			}
			else {
				switch (core_sx_state) {
					case CORE_SX_PACKET_START_SEARCH:
						if (sx_char==SX_PACKET_START) {
							core_sx_packet_init();
//							qDebug("CORE_SX_PACKET_PREAMBLE");
							
						}
						break;
					case CORE_SX_PACKET_PREAMBLE:
						//EA = 0;
//						qDebug("crc[%04x]",crc);
						midi_sx_decode_put(sx_char);
						while(midi_sx_decode_get(&sx_char)) {
							if (core_sx_count<4)
								crc_byte(sx_char);
//							qDebug("core_sx_count[%d] sx_char[%02x] crc[%04x]",core_sx_count,sx_char,crc);
							packet_preamble.raw[core_sx_count++] = sx_char;
							if (core_sx_count==sizeof(PACKET_PREAMBLE)) {
//								qDebug("rx packet");
								if (LE_short(crc)==packet_preamble.s.tail.fmt.crc) {
//									qDebug("good crc: type[%d] limit[%d]",LE_short(packet_preamble.s.type),PACKET_TYPE_COUNT);
									if (LE_short(packet_preamble.s.type)<PACKET_TYPE_COUNT) {
//										packet_data_info.source = source;
//		qDebug("packet_data_handler %d",LE_short(packet_preamble.s.type));
//										if (debug_sx_process_count>252)
//											return;
										
										packet_data_handler(&sysex_handlers[LE_short(packet_preamble.s.type)]);
										packet_data_init(&packet_preamble.s.tail);
										
										
										if (core_sx_state == CORE_SX_PACKET_DATA)
											packet_data_process();
										
									}
									else
										core_sx_set_ignore();
									//lcd_putchar(packet_preamble.s.num+'0');
								} else {
                                                                        //qDebug("bad crc");
									// set state so rest of sx will not be processed
									core_sx_set_ignore();
								}
								break;
							}
						}
						//EA = 1;
						break;
					case CORE_SX_PACKET_DATA:
						midi_sx_decode_put(sx_char);
						packet_data_process();
						break;
					case CORE_SX_HEADER:
						if (core_sx_count<=sizeof(SYSEX_STANDARD)) {
							core_sx.raw[core_sx_count] = sx_char;
							if (++core_sx_count==sizeof(SYSEX_STANDARD)) {
//								qDebug("CORE_SX_HEADER received");
								
								if (core_sx.standard_header.manufacturer_id1==0x00 &&
									core_sx.standard_header.manufacturer_id2==0x1b &&
									core_sx.standard_header.manufacturer_id3==0x48 &&
									core_sx.standard_header.manufacturer_id4==0x7A &&
									core_sx.standard_header.product == 1 &&
									core_sx.standard_header.format == 0
									) {
									core_sx_set_packet_search();
								}
							}
						}
						break;
				}
			}

}

unsigned char *get_sysex_debug(void)
{
//	char i,c;
	
//	qDebug("sysex data %p",&sysex_data);
	
//	for (i=0;i<10;i++)
//	{
//		c = sysex_data.u.debug_msg[i];
//		qDebug("gsd %x %x",c,packet_data_info.header[i]);
//	}
	
	return sysex_data.u.debug_msg;
}

