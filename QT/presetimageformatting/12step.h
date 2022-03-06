// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef TWELVESTEP_H
#define TWELVESTEP_H

//---- GLOBAL DEFINES
#define NUM_KEYS            13
#define NUM_NOTES_PER_KEY   5

#include <QtGlobal>


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

#endif // 12STEP_H
