//	Copyright (C) 2013 Michael McMaster <michael@codesrc.com>
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
#include "flash.h"

#include "config.h"
#include "led.h"
#include "time.h"

typedef struct
{
	S2S_Device dev;

	S2S_Target targets[MAX_SCSI_TARGETS];
    
    uint32_t capacity; // in 512 byte blocks
    
    // CFI info
    uint8_t manufacturerID;
    uint8_t deviceID[2];
    

} SpiFlash;

static void spiFlash_earlyInit(S2S_Device* dev);
static void spiFlash_init(S2S_Device* dev);
static S2S_Target* spiFlash_getTargets(S2S_Device* dev, int* count);
static uint32_t spiFlash_getCapacity(S2S_Device* dev);
static int spiFlash_pollMediaChange(S2S_Device* dev);
static void spiFlash_pollMediaBusy(S2S_Device* dev);

SpiFlash spiFlash = {
	{
		spiFlash_earlyInit,
        spiFlash_init,
		spiFlash_getTargets,
		spiFlash_getCapacity,
		spiFlash_pollMediaChange,
		spiFlash_pollMediaBusy,
        0, // initial mediaState
        CONFIG_STOREDEVICE_FLASH
	}
};

S2S_Device* spiFlashDevice = &(spiFlash.dev);

// Read and write 1 byte.
static uint8_t spiFlashByte(uint8_t value)
{
	NOR_SPI_WriteTxData(value);
	while (!(NOR_SPI_ReadRxStatus() & NOR_SPI_STS_RX_FIFO_NOT_EMPTY)) {}
	return NOR_SPI_ReadRxData();
}

static void spiFlash_earlyInit(S2S_Device* dev)
{
	SpiFlash* spiFlash = (SpiFlash*)dev;

	for (int i = 0; i < MAX_SCSI_TARGETS; ++i)
	{
		spiFlash->targets[i].device = dev;
        
        const S2S_TargetCfg* cfg = getConfigByIndex(i);
        if (cfg->storageDevice == CONFIG_STOREDEVICE_FLASH)
        {
		    spiFlash->targets[i].cfg = (S2S_TargetCfg*)cfg;
        }
        else
        {
            spiFlash->targets[i].cfg = NULL;
        }
	}

	// Don't require the host to send us a START STOP UNIT command
	spiFlash->dev.mediaState = MEDIA_STARTED;
}

static void spiFlash_init(S2S_Device* dev)
{
    SpiFlash* spiFlash = (SpiFlash*)dev;
    spiFlash->capacity = 0;

    nNOR_WP_Write(0); // Enable Write Protect
    nNOR_CS_Write(1); // Deselect
    
    NOR_SPI_Start();
    CyDelayUs(1);

    nNOR_CS_Write(0); // Select
    
    // JEDEC standard "Read Identification" command
    // returns CFI information
    spiFlashByte(0x9F);
    
    // 1 byte manufacturer ID
    spiFlash->manufacturerID = spiFlashByte(0xFF);
    
    // 2 bytes device ID
    spiFlash->deviceID[0] = spiFlashByte(0xFF);
    spiFlash->deviceID[1] = spiFlashByte(0xFF);

    uint8_t bytesFollowing = spiFlashByte(0xFF);
    
    // Chances are this says 0, which means up to 512 bytes.
    // But ignore it for now and just get the capacity.
    for (int i = 0; i < 0x23; ++i)
    {
        spiFlashByte(0xFF);
    }
    
    // Capacity is 2^n at offset 0x27
    //spiFlash->capacity = (1 << spiFlashByte(0xFF)) / 512;
    // Record value in 512-byte sectors.
    spiFlash->capacity = 1 << (spiFlashByte(0xFF) - 9);

    // Don't bother reading the rest. Deselecting will cancel the command.
    
    nNOR_CS_Write(1); // Deselect

}

static S2S_Target* spiFlash_getTargets(S2S_Device* dev, int* count)
{
	SpiFlash* spiFlash = (SpiFlash*)dev;
	*count = MAX_SCSI_TARGETS;
	return spiFlash->targets;
}

static uint32_t spiFlash_getCapacity(S2S_Device* dev)
{
	SpiFlash* spiFlash = (SpiFlash*)dev;
	return spiFlash->capacity;
}

static int spiFlash_pollMediaChange(S2S_Device* dev)
{
    // Non-removable
    return 0;
}

static void spiFlash_pollMediaBusy(S2S_Device* dev)
{
	// Non-removable
}

