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
#include "storedevice.h"

#include "device.h"

#ifdef NOR_SPI_DATA_WIDTH
#include "flash.h"
#endif

#include "sd.h"

#include <stddef.h>
#include <string.h>

S2S_Target* s2s_DeviceFindByScsiId(int scsiId)
{
	int deviceCount;
	S2S_Device** devices = s2s_GetDevices(&deviceCount);
	for (int deviceIdx = 0; deviceIdx < deviceCount; ++deviceIdx)
	{
		int targetCount;
		S2S_Target* targets = devices[deviceIdx]->getTargets(devices[deviceIdx], &targetCount);
		for (int targetIdx = 0; targetIdx < targetCount; ++targetIdx)
		{
			S2S_Target* target = targets + targetIdx;
			if (target &&
                target->cfg &&
				(target->cfg->scsiId & CONFIG_TARGET_ENABLED) &&
				((target->cfg->scsiId & CONFIG_TARGET_ID_BITS) == scsiId))
			{
				return target;
			}
		}
	}

	return NULL;
}

S2S_Device** s2s_GetDevices(int* count)
{
    static S2S_Device* allDevices[2];
    
    *count = 1;
    allDevices[0] = sdDevice;
    
    #ifdef NOR_SPI_DATA_WIDTH
	    *count = 2;
        allDevices[0] = spiFlashDevice;
    #endif
    
    return &allDevices;
}

void s2s_deviceEarlyInit()
{
	int count;
	S2S_Device** devices = s2s_GetDevices(&count);
	for (int i = 0; i < count; ++i)
	{
		devices[i]->earlyInit(devices[i]);
	}
}

void s2s_deviceInit()
{
	int count;
	S2S_Device** devices = s2s_GetDevices(&count);
	for (int i = 0; i < count; ++i)
	{
		devices[i]->init(devices[i]);
	}
}

int s2s_pollMediaChange()
{
	int result = 0;
	int count;
	S2S_Device** devices = s2s_GetDevices(&count);
	for (int i = 0; i < count; ++i)
	{
		int devResult = devices[i]->pollMediaChange(devices[i]);
		result = result || devResult;
	}
	return result;
}
