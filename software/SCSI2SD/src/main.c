//	Copyright (C) 2014 Michael McMaster <michael@codesrc.com>
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
#include "device.h"
#include "scsi.h"
#include "scsiPhy.h"
#include "config.h"
#include "disk.h"
#include "led.h"
#include "time.h"
#include "trace.h"

const char* Notice = "Copyright (C) 2015-2018 Michael McMaster <michael@codesrc.com>";

int main()
{
	timeInit();
	ledInit();
	s2s_deviceEarlyInit();
	traceInit();

	// Enable global interrupts.
	// Needed for RST and ATN interrupt handlers.
	CyGlobalIntEnable;

	// Set interrupt handlers.
	scsiPhyInit();

	configInit(&scsiDev.boardCfg);
	debugInit();

	scsiPhyConfig();

	scsiInit();
	scsiDiskInit();

	// Optional bootup delay
	int delaySeconds = 0;
	while (delaySeconds < scsiDev.boardCfg.startupDelay) {
		// Keep the USB connection working, otherwise it's very hard to revert
		// silly extra-long startup delay settings.
		int i;
		for (i = 0; i < 200; i++) {
			CyDelay(5);
			scsiDev.watchdogTick++;
			configPoll();
		}
		++delaySeconds;
	}

    s2s_deviceInit();

	while (1)
	{
		scsiDev.watchdogTick++;

		scsiPoll();
		scsiDiskPoll();
		configPoll();
		sdPoll();

		if (unlikely(scsiDev.phase == BUS_FREE))
		{
			if (s2s_pollMediaChange())
			{
				scsiPhyConfig();
				scsiInit();
			}
			else
			{
				// Wait for our 1ms timer to save some power.
				// There's an interrupt on the SEL signal to ensure we respond
				// quickly to any SCSI commands. The selection abort time is
				// only 250us, and new SCSI-3 controllers time-out very
				// not long after that, so we need to ensure we wake up quickly.
				uint8_t interruptState = CyEnterCriticalSection();
				if (!SCSI_ReadFilt(SCSI_Filt_SEL))
				{
					__WFI(); // Will wake on interrupt, regardless of mask
				}
				CyExitCriticalSection(interruptState);
			}
		}
		else if ((scsiDev.phase >= 0) &&
			scsiDev.target &&
			(scsiDev.target->device->mediaState & MEDIA_PRESENT))
		{
			scsiDev.target->device->pollMediaBusy(scsiDev.target->device);
		}
	}
	return 0;
}

