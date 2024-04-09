// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef TWELVESTEP_H
#define TWELVESTEP_H

#include <QtGlobal>
#include "midi.h"

//---- OS DEFINES
#ifndef Q_OS_MAC
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


// calibration structs

#define CURRENT_CV_CAL_VERSION 1
#define NUM_CV_OUTS 2
#define NUM_CV_OCTAVES 6 // 0v counts
#define NUM_CV_NOTES 61 // 0-5v is


//---- GLOBAL DEFINES
#define NUM_KEYS            13
#define NUM_NOTES_PER_KEY   5



// message category/types stored in the sysex preamble
enum SYX_MSG_CATEGORY
{
    MSG_CAT_LEGACY,         // 0x00 = older softstep messaging where type was an int with MSB = 0, LSB = LEGACY_SYX_MSG_TYPES
    MSG_CAT_CALIBRATION,    // new message categories start here
    MSG_CAT_PRESET,         // to request/send preset data
    NUM_MSG_CATEGORIES
};

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

enum SYX_CALIBRATION_MSGS
{
    REQUEST_PEDAL_CAL,
    PEDAL_CAL_PAYLOAD,
    REQUEST_KEYS_CAL,
    KEYS_CAL_PAYLOAD,
    REQUEST_CV_CAL,
    CV_CAL_PAYLOAD,
    RESET_CV_CAL_TO_FACTORY,
    NUM_CALIBRATION_MSG_TYPES
};

enum SYX_PRESET_MSGS
{
    REQUEST_PRESET,
    PRESET_PAYLOAD,
    NUM_PRESET_MSG_TYPES
};


#endif // 12STEP_H
