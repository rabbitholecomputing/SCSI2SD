//    Copyright (C) 2013 Michael McMaster <michael@codesrc.com>
//
//    This file is part of SCSI2SD.
//
//    SCSI2SD is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    SCSI2SD is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with SCSI2SD.  If not, see <http://www.gnu.org/licenses/>.

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
static void spiFlash_erase(S2S_Device* dev, uint32_t sectorNumber, uint32_t count);
static void spiFlash_read(S2S_Device* dev, uint32_t sectorNumber, uint32_t count, uint8_t* buffer);
static void spiFlash_write(S2S_Device* dev, uint32_t sectorNumber, uint32_t count, uint8_t* buffer);

SpiFlash spiFlash = {
    {
        spiFlash_earlyInit,
        spiFlash_init,
        spiFlash_getTargets,
        spiFlash_getCapacity,
        spiFlash_pollMediaChange,
        spiFlash_pollMediaBusy,
        spiFlash_erase,
        spiFlash_read,
        spiFlash_write,
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

    nNOR_WP_Write(1); // We don't need write Protect
    nNOR_CS_Write(1); // Deselect
    
    NOR_SPI_Start();
    CyDelayUs(1);

    nNOR_CS_Write(0); // Select
    CyDelayCycles(4); // Tiny delay
    
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
    
    if (spiFlash->capacity > 0)
    {
        spiFlash->dev.mediaState |= MEDIA_PRESENT | MEDIA_INITIALISED;
    }

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

static void spiFlash_WaitForWIP()
{
    int inProgress = 1;
    while (inProgress)
    {
        nNOR_CS_Write(0);
        uint8_t status = spiFlashByte(0x05); // Read Status Register 1;
        inProgress = status & 1;
        nNOR_CS_Write(1);
    }
}

static void spiFlash_erase(S2S_Device* dev, uint32_t sectorNumber, uint32_t count)
{
    // SpiFlash* spiFlash = (SpiFlash*)dev;

    nNOR_CS_Write(0); // Select

    // Send the WREN - Write Enable command
    spiFlashByte(0x06);

    // We NEED to deselect the device now for writes to work
    nNOR_CS_Write(1);
    
    // For now we assume 256kb sectors. This needs to be expanded to cater for 
    // different sector sizes. We safely assume it will always be >= 512 bytes.
    const uint32_t flashSectorSize = 256*1024;

    // We don't have enough memory to do a read-modify-write cycle, so the caller
    // had better line these up on sector boundaries.
    for (uint32_t linearAddress = sectorNumber * 512;
        linearAddress < (sectorNumber + count) * 512;
        linearAddress += flashSectorSize)
    {
        nNOR_CS_Write(0);

        spiFlashByte(0xDC);

        // 4-byte address
        spiFlashByte(linearAddress >> 24);
        spiFlashByte(linearAddress >> 16);
        spiFlashByte(linearAddress >> 8);
        spiFlashByte(linearAddress);

        // Initiate erase
        nNOR_CS_Write(1);

        spiFlash_WaitForWIP();
    }

    nNOR_CS_Write(0);

    // Send the WREN - Write Disable command
    spiFlashByte(0x04);
    
    nNOR_CS_Write(1); // Deselect
}

static void spiFlash_write(S2S_Device* dev, uint32_t sectorNumber, uint32_t count, uint8_t* buffer)
{
    // SpiFlash* spiFlash = (SpiFlash*)dev;

    nNOR_CS_Write(0); // Select

    // Send the WREN - Write Enable command
    spiFlashByte(0x06);

    // We NEED to deselect the device now for writes to work
    nNOR_CS_Write(1);
    
    // We're assuming here that the page size is 512 bytes or more.
    for (unsigned int i = 0; i < count; ++i)
    {
        nNOR_CS_Write(0);

        spiFlashByte(0x12);

        uint32_t linearAddress = (sectorNumber + i) * 512;
        spiFlashByte(linearAddress >> 24);
        spiFlashByte(linearAddress >> 16);
        spiFlashByte(linearAddress >> 8);
        spiFlashByte(linearAddress);

        for (int off = 0; off < 512; ++off)
        {
            spiFlashByte(buffer[i * 512 + off]);
        }

        // Initiate write 
        nNOR_CS_Write(1);

        spiFlash_WaitForWIP();
    }

    nNOR_CS_Write(0);

    // Send the WREN - Write Disable command
    spiFlashByte(0x04);
    
    nNOR_CS_Write(1); // Deselect
}

static void spiFlash_read(S2S_Device* dev, uint32_t sectorNumber, uint32_t count, uint8_t* buffer)
{
    // SpiFlash* spiFlash = (SpiFlash*)dev;

    nNOR_CS_Write(0); // Select
    spiFlashByte(0x13);

    uint32_t linearAddress = sectorNumber * 512;
    spiFlashByte(linearAddress >> 24);
    spiFlashByte(linearAddress >> 16);
    spiFlashByte(linearAddress >> 8);
    spiFlashByte(linearAddress);

    // There's no harm in reading -extra- data, so keep the FIFO
    // one step ahead.
    NOR_SPI_WriteTxData(0xFF);
    NOR_SPI_WriteTxData(0xFF);
    NOR_SPI_WriteTxData(0xFF);

    for (int off = 0; off < count * 512; ++off)
    {
        NOR_SPI_WriteTxData(0xFF);

        while (!(NOR_SPI_ReadRxStatus() & NOR_SPI_STS_RX_FIFO_NOT_EMPTY)) {}
        buffer[off] = NOR_SPI_ReadRxData();
    }

    // Read and discard the extra bytes of data. It was only used to improve
    // performance with a full FIFO.
    for (int i = 0; i < 3; ++i)
    {
        while (!(NOR_SPI_ReadRxStatus() & NOR_SPI_STS_RX_FIFO_NOT_EMPTY)) {}
        NOR_SPI_ReadRxData();
    }

    nNOR_CS_Write(1); // Deselect
}

