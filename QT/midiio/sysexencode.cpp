// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "sysexencode.h"


SysExEncode::SysExEncode(QWidget *parent) :
    QWidget(parent)
{
    ///////////////////////////////////////
    ///////////_midi_sx_encode ////////////
    ///////////////////////////////////////

    size = 0;
    midi_hi_bits = 0;
    midi_hi_count = 0;
    crc = 0;

    SX_PACKET_START = 0x01;
    SX_ENCODE_LEN   = 0x07;

    sx_ident.insert("manufacturer_id1", 0x00);
    sx_ident.insert("manufacturer_id2", 0x01);
    sx_ident.insert("manufacturer_id3", 0x5F);
    sx_ident.insert("manufacturer_id4", 0x7A);
    sx_ident.insert("product", 25);
    sx_ident.insert("format", 0x00);

    sysex_start = 0xF0;
    sysex_end   = 0xF7;

    ///////////////////////////////////////
    /////////////_structList //////////////
    ///////////////////////////////////////

    keyLayerJSON
            //******** globals *********

            <<
               "Keyboard_Global_Program_Change_A"<<
               "Keyboard_Global_Program_Change_B"<<
               "Keyboard_Global_Program_Change_C"<<
               "Keyboard_Global_Program_Change_D"<<
               "Globals_Gain"<<
               "Globals_Off_Thresh"<<
               "Globals_On_Thresh"<<
               "Globals_Tilt_Sensitivity"<<
               "Keyboard_Global_Mode"<<
               "Keyboard_Global_Channel_Rotation_Active"<<
               "Keyboard_Global_USB_1_Channel"<<
               "Keyboard_Global_USB_2_Channel"<<
               "Keyboard_Global_Key_Selection_Criteria"<<
               "Keyboard_Global_Velocity_Active"<<
               "Keyboard_Global_Pressure_Active"<<
               "Keyboard_Global_Tilt_Active"<<
               "Keyboard_Global_Channel_Pressure_Active"<<
               "Keyboard_Global_Pitch_Bend_Active"<<
               "Keyboard_Global_Poly_Aftertouch_Active"<<
               "Keyboard_Global_CC_Active"<<
               "Keyboard_Global_Channel_Rotation_Offset"<<
               "Keyboard_Global_LED_Mode"<<
               "Keyboard_Global_LED_Remote_Channel"<<
               "Keyboard_Global_Polyphony_Number"<<
               "Globals_LED_Refresh_Style"<<
               "Keyboard_Global_Transpose"<<

               //********* keyboard layer *********

               // cc 00
               "Keyboard_CC_00_Control_Number"<<
               "Keyboard_CC_00_Curve"<<
               "Keyboard_CC_00_Gain"<<
               "Keyboard_CC_00_Max"<<
               "Keyboard_CC_00_Min"<<
               "Keyboard_CC_00_Offset"<<
               "Keyboard_CC_00_MIDI_Output_Device"<<
               "Keyboard_CC_00_Source"<<

               // cc 01
               "Keyboard_CC_01_Control_Number"<<
               "Keyboard_CC_01_Curve"<<
               "Keyboard_CC_01_Gain"<<
               "Keyboard_CC_01_Max"<<
               "Keyboard_CC_01_Min"<<
               "Keyboard_CC_01_Offset"<<
               "Keyboard_CC_01_MIDI_Output_Device"<<
               "Keyboard_CC_01_Source"<<

               // cc 02
               "Keyboard_CC_02_Control_Number"<<
               "Keyboard_CC_02_Curve"<<
               "Keyboard_CC_02_Gain"<<
               "Keyboard_CC_02_Max"<<
               "Keyboard_CC_02_Min"<<
               "Keyboard_CC_02_Offset"<<
               "Keyboard_CC_02_MIDI_Output_Device"<<
               "Keyboard_CC_02_Source"<<

               // channel pressure

               "Keyboard_Channel_Pressure_Curve"<<
               "Keyboard_Channel_Pressure_Gain"<<
               "Keyboard_Channel_Pressure_Max"<<
               "Keyboard_Channel_Pressure_Min"<<
               "Keyboard_Channel_Pressure_Offset"<<
               "Keyboard_Channel_Pressure_MIDI_Output_Device"<<
               "Keyboard_Channel_Pressure_Source"<<

               // pitch bend

               "Keyboard_Pitch_Bend_Curve"<<
               "Keyboard_Pitch_Bend_Gain"<<
               "Keyboard_Pitch_Bend_Max"<<
               "Keyboard_Pitch_Bend_Min"<<
               "Keyboard_Pitch_Bend_Offset"<<
               "Keyboard_Pitch_Bend_MIDI_Output_Device"<<
               "Keyboard_Pitch_Bend_Source"<<

               // poly aftertouch

               "Keyboard_Poly_Aftertouch_Curve"<<
               "Keyboard_Poly_Aftertouch_Gain"<<
               "Keyboard_Poly_Aftertouch_Max"<<
               "Keyboard_Poly_Aftertouch_Min"<<
               "Keyboard_Poly_Aftertouch_Offset"<<
               "Keyboard_Poly_Aftertouch_MIDI_Output_Device"<<
               "Keyboard_Poly_Aftertouch_Source"<<

               // velocity

               "Keyboard_Velocity_Curve"<<
               "Keyboard_Velocity_Gain"<<
               "Keyboard_Velocity_Max"<<
               "Keyboard_Velocity_Min"<<
               "Keyboard_Velocity_Offset"<<
               "Keyboard_Velocity_MIDI_Output_Device"<<

               //******** controller layer *********

               "Controller_Global_MIDI_Output_Device"<<

               //******** controller layer keys *********

               "Controller_Key_0_Controller_Layer_Enable_Disable"<<
               "Controller_Key_0_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_0_Note_Number"<<
               "Controller_Key_0_Pressure_Control_Number"<<
               "Controller_Key_0_Tilt_Control_Number"<<
               "Controller_Key_0_Channel"<<
               "Controller_Key_0_Note_Velocity"<<
               "Controller_Key_0_Pressure_Return_Value"<<
               "Controller_Key_0_Tilt_Return_Value"<<
               "Controller_Key_0_Toggle_Control_Number"<<
               "Controller_Key_0_Toggle_Value"<<
               "Controller_Key_0_Toggle_Return_Value"<<
               "Controller_Key_0_Toggle_On_Off"<<

               "Controller_Key_1_Controller_Layer_Enable_Disable"<<
               "Controller_Key_1_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_1_Note_Number"<<
               "Controller_Key_1_Pressure_Control_Number"<<
               "Controller_Key_1_Tilt_Control_Number"<<
               "Controller_Key_1_Channel"<<
               "Controller_Key_1_Note_Velocity"<<
               "Controller_Key_1_Pressure_Return_Value"<<
               "Controller_Key_1_Tilt_Return_Value"<<
               "Controller_Key_1_Toggle_Control_Number"<<
               "Controller_Key_1_Toggle_Value"<<
               "Controller_Key_1_Toggle_Return_Value"<<
               "Controller_Key_1_Toggle_On_Off"<<

               "Controller_Key_2_Controller_Layer_Enable_Disable"<<
               "Controller_Key_2_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_2_Note_Number"<<
               "Controller_Key_2_Pressure_Control_Number"<<
               "Controller_Key_2_Tilt_Control_Number"<<
               "Controller_Key_2_Channel"<<
               "Controller_Key_2_Note_Velocity"<<
               "Controller_Key_2_Pressure_Return_Value"<<
               "Controller_Key_2_Tilt_Return_Value"<<
               "Controller_Key_2_Toggle_Control_Number"<<
               "Controller_Key_2_Toggle_Value"<<
               "Controller_Key_2_Toggle_Return_Value"<<
               "Controller_Key_2_Toggle_On_Off"<<

               "Controller_Key_3_Controller_Layer_Enable_Disable"<<
               "Controller_Key_3_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_3_Note_Number"<<
               "Controller_Key_3_Pressure_Control_Number"<<
               "Controller_Key_3_Tilt_Control_Number"<<
               "Controller_Key_3_Channel"<<
               "Controller_Key_3_Note_Velocity"<<
               "Controller_Key_3_Pressure_Return_Value"<<
               "Controller_Key_3_Tilt_Return_Value"<<
               "Controller_Key_3_Toggle_Control_Number"<<
               "Controller_Key_3_Toggle_Value"<<
               "Controller_Key_3_Toggle_Return_Value"<<
               "Controller_Key_3_Toggle_On_Off"<<

               "Controller_Key_4_Controller_Layer_Enable_Disable"<<
               "Controller_Key_4_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_4_Note_Number"<<
               "Controller_Key_4_Pressure_Control_Number"<<
               "Controller_Key_4_Tilt_Control_Number"<<
               "Controller_Key_4_Channel"<<
               "Controller_Key_4_Note_Velocity"<<
               "Controller_Key_4_Pressure_Return_Value"<<
               "Controller_Key_4_Tilt_Return_Value"<<
               "Controller_Key_4_Toggle_Control_Number"<<
               "Controller_Key_4_Toggle_Value"<<
               "Controller_Key_4_Toggle_Return_Value"<<
               "Controller_Key_4_Toggle_On_Off"<<

               "Controller_Key_5_Controller_Layer_Enable_Disable"<<
               "Controller_Key_5_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_5_Note_Number"<<
               "Controller_Key_5_Pressure_Control_Number"<<
               "Controller_Key_5_Tilt_Control_Number"<<
               "Controller_Key_5_Channel"<<
               "Controller_Key_5_Note_Velocity"<<
               "Controller_Key_5_Pressure_Return_Value"<<
               "Controller_Key_5_Tilt_Return_Value"<<
               "Controller_Key_5_Toggle_Control_Number"<<
               "Controller_Key_5_Toggle_Value"<<
               "Controller_Key_5_Toggle_Return_Value"<<
               "Controller_Key_5_Toggle_On_Off"<<

               "Controller_Key_6_Controller_Layer_Enable_Disable"<<
               "Controller_Key_6_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_6_Note_Number"<<
               "Controller_Key_6_Pressure_Control_Number"<<
               "Controller_Key_6_Tilt_Control_Number"<<
               "Controller_Key_6_Channel"<<
               "Controller_Key_6_Note_Velocity"<<
               "Controller_Key_6_Pressure_Return_Value"<<
               "Controller_Key_6_Tilt_Return_Value"<<
               "Controller_Key_6_Toggle_Control_Number"<<
               "Controller_Key_6_Toggle_Value"<<
               "Controller_Key_6_Toggle_Return_Value"<<
               "Controller_Key_6_Toggle_On_Off"<<

               "Controller_Key_7_Controller_Layer_Enable_Disable"<<
               "Controller_Key_7_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_7_Note_Number"<<
               "Controller_Key_7_Pressure_Control_Number"<<
               "Controller_Key_7_Tilt_Control_Number"<<
               "Controller_Key_7_Channel"<<
               "Controller_Key_7_Note_Velocity"<<
               "Controller_Key_7_Pressure_Return_Value"<<
               "Controller_Key_7_Tilt_Return_Value"<<
               "Controller_Key_7_Toggle_Control_Number"<<
               "Controller_Key_7_Toggle_Value"<<
               "Controller_Key_7_Toggle_Return_Value"<<
               "Controller_Key_7_Toggle_On_Off"<<

               "Controller_Key_8_Controller_Layer_Enable_Disable"<<
               "Controller_Key_8_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_8_Note_Number"<<
               "Controller_Key_8_Pressure_Control_Number"<<
               "Controller_Key_8_Tilt_Control_Number"<<
               "Controller_Key_8_Channel"<<
               "Controller_Key_8_Note_Velocity"<<
               "Controller_Key_8_Pressure_Return_Value"<<
               "Controller_Key_8_Tilt_Return_Value"<<
               "Controller_Key_8_Toggle_Control_Number"<<
               "Controller_Key_8_Toggle_Value"<<
               "Controller_Key_8_Toggle_Return_Value"<<
               "Controller_Key_8_Toggle_On_Off"<<

               "Controller_Key_9_Controller_Layer_Enable_Disable"<<
               "Controller_Key_9_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_9_Note_Number"<<
               "Controller_Key_9_Pressure_Control_Number"<<
               "Controller_Key_9_Tilt_Control_Number"<<
               "Controller_Key_9_Channel"<<
               "Controller_Key_9_Note_Velocity"<<
               "Controller_Key_9_Pressure_Return_Value"<<
               "Controller_Key_9_Tilt_Return_Value"<<
               "Controller_Key_9_Toggle_Control_Number"<<
               "Controller_Key_9_Toggle_Value"<<
               "Controller_Key_9_Toggle_Return_Value"<<
               "Controller_Key_9_Toggle_On_Off"<<

               "Controller_Key_10_Controller_Layer_Enable_Disable"<<
               "Controller_Key_10_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_10_Note_Number"<<
               "Controller_Key_10_Pressure_Control_Number"<<
               "Controller_Key_10_Tilt_Control_Number"<<
               "Controller_Key_10_Channel"<<
               "Controller_Key_10_Note_Velocity"<<
               "Controller_Key_10_Pressure_Return_Value"<<
               "Controller_Key_10_Tilt_Return_Value"<<
               "Controller_Key_10_Toggle_Control_Number"<<
               "Controller_Key_10_Toggle_Value"<<
               "Controller_Key_10_Toggle_Return_Value"<<
               "Controller_Key_10_Toggle_On_Off"<<

               "Controller_Key_11_Controller_Layer_Enable_Disable"<<
               "Controller_Key_11_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_11_Note_Number"<<
               "Controller_Key_11_Pressure_Control_Number"<<
               "Controller_Key_11_Tilt_Control_Number"<<
               "Controller_Key_11_Channel"<<
               "Controller_Key_11_Note_Velocity"<<
               "Controller_Key_11_Pressure_Return_Value"<<
               "Controller_Key_11_Tilt_Return_Value"<<
               "Controller_Key_11_Toggle_Control_Number"<<
               "Controller_Key_11_Toggle_Value"<<
               "Controller_Key_11_Toggle_Return_Value"<<
               "Controller_Key_11_Toggle_On_Off"<<

               "Controller_Key_12_Controller_Layer_Enable_Disable"<<
               "Controller_Key_12_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_12_Note_Number"<<
               "Controller_Key_12_Pressure_Control_Number"<<
               "Controller_Key_12_Tilt_Control_Number"<<
               "Controller_Key_12_Channel"<<
               "Controller_Key_12_Note_Velocity"<<
               "Controller_Key_12_Pressure_Return_Value"<<
               "Controller_Key_12_Tilt_Return_Value"<<
               "Controller_Key_12_Toggle_Control_Number"<<
               "Controller_Key_12_Toggle_Value"<<
               "Controller_Key_12_Toggle_Return_Value"<<
               "Controller_Key_12_Toggle_On_Off"<<

               "Controller_Key_13_Controller_Layer_Enable_Disable"<<
               "Controller_Key_13_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_13_Note_Number"<<
               "Controller_Key_13_Pressure_Control_Number"<<
               "Controller_Key_13_Tilt_Control_Number"<<
               "Controller_Key_13_Channel"<<
               "Controller_Key_13_Note_Velocity"<<
               "Controller_Key_13_Pressure_Return_Value"<<
               "Controller_Key_13_Tilt_Return_Value"<<
               "Controller_Key_13_Toggle_Control_Number"<<
               "Controller_Key_13_Toggle_Value"<<
               "Controller_Key_13_Toggle_Return_Value"<<
               "Controller_Key_13_Toggle_On_Off"<<

               "Controller_Key_14_Controller_Layer_Enable_Disable"<<
               "Controller_Key_14_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_14_Note_Number"<<
               "Controller_Key_14_Pressure_Control_Number"<<
               "Controller_Key_14_Tilt_Control_Number"<<
               "Controller_Key_14_Channel"<<
               "Controller_Key_14_Note_Velocity"<<
               "Controller_Key_14_Pressure_Return_Value"<<
               "Controller_Key_14_Tilt_Return_Value"<<
               "Controller_Key_14_Toggle_Control_Number"<<
               "Controller_Key_14_Toggle_Value"<<
               "Controller_Key_14_Toggle_Return_Value"<<
               "Controller_Key_14_Toggle_On_Off"<<

               "Controller_Key_15_Controller_Layer_Enable_Disable"<<
               "Controller_Key_15_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_15_Note_Number"<<
               "Controller_Key_15_Pressure_Control_Number"<<
               "Controller_Key_15_Tilt_Control_Number"<<
               "Controller_Key_15_Channel"<<
               "Controller_Key_15_Note_Velocity"<<
               "Controller_Key_15_Pressure_Return_Value"<<
               "Controller_Key_15_Tilt_Return_Value"<<
               "Controller_Key_15_Toggle_Control_Number"<<
               "Controller_Key_15_Toggle_Value"<<
               "Controller_Key_15_Toggle_Return_Value"<<
               "Controller_Key_15_Toggle_On_Off"<<

               "Controller_Key_16_Controller_Layer_Enable_Disable"<<
               "Controller_Key_16_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_16_Note_Number"<<
               "Controller_Key_16_Pressure_Control_Number"<<
               "Controller_Key_16_Tilt_Control_Number"<<
               "Controller_Key_16_Channel"<<
               "Controller_Key_16_Note_Velocity"<<
               "Controller_Key_16_Pressure_Return_Value"<<
               "Controller_Key_16_Tilt_Return_Value"<<
               "Controller_Key_16_Toggle_Control_Number"<<
               "Controller_Key_16_Toggle_Value"<<
               "Controller_Key_16_Toggle_Return_Value"<<
               "Controller_Key_16_Toggle_On_Off"<<

               "Controller_Key_17_Controller_Layer_Enable_Disable"<<
               "Controller_Key_17_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_17_Note_Number"<<
               "Controller_Key_17_Pressure_Control_Number"<<
               "Controller_Key_17_Tilt_Control_Number"<<
               "Controller_Key_17_Channel"<<
               "Controller_Key_17_Note_Velocity"<<
               "Controller_Key_17_Pressure_Return_Value"<<
               "Controller_Key_17_Tilt_Return_Value"<<
               "Controller_Key_17_Toggle_Control_Number"<<
               "Controller_Key_17_Toggle_Value"<<
               "Controller_Key_17_Toggle_Return_Value"<<
               "Controller_Key_17_Toggle_On_Off"<<

               "Controller_Key_18_Controller_Layer_Enable_Disable"<<
               "Controller_Key_18_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_18_Note_Number"<<
               "Controller_Key_18_Pressure_Control_Number"<<
               "Controller_Key_18_Tilt_Control_Number"<<
               "Controller_Key_18_Channel"<<
               "Controller_Key_18_Note_Velocity"<<
               "Controller_Key_18_Pressure_Return_Value"<<
               "Controller_Key_18_Tilt_Return_Value"<<
               "Controller_Key_18_Toggle_Control_Number"<<
               "Controller_Key_18_Toggle_Value"<<
               "Controller_Key_18_Toggle_Return_Value"<<
               "Controller_Key_18_Toggle_On_Off"<<

               "Controller_Key_19_Controller_Layer_Enable_Disable"<<
               "Controller_Key_19_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_19_Note_Number"<<
               "Controller_Key_19_Pressure_Control_Number"<<
               "Controller_Key_19_Tilt_Control_Number"<<
               "Controller_Key_19_Channel"<<
               "Controller_Key_19_Note_Velocity"<<
               "Controller_Key_19_Pressure_Return_Value"<<
               "Controller_Key_19_Tilt_Return_Value"<<
               "Controller_Key_19_Toggle_Control_Number"<<
               "Controller_Key_19_Toggle_Value"<<
               "Controller_Key_19_Toggle_Return_Value"<<
               "Controller_Key_19_Toggle_On_Off"<<

               "Controller_Key_20_Controller_Layer_Enable_Disable"<<
               "Controller_Key_20_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_20_Note_Number"<<
               "Controller_Key_20_Pressure_Control_Number"<<
               "Controller_Key_20_Tilt_Control_Number"<<
               "Controller_Key_20_Channel"<<
               "Controller_Key_20_Note_Velocity"<<
               "Controller_Key_20_Pressure_Return_Value"<<
               "Controller_Key_20_Tilt_Return_Value"<<
               "Controller_Key_20_Toggle_Control_Number"<<
               "Controller_Key_20_Toggle_Value"<<
               "Controller_Key_20_Toggle_Return_Value"<<
               "Controller_Key_20_Toggle_On_Off"<<

               "Controller_Key_21_Controller_Layer_Enable_Disable"<<
               "Controller_Key_21_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_21_Note_Number"<<
               "Controller_Key_21_Pressure_Control_Number"<<
               "Controller_Key_21_Tilt_Control_Number"<<
               "Controller_Key_21_Channel"<<
               "Controller_Key_21_Note_Velocity"<<
               "Controller_Key_21_Pressure_Return_Value"<<
               "Controller_Key_21_Tilt_Return_Value"<<
               "Controller_Key_21_Toggle_Control_Number"<<
               "Controller_Key_21_Toggle_Value"<<
               "Controller_Key_21_Toggle_Return_Value"<<
               "Controller_Key_21_Toggle_On_Off"<<

               "Controller_Key_22_Controller_Layer_Enable_Disable"<<
               "Controller_Key_22_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_22_Note_Number"<<
               "Controller_Key_22_Pressure_Control_Number"<<
               "Controller_Key_22_Tilt_Control_Number"<<
               "Controller_Key_22_Channel"<<
               "Controller_Key_22_Note_Velocity"<<
               "Controller_Key_22_Pressure_Return_Value"<<
               "Controller_Key_22_Tilt_Return_Value"<<
               "Controller_Key_22_Toggle_Control_Number"<<
               "Controller_Key_22_Toggle_Value"<<
               "Controller_Key_22_Toggle_Return_Value"<<
               "Controller_Key_22_Toggle_On_Off"<<

               "Controller_Key_23_Controller_Layer_Enable_Disable"<<
               "Controller_Key_23_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_23_Note_Number"<<
               "Controller_Key_23_Pressure_Control_Number"<<
               "Controller_Key_23_Tilt_Control_Number"<<
               "Controller_Key_23_Channel"<<
               "Controller_Key_23_Note_Velocity"<<
               "Controller_Key_23_Pressure_Return_Value"<<
               "Controller_Key_23_Tilt_Return_Value"<<
               "Controller_Key_23_Toggle_Control_Number"<<
               "Controller_Key_23_Toggle_Value"<<
               "Controller_Key_23_Toggle_Return_Value"<<
               "Controller_Key_23_Toggle_On_Off"<<

               "Controller_Key_24_Controller_Layer_Enable_Disable"<<
               "Controller_Key_24_Keyboard_Layer_Enable_Disable"<<
               "Controller_Key_24_Note_Number"<<
               "Controller_Key_24_Pressure_Control_Number"<<
               "Controller_Key_24_Tilt_Control_Number"<<
               "Controller_Key_24_Channel"<<
               "Controller_Key_24_Note_Velocity"<<
               "Controller_Key_24_Pressure_Return_Value"<<
               "Controller_Key_24_Tilt_Return_Value"<<
               "Controller_Key_24_Toggle_Control_Number"<<
               "Controller_Key_24_Toggle_Value"<<
               "Controller_Key_24_Toggle_Return_Value"<<
               "Controller_Key_24_Toggle_On_Off"<<


               //******** controller layer bend pad *********

               "Controller_Bend_Pad_00_Controller_Layer_Enable_Disable"<<
               "Controller_Bend_Pad_00_Keyboard_Layer_Enable_Disable"<<
               "Controller_Bend_Pad_00_Note_Number"<<
               "Controller_Bend_Pad_00_Pressure_Control_Number"<<
               "Controller_Bend_Pad_00_Tilt_Control_Number"<<
               "Controller_Bend_Pad_00_Channel"<<
               "Controller_Bend_Pad_00_Note_Velocity"<<
               "Controller_Bend_Pad_00_Pressure_Return_Value"<<
               "Controller_Bend_Pad_00_Tilt_Return_Value"<<

               //******** cv layer *********

               //******** cv inputs *********

               "CV_In_CV_1_Channel"<<	// default 1
               "CV_In_CV_1_Curve"<<		// default 0
               "CV_In_CV_1_Gain"<<		// default 100
               "CV_In_CV_1_Max"<< // default 127
               "CV_In_CV_1_Min"<< // default 0
               "CV_In_CV_1_Offset"<< // default 0
               "CV_In_CV_1_MIDI_Output_Device"<< // default USB
               "CV_In_CV_1_CC_Number"<<	// default 126

               "CV_In_CV_2_Channel"<<	// default 1
               "CV_In_CV_2_Curve"<<		// default 0
               "CV_In_CV_2_Gain"<<		// default 100
               "CV_In_CV_2_Max"<< // default 127
               "CV_In_CV_2_Min"<< // default 0
               "CV_In_CV_2_Offset"<< // default 0
               "CV_In_CV_2_MIDI_Output_Device"<< // default USB
               "CV_In_CV_2_CC_Number"<<	// default 127

               //******** cv outputs *********

               "CV_Out_CV_1_Channel"<< // default 1
               "CV_Out_CV_1_Curve"<<	// default 0
               "CV_Out_CV_1_Gain"<< // default 100
               "CV_Out_CV_1_Max"<< // default 127
               "CV_Out_CV_1_Min"<< // default 0
               "CV_Out_CV_1_Offset"<< // default 0
               "CV_Out_CV_1_MIDI_Input_Device"<< //default All
               "CV_Out_CV_1_MIDI_Input_Type"<< // default Note On CV Out

               "CV_Out_CV_2_Channel"<< // default 1
               "CV_Out_CV_2_Curve"<<	// default 0
               "CV_Out_CV_2_Gain"<< // default 100
               "CV_Out_CV_2_Max"<< // default 127
               "CV_Out_CV_2_Min"<< // default 0
               "CV_Out_CV_2_Offset"<< // default 0
               "CV_Out_CV_2_MIDI_Input_Device"<< //default All
               "CV_Out_CV_2_MIDI_Input_Type"<< // default Note On CV Out

               "CV_Out_CV_3_Channel"<< // default 1
               "CV_Out_CV_3_Curve"<<	// default 0
               "CV_Out_CV_3_Gain"<< // default 100
               "CV_Out_CV_3_Max"<< // default 127
               "CV_Out_CV_3_Min"<< // default 0
               "CV_Out_CV_3_Offset"<< // default 0
               "CV_Out_CV_3_MIDI_Input_Device"<< //default All
               "CV_Out_CV_3_MIDI_Input_Type"<< // default Note On CV Out

               "CV_Out_Gate_MIDI_Input_Device"<< // default All
               "CV_Out_Gate_Channel"<< // default All
               "CV_Pitch_Scaling_Scheme"<<
               "CV_Gate_Articulation"<<
               "CV_Gate_S_Trig";


    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------------------//


    //----- Create Dictionary for Translating Strings
    //*** key modes
    dictionary.insert("Off" , 0);
    dictionary.insert("On" , 1);
    dictionary.insert("Legato" , 2);

    //*** channel rotation
    //"CHANNEL_ROTATION_DISABLED" , 0);
    //"CHANNEL_ROTATION_ACTIVE" , 1);

    //*** key selection criteria
    dictionary.insert("Latest" , 0);
    dictionary.insert("Earliest" , 1);
    dictionary.insert("Highest" , 2);
    dictionary.insert("Lowest" , 3);

    //*** led modes
    //"REMOTE_LED_MODE" , 0);
    //"LOCAL_LED_MODE": 1);

    //*** curves
    dictionary.insert("Linear" , 0);
    dictionary.insert("Logarithmic" , 1);
    dictionary.insert("Sine" , 2);
    dictionary.insert("Cosine" , 3);
    dictionary.insert("Exponential" , 4);
    dictionary.insert("Invert" , 5);
    dictionary.insert("Custom 1" , 6);
    dictionary.insert("Custom 2" , 7);
    /*dictionary.insert("Linear" , 0);
    dictionary.insert("Exponential" , 1);
    dictionary.insert("Sin" , 2);
    dictionary.insert("Cos" , 3);
    dictionary.insert("Log" , 4);
    dictionary.insert("Compress" , 5);
    dictionary.insert("Mid" , 6);
    dictionary.insert("Expand" , 7);
    dictionary.insert("Expand 2" , 8);
    dictionary.insert("Shift Up 1" , 9);
    dictionary.insert("Shift Up 2" , 10);
    dictionary.insert("Shift Up 3" , 11);
    dictionary.insert("Shift Up 4" , 12);
    dictionary.insert("Xtreme Comp 1" , 13);
    dictionary.insert("Xtreme Comp 2" , 14);
    dictionary.insert("Xtreme Comp 3" , 15);
    dictionary.insert("Hard Comp" , 16);
    dictionary.insert("Xtreme Expand" , 17);
    dictionary.insert("Invert" , 18); // added 4/3/2013); nyw*/

    //*** output devices
    dictionary.insert("USB 1" , 0);
    dictionary.insert("Expander" , 1);
    dictionary.insert("USB 1 + Expander" , 2);

    //*** sources
    dictionary.insert("None" , 0);
    dictionary.insert("Velocity" , 1);
    dictionary.insert("Key Pitch" , 2);
    dictionary.insert("Key Number" , 3);
    dictionary.insert("Note On" , 4);
    dictionary.insert("Pressure" , 5);
    dictionary.insert("Tilt" , 6);
    dictionary.insert("Bend Pad" , 7);
    dictionary.insert("Expression Pedal" , 8);

    //*** output device sources for the CV
    dictionary.insert("Keyboard" , 0);
    dictionary.insert("Expander" , 1);
    dictionary.insert("USB 3" , 2);
    dictionary.insert("Keyboard + Expander" , 3);
    dictionary.insert("Keyboard + USB 3",	4);
    dictionary.insert("USB 3 + Expander",5);
    dictionary.insert("All" , 6);

    //*** CV midi types
    dictionary.insert("CC#000" , 0);
    dictionary.insert("CC#001" , 1);
    dictionary.insert("CC#002" , 2);
    dictionary.insert("CC#003" , 3);
    dictionary.insert("CC#004" , 4);
    dictionary.insert("CC#005" , 5);
    dictionary.insert("CC#006" , 6);
    dictionary.insert("CC#007" , 7);
    dictionary.insert("CC#008" , 8);
    dictionary.insert("CC#009" , 9);
    dictionary.insert("CC#010" , 10);
    dictionary.insert("CC#011" , 11);
    dictionary.insert("CC#012" , 12);
    dictionary.insert("CC#013" , 13);
    dictionary.insert("CC#014" , 14);
    dictionary.insert("CC#015" , 15);
    dictionary.insert("CC#016" , 16);
    dictionary.insert("CC#017" , 17);
    dictionary.insert("CC#018" , 18);
    dictionary.insert("CC#019" , 19);
    dictionary.insert("CC#020" , 20);
    dictionary.insert("CC#021" , 21);
    dictionary.insert("CC#022" , 22);
    dictionary.insert("CC#023" , 23);
    dictionary.insert("CC#024" , 24);
    dictionary.insert("CC#025" , 25);
    dictionary.insert("CC#026" , 26);
    dictionary.insert("CC#027" , 27);
    dictionary.insert("CC#028" , 28);
    dictionary.insert("CC#029" , 29);
    dictionary.insert("CC#030" , 30);
    dictionary.insert("CC#031" , 31);
    dictionary.insert("CC#032" , 32);
    dictionary.insert("CC#033" , 33);
    dictionary.insert("CC#034" , 34);
    dictionary.insert("CC#035" , 35);
    dictionary.insert("CC#036" , 36);
    dictionary.insert("CC#037" , 37);
    dictionary.insert("CC#038" , 38);
    dictionary.insert("CC#039" , 39);
    dictionary.insert("CC#040" , 40);
    dictionary.insert("CC#041" , 41);
    dictionary.insert("CC#042" , 42);
    dictionary.insert("CC#043" , 43);
    dictionary.insert("CC#044" , 44);
    dictionary.insert("CC#045" , 45);
    dictionary.insert("CC#046" , 46);
    dictionary.insert("CC#047" , 47);
    dictionary.insert("CC#048" , 48);
    dictionary.insert("CC#049" , 49);
    dictionary.insert("CC#050" , 50);
    dictionary.insert("CC#051" , 51);
    dictionary.insert("CC#052" , 52);
    dictionary.insert("CC#053" , 53);
    dictionary.insert("CC#054" , 54);
    dictionary.insert("CC#055" , 55);
    dictionary.insert("CC#056" , 56);
    dictionary.insert("CC#057" , 57);
    dictionary.insert("CC#058" , 58);
    dictionary.insert("CC#059" , 59);
    dictionary.insert("CC#060" , 60);
    dictionary.insert("CC#061" , 61);
    dictionary.insert("CC#062" , 62);
    dictionary.insert("CC#063" , 63);
    dictionary.insert("CC#064" , 64);
    dictionary.insert("CC#065" , 65);
    dictionary.insert("CC#066" , 66);
    dictionary.insert("CC#067" , 67);
    dictionary.insert("CC#068" , 68);
    dictionary.insert("CC#069" , 69);
    dictionary.insert("CC#070" , 70);
    dictionary.insert("CC#071" , 71);
    dictionary.insert("CC#072" , 72);
    dictionary.insert("CC#073" , 73);
    dictionary.insert("CC#074" , 74);
    dictionary.insert("CC#075" , 75);
    dictionary.insert("CC#076" , 76);
    dictionary.insert("CC#077" , 77);
    dictionary.insert("CC#078" , 78);
    dictionary.insert("CC#079" , 79);
    dictionary.insert("CC#080" , 80);
    dictionary.insert("CC#081" , 81);
    dictionary.insert("CC#082" , 82);
    dictionary.insert("CC#083" , 83);
    dictionary.insert("CC#084" , 84);
    dictionary.insert("CC#085" , 85);
    dictionary.insert("CC#086" , 86);
    dictionary.insert("CC#087" , 87);
    dictionary.insert("CC#088" , 88);
    dictionary.insert("CC#089" , 89);
    dictionary.insert("CC#090" , 90);
    dictionary.insert("CC#091" , 91);
    dictionary.insert("CC#092" , 92);
    dictionary.insert("CC#093" , 93);
    dictionary.insert("CC#094" , 94);
    dictionary.insert("CC#095" , 95);
    dictionary.insert("CC#096" , 96);
    dictionary.insert("CC#097" , 97);
    dictionary.insert("CC#098" , 98);
    dictionary.insert("CC#099" , 99);
    dictionary.insert("CC#100" , 100);
    dictionary.insert("CC#101" , 101);
    dictionary.insert("CC#102" , 102);
    dictionary.insert("CC#103" , 103);
    dictionary.insert("CC#104" , 104);
    dictionary.insert("CC#105" , 105);
    dictionary.insert("CC#106" , 106);
    dictionary.insert("CC#107" , 107);
    dictionary.insert("CC#108" , 108);
    dictionary.insert("CC#109" , 109);
    dictionary.insert("CC#110" , 110);
    dictionary.insert("CC#111" , 111);
    dictionary.insert("CC#112" , 112);
    dictionary.insert("CC#113" , 113);
    dictionary.insert("CC#114" , 114);
    dictionary.insert("CC#115" , 115);
    dictionary.insert("CC#116" , 116);
    dictionary.insert("CC#117" , 117);
    dictionary.insert("CC#118" , 118);
    dictionary.insert("CC#119" , 119);
    dictionary.insert("CC#120" , 120);
    dictionary.insert("CC#121" , 121);
    dictionary.insert("CC#122" , 122);
    dictionary.insert("CC#123" , 123);
    dictionary.insert("CC#124" , 124);
    dictionary.insert("CC#125" , 125);
    dictionary.insert("CC#126" , 126);
    dictionary.insert("CC#127" , 127);
    dictionary.insert("Note CV Out" , 128);
    dictionary.insert("Velocity CV Out" , 129);
    dictionary.insert("Channel Pressure CV Out" , 130);
    dictionary.insert("Pitch Bend CV Out" , 131);

    dictionary.insert("1 Volt/Octave" , 0);
    dictionary.insert("1.2 Volts/Octave" , 1);
    dictionary.insert("Hz/Volt" , 2);

    //Led Refresh Style
    dictionary.insert("Normal", 0); //Handled in Key modes sections
    dictionary.insert("Control Only", 2);
    dictionary.insert("All Off", 1);

    dictionary.insert("true" , 1);
    dictionary.insert("false" , 0);


    ///////////////////////////////////////
    /////////////QuNexusSysex//////////////
    ///////////////////////////////////////
    //----- QuNexusSysEx Vars
    PRESET_LENGTH = 470;  //<<------------------------------  DON'T FORGET TO CHANGE THIS WHEN PRESETS GET CHANGED
    PRESET_START = 0xA1;
    PRESET_LENGTH_MSB = (PRESET_LENGTH >> 8);
    PRESET_LENGTH_LSB = (PRESET_LENGTH & 0xFF);
}


