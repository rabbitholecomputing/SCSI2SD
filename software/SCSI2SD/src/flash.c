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
static void spiFlash_readAsync(S2S_Device* dev, uint32_t sectorNumber, uint32_t count, uint8_t* buffer);
static int  spiFlash_readAsyncPoll(S2S_Device* dev);
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
        spiFlash_readAsync,
        spiFlash_readAsyncPoll,
        spiFlash_write,
        0, // initial mediaState
        CONFIG_STOREDEVICE_FLASH
    }
};

S2S_Device* spiFlashDevice = &(spiFlash.dev);

// Private DMA variables.
static uint8 spiFlashDMARxChan = CY_DMA_INVALID_CHANNEL;
static uint8 spiFlashDMATxChan = CY_DMA_INVALID_CHANNEL;
static uint8_t spiFlashDmaRxTd[2] = { CY_DMA_INVALID_TD, CY_DMA_INVALID_TD };
static uint8_t spiFlashDmaTxTd[2] = { CY_DMA_INVALID_TD, CY_DMA_INVALID_TD };

// Source of dummy SPI bytes for DMA
static uint8_t dummyBuffer[2]  __attribute__((aligned(4))) = {0xFF, 0xFF};
// Dummy location for DMA to sink usless data to
static uint8 discardBuffer[2] __attribute__((aligned(4)));


volatile uint8_t spiFlashRxDMAComplete = 1;
volatile uint8_t spiFlashTxDMAComplete = 1;

CY_ISR_PROTO(spiFlashRxISR);
CY_ISR(spiFlashRxISR)
{
	spiFlashRxDMAComplete = 1;
}
CY_ISR_PROTO(spiFlashTxISR);
CY_ISR(spiFlashTxISR)
{
	spiFlashTxDMAComplete = 1;
}

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
    spiFlash->dev.mediaState = MEDIA_STARTED | MEDIA_WP;
    
    // DMA stuff
	spiFlashDMATxChan =
		NOR_TX_DMA_DmaInitialize(
			2, // Bytes per burst
			1, // request per burst
			HI16(CYDEV_SRAM_BASE),
			HI16(CYDEV_PERIPH_BASE)
			);

	spiFlashDMARxChan =
		NOR_RX_DMA_DmaInitialize(
			1, // Bytes per burst
			1, // request per burst
			HI16(CYDEV_PERIPH_BASE),
			HI16(CYDEV_SRAM_BASE)
			);

	CyDmaChDisable(spiFlashDMATxChan);
	CyDmaChDisable(spiFlashDMARxChan);

	NOR_RX_DMA_COMPLETE_StartEx(spiFlashRxISR);
	NOR_TX_DMA_COMPLETE_StartEx(spiFlashTxISR);
    
    spiFlashDmaRxTd[0] = CyDmaTdAllocate();
    spiFlashDmaRxTd[1] = CyDmaTdAllocate();
    
    spiFlashDmaTxTd[0] = CyDmaTdAllocate();
    spiFlashDmaTxTd[1] = CyDmaTdAllocate();
}

