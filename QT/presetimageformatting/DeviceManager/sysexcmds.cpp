// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <stdlib.h>
#include <stdio.h>
#include "sysex.h"
#include "sysexcmds.h"

FILE		*fd_syx;


extern "C" STANDALONE_INFO standalone_info;
STANDALONE_INFO standalone_info;

PAD_INFO pad_info;
PEDAL_INFO pedal_info;

void sx_send_list()
{

    printf("sx_send_list called\n");

    if (fd_syx)
    {
        unsigned char *buffer = midi_sx_buffer();
        int len = midi_sx_length();

            fwrite(buffer, len, 1, fd_syx);
            fclose(fd_syx);
            fd_syx = 0;
    }

 //       sendSysex(0,(unsigned char *) midi_sx_buffer(),midi_sx_length(),0);
}

void on_off_common(int state,int save)
{
    standalone_info.u.onoff.state = state;
    standalone_info.u.onoff.save = save;

    midi_sx_header();
    midi_sx_packet(SX_PACKET_STANDALONE,&standalone_info,sizeof(standalone_info));
    midi_sx_close();
}

void t_device_standalone(unsigned char **buffer,int *len, int state, int save)
{
        standalone_info.type = LE_short(SA_TYPE_STANDALONE_ONOFF);

        on_off_common(state,save);

        *buffer = midi_sx_buffer();
        *len = midi_sx_length();
}

void t_device_tether(unsigned char **buffer, int *len, int state, int save)
{
    standalone_info.type = LE_short(SA_TYPE_TETHER_ONOFF);

    on_off_common(state,save);

    *buffer = midi_sx_buffer();
    *len = midi_sx_length();

}

void t_device_nav_tether(unsigned char **buffer,int *len, int state, int save)
{
    standalone_info.type = LE_short(SA_TYPE_NAVTETHER_ONOFF);

    on_off_common(state,save);

    *buffer = midi_sx_buffer();
    *len = midi_sx_length();
}