///////////////////////////////////////
///////////_midi_sx_encode ////////////
///////////////////////////////////////
void SysExEncode::sysex_out(unsigned char ch)
{
    QString chr;
    chr.setNum(ch, 16);
    //qDebug() << presetByteArray.count() << "-" << chr << "       crc - " << crc;
    presetByteArray.append(ch);
}

void SysExEncode::midi_buffer_put_core(unsigned char ch)
{
    sysex_out(ch);
}

void SysExEncode::midi_chunk_init()
{
    midi_hi_bits = midi_hi_count = 0;
}

void SysExEncode::midi_sx_encode_char(unsigned char ch)
{
    midi_hi_bits |= (ch & 0x80);
    midi_hi_bits >>=1;
    midi_buffer_put_core(ch & 0x7f);
    if (++midi_hi_count == SX_ENCODE_LEN)
    {
        midi_hi_count = 0;
        //itemName = "High Bit Decode Byte";
        midi_buffer_put_core(midi_hi_bits);
        //itemName = ""
    }
}

void SysExEncode::midi_sx_encode_int(int i)
{
    midi_sx_encode_crc_char((unsigned char)((i>>8) & 0xFF)); // encode the MSB
    midi_sx_encode_crc_char((unsigned char)(i & 0xFF)); // encode the LSB
}

