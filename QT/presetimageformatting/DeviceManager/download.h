// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


#ifndef DOWNLOAD_H
#define DOWNLOAD_H



#ifdef __cplusplus
 extern "C" {
 #endif

void download_start(int build_num,unsigned char *image,int count,unsigned char *settings,int len);
int firmware_compatable(int build_num);
void send_standalone_settings(unsigned char *settings,int len);
void send_standalone_image(unsigned char *image,int count);

#ifdef __cplusplus
 }
#endif

#endif // DOWNLOAD_H
