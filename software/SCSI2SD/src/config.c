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
#include "config.h"
#include "debug.h"
#include "USBFS.h"
#include "led.h"

#include "scsi.h"
#include "scsiPhy.h"
#include "disk.h"
#include "trace.h"

#include "../../include/scsi2sd.h"
#include "../../include/hidpacket.h"

#include <string.h>

static const uint16_t FIRMWARE_VERSION = 0x0485;

// 1 flash row
static const uint8_t DEFAULT_CONFIG[256] =
{
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x3F, 0x00,
	0x00, 0x02, 0x3F, 0x00, 0xFF, 0x00, 0x20, 0x63, 0x6F, 0x64, 0x65, 0x73,
	0x72, 0x63, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x53,
	0x43, 0x53, 0x49, 0x32, 0x53, 0x44, 0x20, 0x31, 0x2E, 0x30, 0x31, 0x32,
	0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
	0x37, 0x38, 0x00, 0x00
};

enum USB_ENDPOINTS
{
	USB_EP_OUT = 1,
	USB_EP_IN = 2,
	USB_EP_COMMAND = 3,
	USB_EP_DEBUG = 4
};
enum USB_STATE
{
	USB_IDLE,
	USB_DATA_SENT
};

static uint8_t hidBuffer[USBHID_LEN];

static int usbInEpState;
static int usbDebugEpState;
static int usbReady;

static void initBoardConfig(S2S_BoardConfig* config) {
	memcpy(
		config,
		(
			CY_FLASH_BASE +
			(CY_FLASH_SIZEOF_ARRAY * (size_t) SCSI_CONFIG_ARRAY) +
			(CY_FLASH_SIZEOF_ROW * SCSI_CONFIG_BOARD_ROW)
			),
		sizeof(S2S_BoardConfig));

	if (memcmp(config->magic, "BCFG", 4)) {
		// Set a default from the deprecated flags, or 0 if
		// there is no initial config.
		config->flags = getConfigByIndex(0)->flagsDEPRECATED;

		config->selectionDelay = 255; // auto
		config->flags6 = S2S_CFG_ENABLE_TERMINATOR;
	}
}

void configInit(S2S_BoardConfig* config)
{
	// The USB block will be powered by an internal 3.3V regulator.
	// The PSoC must be operating between 4.6V and 5V for the regulator
	// to work.
	USBFS_Start(0, USBFS_5V_OPERATION);
	usbInEpState = usbDebugEpState = USB_IDLE;
	usbReady = 0; // We don't know if host is connected yet.

	int invalid = 1;
	uint8_t* rawConfig = (uint8_t*)getConfigByIndex(0);
	int i;
	for (i = 0; i < 64; ++i)
	{
		if (rawConfig[i])
		{
			invalid = 0;
			break;
		}
	}
	if (invalid)
	{
		// Save a default config.
		CySetTemp();
		CyWriteRowData(SCSI_CONFIG_ARRAY, SCSI_CONFIG_0_ROW, DEFAULT_CONFIG);
	}

	initBoardConfig(config);
}

static void
readFlashCommand(const uint8_t* cmd, size_t cmdSize)
{
	if (cmdSize < 3)
	{
		return; // ignore.
	}
	uint8_t flashArray = cmd[1];
	uint8_t flashRow = cmd[2];

	uint8_t* flash =
		CY_FLASH_BASE +
		(CY_FLASH_SIZEOF_ARRAY * (size_t) flashArray) +
		(CY_FLASH_SIZEOF_ROW * (size_t) flashRow);

	hidPacket_send(flash, SCSI_CONFIG_ROW_SIZE);
}

static void
writeFlashCommand(const uint8_t* cmd, size_t cmdSize)
{
	if (cmdSize < 259)
	{
		return; // ignore.
	}
	uint8_t flashArray = cmd[257];
	uint8_t flashRow = cmd[258];

	// Be very careful not to overwrite the bootloader or other\r
	// code. Bootloader updates no longer supported. Use v5.1 board
	// instead.
	if ((flashArray != SCSI_CONFIG_ARRAY) ||
		(flashRow < SCSI_CONFIG_4_ROW) ||
		(flashRow >= SCSI_CONFIG_3_ROW + SCSI_CONFIG_ROWS))
	{
		uint8_t response[] = { CONFIG_STATUS_ERR };
		hidPacket_send(response, sizeof(response));
	}

	CySetTemp();
	int status = CyWriteRowData(flashArray, flashRow, cmd + 1);

	uint8_t response[] =
	{
		status == CYRET_SUCCESS ? CONFIG_STATUS_GOOD : CONFIG_STATUS_ERR
	};
	hidPacket_send(response, sizeof(response));
}