void SysExEncode::midi_sx_encode_crc_char(unsigned char ch)
{
    crc_byte(ch);
    midi_sx_encode_char(ch);
}

void SysExEncode::midi_sx_encode_crc_int(int i)
{
    midi_sx_encode_crc_char((unsigned char)((i>>8) & 0xFF)); // encode the MSB
    qDebug() << "ENCODE MSB" << QString("0x%1").arg((unsigned char)((i>>8) & 0xFF), 0, 16);

    midi_sx_encode_crc_char((unsigned char)(i & 0xFF)); // encode the LSB
    qDebug() << "ENCODE LSB" << QString("0x%1").arg((unsigned char)(i & 0xFF), 0, 16);
}

void SysExEncode::midi_sx_flush()
{
    while(midi_hi_count)
    {
        midi_sx_encode_crc_char(0);
    }
}

void SysExEncode::send_sx_stop()
{
    midi_buffer_put_core(sysex_end);
}

void SysExEncode::crc_byte(unsigned char ch)
{
    unsigned int temp = 0;
    unsigned int quick = 0;
    //if we represent crc at start as 0xHHLL
    temp = (crc >> 8) ^ ch;    //xor 8 bit val with upper byte of crc (0x00HH ^ val) = 0x00XX
    crc &= 0xFFFF;
    crc <<= 8;				    // left shift crc now 0xLL00
    crc &= 0xFFFF;
    quick = ((temp ^ (temp >> 4)) & 0xFFFF);	//0x00XX ^ 0x000X = 0x00XY
    crc ^= quick;                               // 0xLL00 ^ 0x00XY = 0xLLXY
    crc &= 0xFFFF;                              //effect of all this is to preserve the information in
    //LSB (LL) intact, while mixing the new data and the old MSB thoroughly

    quick <<= 5;				//hash	(0x00XY << 5) = 0xNNN0	 (quick * 2 to the fifth)
    quick &= 0xFFFF;
    crc ^= quick;				//hash
    crc &= 0xFFFF;
    quick <<= 7;				//hash	(0xNNN0 << 7) = 0xN000
    quick &= 0xFFFF;			//	 (quick * 2 to the seventh)
    crc ^= quick; 				//hash
    crc &= 0xFFFF;
}