static void spiFlash_init(S2S_Device* dev)
{
    SpiFlash* spiFlash = (SpiFlash*)dev;
    spiFlash->capacity = 0;

    nNOR_WP_Write(1); // We don't need write Protect
    nNOR_CS_Write(1); // Deselect
    
    NOR_SPI_Start();
    CyDelayUs(50); // tRPH is 35 uS. Wait a bit longer

    // Mode-bit-reset (go back to normal from high performance mode)
    nNOR_CS_Write(0); // Select
    CyDelayCycles(4); // Tiny delay
    spiFlashByte(0xFF);
    nNOR_CS_Write(1); // Deselect
    CyDelayCycles(4); // Tiny delay

    // Software-reset
    nNOR_CS_Write(0); // Select
    CyDelayCycles(4); // Tiny delay
    spiFlashByte(0xF0);
    nNOR_CS_Write(1); // Deselect - reset is triggered on the deselect
    CyDelayUs(50); // tRPH is 35 uS. Wait a bit longer

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

static void spiFlash_readAsync(S2S_Device* dev, uint32_t sectorNumber, uint32_t count, uint8_t* buffer)
{
    // SpiFlash* spiFlash = (SpiFlash*)dev;

    nNOR_CS_Write(0); // Select
    spiFlashByte(0x13);

    uint32_t linearAddress = sectorNumber * 512;
    
    // DMA implementation
    // send is static as the address must remain consistent for the static
	// DMA descriptors to work.
	// Size must be divisible by 2 to suit 2-byte-burst TX DMA channel.
	static uint8_t send[4] __attribute__((aligned(4)));
    send[0] = linearAddress >> 24;
    send[1] = linearAddress >> 16;
    send[2] = linearAddress >> 8;
    send[3] = linearAddress;
    
	// Prepare DMA transfer
    CyDmaTdSetConfiguration(spiFlashDmaTxTd[0], sizeof(send), spiFlashDmaTxTd[1], TD_INC_SRC_ADR);
    CyDmaTdSetAddress(spiFlashDmaTxTd[0], LO16((uint32)&send), LO16((uint32)NOR_SPI_TXDATA_PTR));
        
	CyDmaTdSetConfiguration(
		spiFlashDmaTxTd[1],
		count * 512,
		CY_DMA_DISABLE_TD, // Disable the DMA channel when TD completes count bytes
		NOR_TX_DMA__TD_TERMOUT_EN // Trigger interrupt when complete
		);
    CyDmaTdSetAddress(
		spiFlashDmaTxTd[1],
		LO16((uint32)&dummyBuffer),
		LO16((uint32)NOR_SPI_TXDATA_PTR));
    
    CyDmaTdSetConfiguration(spiFlashDmaRxTd[0], sizeof(send), spiFlashDmaRxTd[1], 0);
    CyDmaTdSetAddress(spiFlashDmaRxTd[0], LO16((uint32)NOR_SPI_RXDATA_PTR), LO16((uint32)&discardBuffer));
        
	CyDmaTdSetConfiguration(
		spiFlashDmaRxTd[1],
		count * 512,
		CY_DMA_DISABLE_TD, // Disable the DMA channel when TD completes count bytes
		TD_INC_DST_ADR |
			NOR_RX_DMA__TD_TERMOUT_EN // Trigger interrupt when complete
		);
	
	CyDmaTdSetAddress(
		spiFlashDmaRxTd[1],
		LO16((uint32)NOR_SPI_RXDATA_PTR),
		LO16((uint32)buffer)
		);

	CyDmaChSetInitialTd(spiFlashDMATxChan, spiFlashDmaTxTd[0]);
	CyDmaChSetInitialTd(spiFlashDMARxChan, spiFlashDmaRxTd[0]);

	// The DMA controller is a bit trigger-happy. It will retain
	// a drq request that was triggered while the channel was
	// disabled.
	CyDmaChSetRequest(spiFlashDMATxChan, CY_DMA_CPU_REQ);
	CyDmaClearPendingDrq(spiFlashDMARxChan);

	spiFlashTxDMAComplete = 0;
	spiFlashRxDMAComplete = 0;

	CyDmaChEnable(spiFlashDMARxChan, 1);
	CyDmaChEnable(spiFlashDMATxChan, 1);
}

static int spiFlash_readAsyncPoll(S2S_Device* dev)
{
    // SpiFlash* spiFlash = (SpiFlash*)dev;

    int allComplete = 0;
    uint8_t intr = CyEnterCriticalSection();
	allComplete = spiFlashTxDMAComplete && spiFlashRxDMAComplete;
	CyExitCriticalSection(intr);

    if (allComplete)
    {
        nNOR_CS_Write(1); // Deselect
    }
    
    return allComplete;
}
