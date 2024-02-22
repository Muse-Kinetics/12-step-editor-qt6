
//#include "utils.h"
#include <QtGlobal>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysex.h"
#include "sysexcmds.h"

#include "StandalonePreset.h"

extern int midi_sx_data_addr;

int first,data_index;

extern STANDALONE_INFO standalone_info;

FILE *fd_c,*fd_syx;

void write_c_title(char *title)
{
    if (!fd_c)
        return;

    fprintf(fd_c,"\ncode const unsigned char %s[]={",title);
    first = 1;
    data_index = 0;
}
void write_c_data(void *data,int length)
{
    int i;

    if (!fd_c)
        return;

    for (i=0;i<length;i++)
    {
        if (i || !first)
            fputc(',',fd_c);

        first = 0;
        if (!(i%16)) {
            fprintf(fd_c,"\n/*%04x*/\t",data_index);
        }
        fprintf(fd_c,"0x%02x",((unsigned char *) data)[i]);
        data_index++;
    }
}
void write_c_end()
{
    if (!fd_c)
        return;

    fprintf(fd_c,"\n\t};");
}
void write_c_close()
{
    if (!fd_c)
        return;

    fclose(fd_c);

    fd_c = 0;
}
void write_c(char *title,void *data,int length)
{
    if (!fd_c)
        return;

    write_c_title(title);
    write_c_data(data,length);
    write_c_end();
}

void send_standalone_settings(unsigned char *settings,int len)
{
    // first we send standalone_info, which primarily tells us the type of the
    // next data packet, or in the case of presets how many preset packets are coming

    standalone_info.u.settings_format = 0;

    standalone_info.type = LE_short(SA_TYPE_SETTINGS);

    midi_sx_header();

    midi_sx_packet_preamble(SX_PACKET_STANDALONE,sizeof(standalone_info));
    midi_sx_packet_data(&standalone_info,sizeof(standalone_info));

    midi_sx_packet_data_close(len); // if this was 0 then this would be the last data packet

    //          write_c("standalone_settings",settings,len);
    //          write_c_close(x);

    midi_sx_packet_data(settings,len);

    midi_sx_packet_data_close(0);    // length 0 to indicate no more data packets

    midi_sx_flush();

    midi_sx_close();
    sx_send_list();


}

void download_file_open(char *name);
void download_file_close();

