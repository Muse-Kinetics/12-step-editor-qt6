// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SYSEXCMDS_H
#define SYSEXCMDS_H

#include "sysex.h"
#include "StandalonePreset.h"

#ifdef __cplusplus
 extern "C" {
#endif

void sx_send_list();

#ifdef __cplusplus
}
#endif

void t_device_standalone(unsigned char **buffer,int *len, int state, int save);
void t_device_tether(unsigned char **buffer,int *len, int state, int save);
void t_device_nav_tether(unsigned char **buffer,int *len, int state, int save);

enum {SX_TYPE_NORMAL,SX_TYPE_FWUPDATE,SX_TYPE_DOWNLOAD};
enum {TYPE_NONE,TYPE_DEVICE,TYPE_MIDIINFO,TYPE_MIDIOUT,TYPE_MIDIIN,TYPE_CTL,TYPE_END_OF_LIST};

enum {LED_OFF,LED_ON,LED_SLOW,LED_FAST,LED_BLINK};
enum {LED_GREEN,LED_RED,LED_BOTH};


enum {PAD_QUERY,PAD_LIMIT_LOW};
enum {PEDAL_ONOFF,PEDAL_FILTER_ONOFF,PEDAL_CALIBRATION_EDGES,PEDAL_FILTER_HYS_LENGTH};
enum {SA_TYPE_PRESET_IMAGE,SA_TYPE_PRESET_SET,SA_TYPE_SETTINGS,SA_TYPE_STANDALONE_ONOFF,SA_TYPE_TETHER_ONOFF,SA_TYPE_PIN,SA_TYPE_PORT,SA_TYPE_SCAN,SA_TYPE_NAVTETHER_ONOFF};

#ifdef	KWINDOWS
#pragma PACK(1)
#endif


typedef struct {
        unsigned short type; // the type of standalone packet, ie SA_TYPE_SETTINGS
        union {
                struct {unsigned char format,reserved[2],num_presets;} PACK_INLINE preset_info;
                unsigned char settings_format;
                unsigned char preset_num;
                struct {unsigned char state,save;} onoff;
        } u;
} PACK_INLINE STANDALONE_INFO;

typedef struct {
        unsigned char type;
        union {
                unsigned char pad;
                unsigned char limit;
        } u;

} PACK_INLINE PAD_INFO;

typedef struct {unsigned char hysteresis,length;} PACK_INLINE HL;

typedef struct {
        unsigned char type;
        union {
                unsigned char onoff;
                PEDAL_CALIBRATION pedal_calibration;
                HL hl;
        } u;
} PACK_INLINE PEDAL_INFO;

//extern  STANDALONE_INFO standalone_info;


#endif
