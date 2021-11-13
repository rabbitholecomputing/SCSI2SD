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
#ifndef scsi2sd_h
#define scsi2sd_h

#ifdef __cplusplus
extern "C" {
#endif

/* Common type definitions shared between the firmware and config tools

	The configuration data is stored in flash.

	The flash is organised as 2 arrays of 256 rows, with each row
	having 256 bytes. Total of 128kb.

	Linear flash memory map:
	-----------------------------------------
	Array 1 |Row 255 | Bootloader metadata
			---------------------------------
			|Row 254 |
			|Row 252 | Blank
			---------------------------------
			|Row 251 |
			| ...    |
			|Row 236 | Config target 3
			| ...    |
			|Row 220 | Config target 2
			| ...    |
			|Row 204 | Config target 1
			| ...    |
			|Row 188 | Config target 0
			---------------------------------
			|Row 187 | Board Config
			| ...    | Empty
			|Row 172 | Empty
			---------------------------------
			|Row 171 |
			| ...    |
			|Row 156 | Config target 6
			| ...    |
			|Row 140 | Config target 5
			| ...    |
			|Row 124 | Config target 4
			---------------------------------
			|Row 123 |
			| ...    |
			|Row 0   |
	--------|        |
	Array 0	|Row 255 | Blank
			---------------------------------
			|Row 121 |
			| ...    |
			|Row 37  | Application
			---------------------------------
			|Row 36  |
			| ...    |
			|Row 0   | Bootloader

*/

#include "stdint.h"

//#define MAX_SCSI_TARGETS 7
#define MAX_SCSI_TARGETS 4
#define SCSI_CONFIG_ARRAY 1
#define SCSI_CONFIG_ROWS 16

// 256 bytes data, 32 bytes ECC
#define SCSI_CONFIG_ROW_SIZE 256
#define SCSI_CONFIG_ROW_ECC 288
#define SCSI_CONFIG_BOARD_ROW 187
#define SCSI_CONFIG_0_ROW 188
#define SCSI_CONFIG_1_ROW 204
#define SCSI_CONFIG_2_ROW 220
#define SCSI_CONFIG_3_ROW 236
#define SCSI_CONFIG_4_ROW 124
#define SCSI_CONFIG_5_ROW 140
#define SCSI_CONFIG_6_ROW 156

typedef enum
{
	CONFIG_TARGET_ID_BITS = 0x07,
	CONFIG_TARGET_ENABLED = 0x80
} CONFIG_TARGET_FLAGS;

typedef enum
{
	CONFIG_ENABLE_UNIT_ATTENTION = 1,
	CONFIG_ENABLE_PARITY = 2,
	CONFIG_ENABLE_SCSI2 = 4,
	CONFIG_DISABLE_GLITCH = 8,
	CONFIG_ENABLE_CACHE = 16,
	CONFIG_ENABLE_DISCONNECT = 32,
	CONFIG_ENABLE_SEL_LATCH = 64,
	CONFIG_MAP_LUNS_TO_IDS = 128
} CONFIG_FLAGS;

typedef enum
{
	S2S_CFG_ENABLE_TERMINATOR = 1
} CONFIG_FLAGS6;

typedef enum
{
	CONFIG_FIXED,
	CONFIG_REMOVEABLE,
	CONFIG_OPTICAL,
	CONFIG_FLOPPY_14MB,
	CONFIG_MO,
	CONFIG_SEQUENTIAL

} CONFIG_TYPE;

typedef enum
{
	CONFIG_QUIRKS_NONE = 0,
	CONFIG_QUIRKS_APPLE = 1,
	CONFIG_QUIRKS_OMTI = 2,
	CONFIG_QUIRKS_XEBEC = 4,
	CONFIG_QUIRKS_VMS = 8
} CONFIG_QUIRKS;

typedef enum
{
	CONFIG_SPEED_NoLimit,
	CONFIG_SPEED_ASYNC_15
} CONFIG_SPEED;

typedef enum
{
	CONFIG_STOREDEVICE_SD,
	CONFIG_STOREDEVICE_FLASH
} CONFIG_STOREDEVICE;

typedef struct __attribute__((packed))
{
	uint8_t deviceType;
	uint8_t pageCode;
	uint8_t reserved;
	uint8_t pageLength;
	uint8_t data[0]; // pageLength bytes.
} VPD;

typedef struct __attribute__((packed))
{
	// bits 7 -> 3 = CONFIG_TARGET_FLAGS
	// bits 2 -> 0 = target SCSI ID.
	uint8_t scsiId;

	uint8_t deviceType; // CONFIG_TYPE
	uint8_t flagsDEPRECATED; // CONFIG_FLAGS, removed in v4.5
	uint8_t deviceTypeModifier; // Used in INQUIRY response.

	uint32_t sdSectorStart;
	uint32_t scsiSectors;

	uint16_t bytesPerSector;

	// Max allowed by legacy IBM-PC bios is 6 bits (63)
	uint16_t sectorsPerTrack;

	// MS-Dos up to 7.10 will crash on >= 256 heads.
	uint16_t headsPerCylinder;


	char vendor[8];
	char prodId[16];
	char revision[4];
	char serial[16];

	uint16_t quirks; // CONFIG_QUIRKS
    
    // 0 == SD card
    // 1 == SPI Flash
    uint8_t storageDevice; // CONFIG_STOREDEVICE

	uint8_t reserved[959]; // Pad out to 1024 bytes for main section.

	uint8_t modePages[1024];
	uint8_t vpd[1024];
	uint8_t unused[1024]; // Total size is 4k.
} S2S_TargetCfg;

typedef struct __attribute__((packed))
{
	char magic[4]; // 'BCFG'
	uint8_t flags; // CONFIG_FLAGS
	uint8_t startupDelay; // Seconds.
	uint8_t selectionDelay; // milliseconds. 255 = auto
	uint8_t flags6; // CONFIG_FLAGS6

	uint8_t scsiSpeed;


	uint8_t reserved[247]; // Pad out to 256 bytes
} S2S_BoardConfig;

typedef enum
{
	CONFIG_NONE, // Invalid

	// Command content:
	// uint8_t CONFIG_PING
	// Response:
	// CONFIG_STATUS
	CONFIG_PING,

	// Command content:
	// uint8_t CONFIG_WRITEFLASH
	// uint8_t[256] flashData
	// uint8_t flashArray
	// uint8_t flashRow
	// Response:
	// CONFIG_STATUS
	CONFIG_WRITEFLASH,

	// Command content:
	// uint8_t CONFIG_READFLASH
	// uint8_t flashArray
	// uint8_t flashRow
	// Response:
	// 256 bytes of flash
	CONFIG_READFLASH,

	// Command content:
	// uint8_t CONFIG_REBOOT
	// Response: None.
	CONFIG_REBOOT,

	// Command content:
	// uint8_t CONFIG_INFO
	// Response:
	// uint8_t[16] CSD
	// uint8_t[16] CID
	CONFIG_SDINFO,

	// Command content:
	// uint8_t CONFIG_SCSITEST
	// Response:
	// CONFIG_STATUS
	// uint8_t result code (0 = passed)
	CONFIG_SCSITEST,
    
    // Not implemented, V6 only
    // Command content:
    // uint8_t S2S_CMD_DEVINFO
    // Response:
    // uint16_t protocol version (MSB)
    // uint16_t firmware version (MSB)
    // uint32_t SD capacity(MSB)
    S2S_CMD_DEVINFO_OBSOLETE,

    // Not implemented, V6 only
    // Command content:
    // uint8_t S2S_CMD_SD_WRITE
    // uint32_t Sector Number (MSB)
    // uint8_t[512] data
    // Response:
    // S2S_CMD_STATUS
    S2S_CMD_SD_WRITE,

    // Not implemented, V6 only
    // Command content:
    // uint8_t S2S_CMD_SD_READ
    // uint32_t Sector Number (MSB)
    // Response:
    // 512 bytes of data
    S2S_CMD_SD_READ,

    // Not implemented, V6 only
    // Command content:
    // uint8_t S2S_CMD_DEBUG
    // Response:
    S2S_CMD_DEBUG,
    
    // Command content:
    // uint8_t S2S_CMD_DEV_LIST
    // Response:
    // uint8_t Number of devices
    // For each device:
    // uint8_t device type
    //  0 == SD card
    //  1 == NOR FLASH
    // uint32_t capacity(MSB)
    S2S_CMD_DEV_LIST,
    
    // Command content:
    // uint8_t S2S_CMD_DEV_INFO
    // uint8_t Device Number
    // Response:
    //  SD card:
    //    uint8_t[16] CSD
	//    uint8_t[16] CID
    //  NOR Flash: 
    //    uint8_t[512] JEDEC CFI from RDID command
    S2S_CMD_DEV_INFO,
    
    // Command content:
    // uint8_t S2S_CMD_DEV_ERASE
    // uint8_t Device Number
    // uint32_t Sector Number (MSB)
    // uint32_t Sector Count (MSB)
    // Response:
    // S2S_CMD_STATUS
    S2S_CMD_DEV_ERASE,
    
    // Command content:
    // uint8_t S2S_CMD_DEV_WRITE
    // uint8_t Device Number (MSB)
    // uint32_t Sector Number (MSB)
    // uint8_t[512] data
    // Response:
    // S2S_CMD_STATUS
    S2S_CMD_DEV_WRITE,

    // Command content:
    // uint8_t S2S_CMD_DEV_READ
    // uint8_t Device Number (MSB)
    // uint32_t Sector Number (MSB)
    // Response:
    // 512 bytes of data
    S2S_CMD_DEV_READ,
} CONFIG_COMMAND;

typedef enum
{
	CONFIG_STATUS_GOOD,
	CONFIG_STATUS_ERR
} CONFIG_STATUS;



#define TargetConfig S2S_TargetCfg
#define BoardConfig S2S_BoardConfig

#ifdef __cplusplus
} // extern "C"

	#include <type_traits>
	static_assert(
		std::is_pod<S2S_TargetCfg>::value, "Misuse of TargetConfig struct"
		);
	static_assert(
		sizeof(S2S_TargetCfg) == 4096,
		"TargetConfig struct size mismatch"
		);

#endif

#endif
