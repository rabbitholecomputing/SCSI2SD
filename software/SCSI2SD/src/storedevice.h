//	Copyright (C) 2020 Michael McMaster <michael@codesrc.com>
//
//	This file is part of SCSI2SD.
//
//	SCSI2SD is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	SCSI2SD is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with SCSI2SD.  If not, see <http://www.gnu.org/licenses/>.
#ifndef S2S_DEVICE_H
#define S2S_DEVICE_H

#include "scsi2sd.h"
#include "sense.h"

#include <stdint.h>

struct S2S_DeviceStruct;
typedef struct S2S_DeviceStruct S2S_Device;

struct S2S_TargetStruct;
typedef struct S2S_TargetStruct S2S_Target;

struct S2S_TargetStateStruct;
typedef struct S2S_TargetStateStruct S2S_TargetState;

typedef enum
{
	MEDIA_STARTED = 1,     // Controlled via START STOP UNIT
	MEDIA_PRESENT = 2,     // SD card is physically present
	MEDIA_INITIALISED = 4, // SD card responded to init sequence
	MEDIA_WP = 8           // Write-protect.
} MEDIA_STATE;

struct S2S_TargetStateStruct
{
	ScsiSense sense;

	uint16_t unitAttention; // Set to the sense qualifier key to be returned.

	// Only let the reserved initiator talk to us.
	// A 3rd party may be sending the RESERVE/RELEASE commands
	int reservedId; // 0 -> 7 if reserved. -1 if not reserved.
	int reserverId; // 0 -> 7 if reserved. -1 if not reserved.

	// Shadow parameters, possibly not saved to flash yet.
	// Set via Mode Select
	uint16_t bytesPerSector;
};

struct S2S_TargetStruct
{
	S2S_Device* device;
	S2S_TargetCfg* cfg;

	S2S_TargetState state;
};

struct S2S_DeviceStruct
{
	void (*earlyInit)(S2S_Device* dev);
    void (*init)(S2S_Device* dev);

	S2S_Target* (*getTargets)(S2S_Device* dev, int* count);

	// Get the number of 512 byte blocks
	uint32_t (*getCapacity)(S2S_Device* dev);

	int (*pollMediaChange)(S2S_Device* dev);
	void (*pollMediaBusy)(S2S_Device* dev);

	MEDIA_STATE mediaState;
    CONFIG_STOREDEVICE deviceType;
};

S2S_Target* s2s_DeviceFindByScsiId(int scsiId);

S2S_Device** s2s_GetDevices(int* count);

void s2s_deviceEarlyInit();
void s2s_deviceInit();
int s2s_pollMediaChange();
#endif