static void
pingCommand()
{
	uint8_t response[] =
	{
		CONFIG_STATUS_GOOD
	};
	hidPacket_send(response, sizeof(response));
}

static void
sdInfoCommand()
{
	uint8_t response[sizeof(sdCard.csd) + sizeof(sdCard.cid)];
	memcpy(response, sdCard.csd, sizeof(sdCard.csd));
	memcpy(response + sizeof(sdCard.csd), sdCard.cid, sizeof(sdCard.cid));

	hidPacket_send(response, sizeof(response));
}


static void
scsiTestCommand()
{
	int resultCode = scsiSelfTest();
	uint8_t response[] =
	{
		resultCode == 0 ? CONFIG_STATUS_GOOD : CONFIG_STATUS_ERR,
		resultCode
	};
	hidPacket_send(response, sizeof(response));
}

static void
deviceListCommand()
{
    int deviceCount;
    S2S_Device** devices = s2s_GetDevices(&deviceCount);
    
    uint8_t response[16] = // Make larger if there can be more than 2 devices
    {
        deviceCount
    };
    
    int pos = 1;
    
    for (int i = 0; i < deviceCount; ++i)
    {
        response[pos++] = devices[i]->deviceType;
        
        uint32_t capacity = devices[i]->getCapacity(devices[i]);
        response[pos++] = capacity >> 24;
        response[pos++] = capacity >> 16;
        response[pos++] = capacity >> 8;
        response[pos++] = capacity;
    }
    
    hidPacket_send(response, pos);
}

static void
deviceEraseCommand(const uint8_t* cmd)
{
    int deviceCount;
    S2S_Device** devices = s2s_GetDevices(&deviceCount);
    
    uint32_t sectorNum =
        ((uint32_t)cmd[2]) << 24 |
        ((uint32_t)cmd[3]) << 16 |
        ((uint32_t)cmd[4]) << 8 |
        ((uint32_t)cmd[5]);

    uint32_t count =
        ((uint32_t)cmd[6]) << 24 |
        ((uint32_t)cmd[7]) << 16 |
        ((uint32_t)cmd[8]) << 8 |
        ((uint32_t)cmd[9]);

    devices[cmd[1]]->erase(devices[cmd[1]], sectorNum, count);
    
	uint8_t response[] =
	{
		CONFIG_STATUS_GOOD
	};
    hidPacket_send(response, sizeof(response));
}

static void
deviceWriteCommand(const uint8_t* cmd)
{
    int deviceCount;
    S2S_Device** devices = s2s_GetDevices(&deviceCount);
    
    uint32_t sectorNum =
        ((uint32_t)cmd[2]) << 24 |
        ((uint32_t)cmd[3]) << 16 |
        ((uint32_t)cmd[4]) << 8 |
        ((uint32_t)cmd[5]);

    devices[cmd[1]]->write(devices[cmd[1]], sectorNum, 1, &cmd[6]);
    
	uint8_t response[] =
	{
		CONFIG_STATUS_GOOD
	};
    hidPacket_send(response, sizeof(response));
}


static void
deviceReadCommand(const uint8_t* cmd)
{
    int deviceCount;
    S2S_Device** devices = s2s_GetDevices(&deviceCount);
    
    uint32_t sectorNum =
        ((uint32_t)cmd[2]) << 24 |
        ((uint32_t)cmd[3]) << 16 |
        ((uint32_t)cmd[4]) << 8 |
        ((uint32_t)cmd[5]);

    uint32_t response[512];
    devices[cmd[1]]->read(devices[cmd[1]], sectorNum, 1, &response[0]);
    
    hidPacket_send(&response[0], 512);
}