void SysExEncode::crc_init()
{
    crc = 0xffff;
}

void SysExEncode::lineInit()
{
    crc_init();
    midi_chunk_init();
}

///////////////////////////////////////
/////////////QuNexusSysex//////////////
///////////////////////////////////////

void SysExEncode::encode(unsigned char ch)
{
    //qDebug() << "char:" << ch;
    midi_sx_encode_crc_char(ch);

    if (ch < 0)			//write text file for use in explicitly initializing variables
    {
        ch += 256;
    }

    //	print('\t#', count++);
    //count++;
    sumByte += ch;
    sumByte &= 0xFF;
}

void SysExEncode::encodeInt(int i)
{
    midi_sx_encode_crc_int(i);

    if (i < 0)  //write text file for use in explicitly initializing variables
    {
        i += 256;
    }

    //count++;
    sumByte += i;
    sumByte &= 0xFF;
}

///////////////////////////////////////
/////////////  Encoding  //////////////
///////////////////////////////////////

void SysExEncode::slotSysExEncodePreset(QVariantMap preset, unsigned char presetNumChar)
{
    presetByteArray.clear();

    //----- SysEx Header
    sysex_out(sysex_start);
    sysex_out(sx_ident.value("manufacturer_id1"));
    sysex_out(sx_ident.value("manufacturer_id2"));
    sysex_out(sx_ident.value("manufacturer_id3"));
    sysex_out(sx_ident.value("manufacturer_id4"));
    sysex_out(sx_ident.value("product"));
    sysex_out(sx_ident.value("format"));
    //itemName = "SX_PACKET_START"
    sysex_out(SX_PACKET_START);
    //itemName = ""

    //----- Packet Preamble
    crc_init(); // sends nothing
    midi_chunk_init(); //sends nothing

    //itemName = "\tstart text"
    midi_sx_encode_crc_char(0x00);
    midi_sx_encode_crc_char(0x02);

    //itemName = "preset start"
    midi_sx_encode_crc_char(0x22);
    midi_sx_encode_crc_char(0x20);

    //itemName = "CRC"
    midi_sx_encode_crc_int(crc);
    //itemName = "flush"
    midi_sx_flush();

    //----- Preset Body
    //print('\nPRESET STARTS');
    midi_sx_encode_char(PRESET_START);
    midi_sx_encode_char(PRESET_LENGTH_MSB);
    midi_sx_encode_char(PRESET_LENGTH_LSB);
    crc_init();

    //count = 1 ;//PRESET_LENGTH//1;  //preset size
    //hash = "c"
    sumByte = 0;

    encode(presetNumChar); //0 == Prest Number

    for (int i=0; i<keyLayerJSON.count(); i++)
    {

        QString tempParamName = keyLayerJSON[i];
        QVariant tempPresetValue = preset.value(tempParamName);

        //qDebug() << "JSON Param: " << tempParamName << "Value: " << tempPresetValue;


        if(tempPresetValue.typeName() == QString("QString"))
        {
            tempPresetValue = dictionary.value(preset.value(tempParamName).toString());
            //qDebug() << "String Param:" << tempParamName << "Dictionary Value" << tempPresetValue;
        }
        else if(tempPresetValue.typeName() == QString("int"))
        {
            //tempPresetValue.toInt();
        }
        else if(tempPresetValue.typeName() == QString("double"))
        {
            //tempPresetValue.toDouble();
        }

        //qDebug() << "encode this val:" << tempParamName << tempPresetValue.toInt();

        //If multiple byte param
        if(tempParamName.contains("Gain") && tempParamName != "Globals_Gain")
        {
            encodeInt(tempPresetValue.toInt());
        }
        else
        {
            encode(tempPresetValue.toInt());
        }

        //encode(tempPresetValue.toInt());
    }

    encode(sumByte);
    midi_sx_flush();
    sysex_out(sysex_end);

    emit signalEncodedPreset("Sync Edit Buffer", presetByteArray, "QuNexus Port 1");
}



