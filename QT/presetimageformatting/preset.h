// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef PRESET_H
#define PRESET_H

#include "12step.h"

#define NUM_KEYS            13
#define NUM_NOTES_PER_KEY   5

//#define DEVELOPMENT_VERSION
#ifdef	KWINDOWS
#include <windows.h>
#define	PACK_INLINE
#define	stricmp	_stricmp
#define CASE_CMP(v1,v2) stricmp(v1,v2)
#define	snprintf	sprintf_s
//#define vsnprintf	vsprintf_s
#pragma pack(1)
#else
#define	PACK_INLINE __attribute__ ((packed))
#define CASE_CMP(v1,v2) strcasecmp(v1,v2)
#endif

typedef union FIXED_PT
        {
                int whole;
                struct { unsigned short int upper; unsigned short int lower;} u;
                struct { unsigned char a,b,c,d;} v;
        } FIXED_PT;


typedef struct {FIXED_PT Gain;char Max,Min,Offset,Smooth,Source_A,Source_B,Table;} PACK_INLINE MODLINE;
typedef struct {FIXED_PT bendRange;char bank,channel,programChangeEnable,programChange,transpose;} PACK_INLINE VOICE;
typedef struct {char notes[NUM_NOTES_PER_KEY];} PACK_INLINE KEY;


typedef struct { MODLINE AfTch,bend,CC1,CC2,Pan,PlyAftTch,Velociy,Volume,XFade;} PACK_INLINE MODLINES;

typedef struct {
               char slotNum;
               MODLINES modlines;
               char display[4];
               char CtlNum1,CtlNum2;
               char noteMode,footMode;
               VOICE voiceA,voiceB;
                KEY keys[NUM_KEYS];
               } PACK_INLINE IMAGE;

typedef struct {unsigned char heal,toe;unsigned short mpx;} PEDAL_CALIBRATION;
typedef struct {unsigned char hysteresis,length;} PACK_INLINE PEDAL_FILTER;
typedef struct {unsigned char standalone,tether;} PACK_INLINE CONNECT_MODE;
typedef struct {FIXED_PT GlobalSensitivity;char SelectSensitivity,offThreshold,onThreshold;} PACK_INLINE INPUT_SETTINGS;
typedef struct {INPUT_SETTINGS input_settings;PEDAL_CALIBRATION pedal_calibration;PEDAL_FILTER pedal_filter;CONNECT_MODE connect_mode;} PACK_INLINE SETTINGS;


#endif // PRESET_H