static void
processCommand(const uint8_t* cmd, size_t cmdSize)
{
	switch (cmd[0])
	{
	case CONFIG_PING:
		pingCommand();
		break;

	case CONFIG_READFLASH:
		readFlashCommand(cmd, cmdSize);
		break;

	case CONFIG_WRITEFLASH:
		writeFlashCommand(cmd, cmdSize);
		break;

	case CONFIG_REBOOT:
		Bootloadable_1_Load();
		break;

	case CONFIG_SDINFO:
		sdInfoCommand();
		break;

	case CONFIG_SCSITEST:
		scsiTestCommand();
		break;

    case S2S_CMD_DEV_LIST:
        deviceListCommand();
        break;

    case S2S_CMD_DEV_ERASE:
        deviceEraseCommand(cmd);
        break;

    case S2S_CMD_DEV_WRITE:
        deviceWriteCommand(cmd);
        break;

    case S2S_CMD_DEV_READ:
        deviceReadCommand(cmd);
        break;
        
	case CONFIG_NONE: // invalid
	default:
		break;
	}
}

void configPoll()
{
	int reset = 0;
	if (!usbReady || USBFS_IsConfigurationChanged())
	{
		reset = 1;
	}
	usbReady = USBFS_bGetConfiguration();

	if (!usbReady)
	{
		return;
	}

	if (reset)
	{
		USBFS_EnableOutEP(USB_EP_OUT);
		USBFS_EnableOutEP(USB_EP_COMMAND);
		usbInEpState = usbDebugEpState = USB_IDLE;
	}

	if(USBFS_GetEPState(USB_EP_OUT) == USBFS_OUT_BUFFER_FULL)
	{
		ledOn();

		// The host sent us some data!
		int byteCount = USBFS_GetEPCount(USB_EP_OUT);
		USBFS_ReadOutEP(USB_EP_OUT, hidBuffer, sizeof(hidBuffer));
		hidPacket_recv(hidBuffer, byteCount);

		size_t cmdSize;
		const uint8_t* cmd = hidPacket_getPacket(&cmdSize);
		if (cmd && (cmdSize > 0))
		{
			processCommand(cmd, cmdSize);
		}

		// Allow the host to send us another updated config.
		USBFS_EnableOutEP(USB_EP_OUT);

		ledOff();
	}

	switch (usbInEpState)
	{
	case USB_IDLE:
		{
			const uint8_t* nextChunk = hidPacket_getHIDBytes(hidBuffer);

			if (nextChunk)
			{
				USBFS_LoadInEP(USB_EP_IN, nextChunk, sizeof(hidBuffer));
				usbInEpState = USB_DATA_SENT;
			}
		}
		break;

	case USB_DATA_SENT:
		if (USBFS_bGetEPAckState(USB_EP_IN))
		{
			// Data accepted.
			usbInEpState = USB_IDLE;
		}
		break;
	}
}

void debugPoll()
{
	if (!usbReady)
	{
		return;
	}

	if(USBFS_GetEPState(USB_EP_COMMAND) == USBFS_OUT_BUFFER_FULL)
	{
		// The host sent us some data!
		int byteCount = USBFS_GetEPCount(USB_EP_COMMAND);
		USBFS_ReadOutEP(USB_EP_COMMAND, (uint8 *)&hidBuffer, byteCount);

		if (byteCount >= 1 &&
			hidBuffer[0] == 0x01)
		{
			// Reboot command.
			Bootloadable_1_Load();
		}

		// Allow the host to send us another command.
		// (assuming we didn't reboot outselves)
		USBFS_EnableOutEP(USB_EP_COMMAND);
	}

	switch (usbDebugEpState)
	{
	case USB_IDLE:
		memcpy(&hidBuffer, &scsiDev.cdb, 12);
		hidBuffer[12] = scsiDev.msgIn;
		hidBuffer[13] = scsiDev.msgOut;
		hidBuffer[14] = scsiDev.lastStatus;
		hidBuffer[15] = scsiDev.lastSense;
		hidBuffer[16] = scsiDev.phase;
		hidBuffer[17] = SCSI_ReadFilt(SCSI_Filt_BSY);
		hidBuffer[18] = SCSI_ReadFilt(SCSI_Filt_SEL);
		hidBuffer[19] = SCSI_ReadFilt(SCSI_Filt_ATN);
		hidBuffer[20] = SCSI_ReadFilt(SCSI_Filt_RST);
		hidBuffer[21] = scsiDev.rstCount;
		hidBuffer[22] = scsiDev.selCount;
		hidBuffer[23] = scsiDev.msgCount;
		hidBuffer[24] = scsiDev.cmdCount;
		hidBuffer[25] = scsiDev.watchdogTick;
		hidBuffer[26] = 0; // OBSOLETE. Previously media state
		hidBuffer[27] = scsiDev.lastSenseASC >> 8;
		hidBuffer[28] = scsiDev.lastSenseASC;
		hidBuffer[29] = scsiReadDBxPins();
		hidBuffer[30] = LastTrace;

		hidBuffer[58] = sdCard.capacity >> 24;
		hidBuffer[59] = sdCard.capacity >> 16;
		hidBuffer[60] = sdCard.capacity >> 8;
		hidBuffer[61] = sdCard.capacity;

		hidBuffer[62] = FIRMWARE_VERSION >> 8;
		hidBuffer[63] = FIRMWARE_VERSION;

		USBFS_LoadInEP(USB_EP_DEBUG, (uint8 *)&hidBuffer, sizeof(hidBuffer));
		usbDebugEpState = USB_DATA_SENT;
		break;

	case USB_DATA_SENT:
		if (USBFS_bGetEPAckState(USB_EP_DEBUG))
		{
			// Data accepted.
			usbDebugEpState = USB_IDLE;
		}
		break;
	}
}