void SysExEncode::slotSysExEncodeGlobal(QString global, QList<int> vals)
{
    qDebug() << "size of globals" << vals.size() << "global: " << global;

    unsigned int GLOBAL_LENGTH = 290;
    unsigned char GLOBAL_START = 0xA1;
    unsigned int GLOBAL_LENGTH_MSB = GLOBAL_LENGTH >> 8;
    unsigned int GLOBAL_LENGTH_LSB = GLOBAL_LENGTH & 0xFF;

    //qDebug() << "slot encode global";

    presetByteArray.clear();

    sysex_out(sysex_start);

    sysex_out(0x00);
    sysex_out(0x01);
    sysex_out(0x5F);
    sysex_out(0x7A);
    sysex_out(25);      //PID = 25 for qunexus
    sysex_out(0x00);

    sysex_out(SX_PACKET_START);

    crc_init();
    midi_chunk_init();

    // ***** PREAMBLE *****
    //preamble is 6 bytes long
    // -type
    //  - ID
    //  -crc

    //Type is unsigned int, use 0x02 in lsb
    midi_sx_encode_crc_char(0x00);
    midi_sx_encode_crc_char(0x02);  //0x02 is START_OF_TEXT

    //ID unsigned int, 0x5000 is update globals
    midi_sx_encode_crc_char(0x50);  //HIGH BYTE
    midi_sx_encode_crc_char(0x00);  //LOW BYTE

    //CRC
    midi_sx_encode_crc_int(crc);
    midi_sx_flush();

    midi_sx_encode_char(GLOBAL_START);
    midi_sx_encode_char(GLOBAL_LENGTH_MSB);
    midi_sx_encode_char(GLOBAL_LENGTH_LSB);
    crc_init();

    //Encode sensitivity bytes here
    for(int i = 0; i < vals.count(); i++)
    {
        if(i < (vals.count() - 3))
        {
            encode(vals.at(i));
        }
        else
        {
            encodeInt(vals.at(i));
        }
    }

    midi_sx_encode_crc_char(sumByte);

    midi_sx_flush();

    sysex_out(sysex_end);

    emit signalEncodedGlobal("Write Globals", presetByteArray, "QuNexus Port 1");


}






