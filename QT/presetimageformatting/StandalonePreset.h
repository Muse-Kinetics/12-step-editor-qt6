// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef STANDALONEPRESET_H
#define STANDALONEPRESET_H

#include "device_includes.h"

#define PRESET_INFO_FORMAT_VERSION 1 // this is the current format version used in preset_info

// CV Enums
// 543210
// |||\\\__ Local control - 8 types (default, gate, pitch, velocity, pressure, tilt, expression pedal, disabled)
// |\\_____ USB control - NoteOn Gate, pitch, Velocity, Bend/CC
// \_______ USB channel 0/1

enum CV_MODE_LOCAL // 3 bit (8 values)
{
    CV_DEFAULT, // 0, default that will work with older presets, CV1 = gate, CV2 = pitch
    CV_GATE,
    CV_PITCH,
    CV_VELOCITY,
    CV_PRESSURE,
    CV_TILT,
    CV_EXPRESSION_PEDAL,
    CV_DISABLED
};

enum CV_MODE_USB // 2 bit (4 values)
{
    CV_USB_GATE,   // CV acts as a gate for note on/off, pitch bend and CCs are direct control
    CV_USB_PITCH,  // CV acts as pitch for note on/off
    CV_USB_VELOCITY,
    CV_USB_BEND_MOD
};

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
    // FIXED_PT bendRange;             // Bend range (FIXED_PT type) (deprecated)
    char reserved[3];
    char bankMSB;                   // Bank number MSB, -1 = disabled
    char bankLSB;                   // Bank number LSB, was previously "bank", -1 = disabled
    char channel;                   // MIDI Channel
    char reserved4;                 // was Program change enable flag, deprecated in fw 1.0.0
    char programChange;             // Program change number, -1 = disabled
    char transpose;                 // Transpose value
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
    unsigned char noteMode:2, cv1ModeLocal:3, cv1ModeUSB:2, cv1USBChannel:1;        // in 12s1 this was 8 bits for noteMode, which only used 2 bits (4 values). Here we've preserved backwards
                                                                                    // compatibility for older presets, while allowing new 12s2 presets to write to these bits for cv modes.
    unsigned char footMode:2, cv2ModeLocal:3, cv2ModeUSB:2, cv2USBChannel:1;        // same...
    VOICE voiceA,voiceB;
    KEY keys[NUM_KEYS];
} PACK_INLINE IMAGE;

//---- PEDAL CALIBRATION
typedef struct
{
    unsigned char heel, toe;
    unsigned char table;
} PEDAL_CALIBRATION;

//---- PEDAL FILTER
typedef struct {
    unsigned char hysteresis,length;
} PACK_INLINE PEDAL_FILTER;


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
    char progchg_rx_channel, reserved1; // was PEDAL_FILTER pedal_filter;
    CONNECT_MODE connect_mode;
} PACK_INLINE SETTINGS;

#endif // STANDALONEPRESET_H