CY_ISR(debugTimerISR)
{
	Debug_Timer_ReadStatusRegister();
	Debug_Timer_Interrupt_ClearPending();
	uint8 savedIntrStatus = CyEnterCriticalSection();
	debugPoll();
	CyExitCriticalSection(savedIntrStatus);
}

void debugInit()
{
	Debug_Timer_Interrupt_StartEx(debugTimerISR);
	Debug_Timer_Start();
}

void debugPause()
{
	Debug_Timer_Stop();
}

void debugResume()
{
	Debug_Timer_Start();
}

int isDebugEnabled()
{
	return usbReady;
}

// Public method for storing MODE SELECT results.
void configSave(int scsiId, uint16_t bytesPerSector)
{
	int cfgIdx;
	for (cfgIdx = 0; cfgIdx < MAX_SCSI_TARGETS; ++cfgIdx)
	{
		const S2S_TargetCfg* tgt = getConfigByIndex(cfgIdx);
		if ((tgt->scsiId & CONFIG_TARGET_ID_BITS) == scsiId)
		{
			// Save row to flash
			// We only save the first row of the configuration
			// this contains the parameters changeable by a MODE SELECT command
			uint8_t rowData[CYDEV_FLS_ROW_SIZE];
			S2S_TargetCfg* rowCfgData = (S2S_TargetCfg*)&rowData;
			memcpy(rowCfgData, tgt, sizeof(rowData));
			rowCfgData->bytesPerSector = bytesPerSector;

			CySetTemp();
			CyWriteRowData(
				SCSI_CONFIG_ARRAY,
				SCSI_CONFIG_0_ROW + (cfgIdx * SCSI_CONFIG_ROWS),
				(uint8_t*)rowCfgData);
			return;
		}
	}
}


const S2S_TargetCfg* getConfigByIndex(int i)
{
	if (i <= 3)
	{
		size_t row = SCSI_CONFIG_0_ROW + (i * SCSI_CONFIG_ROWS);
		return (const S2S_TargetCfg*)
			(
				CY_FLASH_BASE +
				(CY_FLASH_SIZEOF_ARRAY * (size_t) SCSI_CONFIG_ARRAY) +
				(CY_FLASH_SIZEOF_ROW * row)
				);
	} else {
		size_t row = SCSI_CONFIG_4_ROW + ((i-4) * SCSI_CONFIG_ROWS);
		return (const S2S_TargetCfg*)
			(
				CY_FLASH_BASE +
				(CY_FLASH_SIZEOF_ARRAY * (size_t) SCSI_CONFIG_ARRAY) +
				(CY_FLASH_SIZEOF_ROW * row)
				);
	}
}

const S2S_TargetCfg* getConfigById(int scsiId)
{
	int i;
	for (i = 0; i < MAX_SCSI_TARGETS; ++i)
	{
		const S2S_TargetCfg* tgt = getConfigByIndex(i);
		if ((tgt->scsiId & CONFIG_TARGET_ID_BITS) == scsiId)
		{
			return tgt;
		}
	}
	return NULL;

}
