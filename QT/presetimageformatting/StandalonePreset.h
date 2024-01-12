// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef STANDALONEPRESET_H
#define STANDALONEPRESET_H

#include "12step.h"

//---- FIXED POINT CONVERSION
typedef union FIXED_PT
{
    int whole;
    struct { unsigned short int upper; unsigned short int lower;} u;
    struct { unsigned char a,b,c,d;} v;
} FIXED_PT;

//---- MODLINE (Single)
typedef struct {
    FIXED_PT Gain;
    char Max,Min,Offset,Smooth,Source_A,Source_B,Table;
} PACK_INLINE MODLINE;

//---- VOICES
typedef struct {
    FIXED_PT bendRange;
    char bank,channel,programChangeEnable,programChange,transpose;
} PACK_INLINE VOICE;

//---- KEY
typedef struct {
    char notes[NUM_NOTES_PER_KEY];
} PACK_INLINE KEY;

//---- MODLINES (Group)
typedef struct {
    MODLINE AfTch,bend,CC1,CC2,Pan,PlyAftTch,Velociy,Volume,XFade;
} PACK_INLINE MODLINES;

//---- IMAGE
typedef struct {
    char slotNum; //Refers to order in setlist: IMAGE should really be labeled PRESET, and IMAGE should be a group of PRESETS
    MODLINES modlines;
    char display[4];
    char CtlNum1,CtlNum2;
    unsigned char noteMode:2, cv1Mode:3, cv2Mode:3;  // in 12s1 this was 8 bits for noteMode, which only used 2 bits (4 values). Here we've preserved backwards
                                            // compatibility for older presets, while allowing new 12s2 presets to write to these bits for cv modes.
    unsigned char footMode:2, presetVersion:6;
    VOICE voiceA,voiceB;
    KEY keys[NUM_KEYS];
} PACK_INLINE IMAGE;

//---- PEDAL CALIBRATION
typedef struct
{
    unsigned char heel, toe;
    unsigned char table;
} PEDAL_CALIBRATION;

enum CVMODE
{
    CV_DEFAULT, // 0, default that will work with older presets, CV1 = gate, CV2 = pitch
    CV_GATE,
    CV_PITCH,
    CV_PRESSURE,
    CV_TILT,
    CV_EXPRESSION_PEDAL,
    CV_USBMIDI_CH15,
    CV_USBMIDI_CH16 // midi notes will control CV = gate, CV2 = pitch. CC1 will control
};

//---- PEDAL FILTER
typedef struct {
    unsigned char hysteresis,length;
} PACK_INLINE PEDAL_FILTER;

//---- CONNECT MODE (Always standalone)
//typedef struct {
//    unsigned char standalone,tether;
//} PACK_INLINE CONNECT_MODE;

typedef struct {
    unsigned char standalone,tether : 1 , midi_volume_reset : 1, key_velocity_disable : 1;
} PACK_INLINE CONNECT_MODE;

//---- INPUT SETTINGS (Labeled input, meaning on the board)
typedef struct {
    FIXED_PT GlobalSensitivity;
    char SelectSensitivity,offThreshold,onThreshold;
} PACK_INLINE INPUT_SETTINGS;

//---- SETTINGS (Includes above, plus other)
typedef struct {
    INPUT_SETTINGS input_settings;
    PEDAL_CALIBRATION pedal_calibration;
    unsigned char keyL_brightness; // no reserved data in the settings struct, so we are repurposing the pedal mpx
    PEDAL_FILTER pedal_filter;
    CONNECT_MODE connect_mode;
} PACK_INLINE SETTINGS;

#endif // STANDALONEPRESET_H