void send_standalone_image(unsigned char *image,int count)
{

    //qDebug("modline length is %d bytes",sizeof(MODLINE));
        printf("send_sa_image: found %d presets",count);
    //qDebug("preset count is %d", count);// << "preset count" << count;

    //--- only used in development, not necessary. loads sysex from external file
    download_file_open("scenes");

    //Write the sysex header
    midi_sx_header();

    //Zero memory block to hold standalone image
    memset(&standalone_info.u.preset_info,0,sizeof(standalone_info.u.preset_info));

    //Set info type to preset image
    standalone_info.type = LE_short(SA_TYPE_PRESET_IMAGE);

    //Store count in our current info struct
    standalone_info.u.preset_info.format = PRESET_INFO_FORMAT_VERSION; // EB - updated this for CV/12Step2, requires fw 1.0.1

    //Store count in our current info struct
    standalone_info.u.preset_info.num_presets = count;


    //write our preamble
    midi_sx_packet_preamble(SX_PACKET_STANDALONE,sizeof(standalone_info));

    //Writes our presete info to a .c file
    write_c("standalone_info",&standalone_info.u.preset_info,sizeof(standalone_info.u.preset_info));

    //Format our standalone image info into sysex packet
    midi_sx_packet_data(&standalone_info,sizeof(standalone_info));

    //Write title to external .c file
    write_c_title("scenes");

    //Iterate through our scenes
    while(count--)
    {
        //Write bytes signifying packet closed
        midi_sx_packet_data_close(sizeof(IMAGE));

        //Write our image data to the .c file
        write_c_data(image,sizeof(IMAGE));

        //Format our image data in sysex
        midi_sx_packet_data(image,sizeof(IMAGE));

        //Reset data address
        midi_sx_data_addr = 0;

        //Increment to our next image
        image += sizeof(IMAGE);
    }

    //Write the end of our .c file
    write_c_end();

    // length 0 to indicate no more data packets
    midi_sx_packet_data_close(0);

    //Append 0s as needed to fill out packet
    midi_sx_flush();

    //Write SysEx End byte
    midi_sx_close();

    //Writes file to disk
    sx_send_list(SX_TYPE_DOWNLOAD,"send_standalone_image");

    //Closes file we just wrote
    download_file_close();


}
#ifdef UNUSED
void xlate(t_softstep *x)
{
    switch (x->d.settings.pedal_filter.length)
    {
    case 1:
        x->d.settings.pedal_filter.hysteresis = 5;
        break;
    case 2:
        x->d.settings.pedal_filter.hysteresis = 5;
        break;
    case 3:
        x->d.settings.pedal_filter.hysteresis = 5;
        break;
    case 4:
        if (x->d.settings.pedal_filter.hysteresis == 5)
        {
            x->d.settings.pedal_filter.hysteresis = 10;
            x->d.settings.pedal_filter.length = 3;
        } else
            x->d.settings.pedal_filter.hysteresis = 5;

        break;
    case 5:
        switch (x->d.settings.pedal_filter.hysteresis)
        {
        case 7:
            x->d.settings.pedal_filter.length = 4;
            x->d.settings.pedal_filter.hysteresis = 10;
            break;
        case 10:
            x->d.settings.pedal_filter.hysteresis = 5;
            break;
        case 12:
            x->d.settings.pedal_filter.hysteresis = 10;
            break;
        }
    }
    //	post("settings %d %d",x->d.settings.pedal_filter.hysteresis,x->d.settings.pedal_filter.length);
}
void send_standalone_settings(t_softstep *x)
{	
    //	post("settings.key.dead_x[%x]",x->d.settings.key[0].dead_x);
    standalone_info.u.settings_format = 0;

    standalone_info.type = LE_short(SA_TYPE_SETTINGS);

    midi_sx_header();
    //	xlate(x);
    midi_sx_packet_preamble(SX_PACKET_STANDALONE,sizeof(standalone_info));
    midi_sx_packet_data(&standalone_info,sizeof(standalone_info));

    midi_sx_packet_data_close(sizeof(x->d.settings));

    write_c("standalone_settings",&x->d.settings,sizeof(x->d.settings),x);
    write_c_close(x);

    midi_sx_packet_data(&x->d.settings,sizeof(x->d.settings));

    midi_sx_packet_data_close(0);    // length 0 to indicate no more data packets

    midi_sx_flush();

    midi_sx_close();
    sx_send_list(x,SX_TYPE_NORMAL,"send_standalone_settings");
}

#endif
int firmware_compatable(int build_num)
{
    return 1;

    // VK, VK1 55
    // VK2 56
    //qDebug("checking firmware compatable against %d",build_num);
    switch(build_num)
    {
    //		case 55: // VK, VK1
    //		case 56: // VK2
    //		case 57: // VK1f
    //		case 58: // VK1I
    //		case 59: // VK1L
    //		case 60: // VK1M
    //		case 61: // VK2
    case 67: // VK2
    case 68: // VK2 for firmware Program Change no running status
        return 1;
    }
    return 0;
}

void download_start(int build_num,unsigned char *image,int count,unsigned char *settings,int len)
{
    if (!firmware_compatable(build_num))
    {
        //qDebug("not compatable with build num %d",build_num);
        return;
    }

    send_standalone_image(image,count);
    send_standalone_settings(settings,len);

    //        sysex_schedule_init();
    //        sysex_schedule_add(send_standalone_image);
    //        sysex_schedule_add(send_standalone_settings);
    //        sysex_schedule_exec();
}
void download_file_open(char *name)
{
    Q_UNUSED(name);
#ifdef DEVELOPMENT_VERSION
    const char *homeDir = getenv("HOME");
    char fname[200];

    snprintf(fname,sizeof(fname),"%s/%s.c",homeDir,name);
    fd_c = fopen(fname,"w+");
    //qDebug("download_file_open[%s] [%p]",fname,fd_c);
    snprintf(fname,sizeof(fname),"%s/%s.syx",homeDir,name);
    fd_syx = fopen(fname,"w+");
    //       qDebug("download_file_open[%s] [%p]",fname,fd_syx);
#else
    fd_c = 0;
    fd_syx = 0;
#endif

}
void download_file_close()
{
    if (fd_c)
        fclose(fd_c);
    if (fd_syx)
        fclose(fd_syx);
    fd_c = fd_syx = 0;
}






