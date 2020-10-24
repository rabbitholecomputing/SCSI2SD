# USB HID Documentation for SCSI2SD v5.x
-

This document contains a technical description of the format that is sent to the device when the data is extracted from the XML document or from the GUI.

## Board Configuration information:
The board config for the device is a packed decimal structure defined by the declaration below:

```C++
typedef struct __attribute__((packed)) {
	char magic[4]; // 'BCFG'
	uint8_t flags; // CONFIG_FLAGS
	uint8_t startupDelay; // Seconds.
	uint8_t selectionDelay; // milliseconds. 255 = auto
	uint8_t flags6; // CONFIG_FLAGS6
	uint8_t scsiSpeed;
	uint8_t reserved[247]; // Pad out to 256 bytes
} BoardConfig;
```

The board configuration determines general configuration parameters for the behavior of the SCSI emulation

* flags - contains the following information
	* bit 0: UNIT ATTENTION
	* bit 1: PARITY
	* bit 2: SCSI2
	* bit 3: GLITCH
	* bit 4: CACHE
	* bit 5: DISCONNECT
	* bit 6: SELECT LATCH
	* bit 7: LOGICAL UNITS to IDs
* scsiSpeed - Defines the speed of the SCSI interface: 0/1 - Normal and slow
* startUpDelay - 0-255
* configFlags6 is not set
* reserved is filled with NULL
* termination is or'd with the bits above.

## Device Configuration Information
The device configuration is a packed decimal structure which dictates the information for each individual virtual device.

```C++
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

	uint8_t reserved[960]; // Pad out to 1024 bytes for main section.

	uint8_t modePages[1024];
	uint8_t vpd[1024];
	uint8_t unused[1024]; // Total size is 4k.
} TargetConfig;
```

* scsiId - the SCSI ID for the device 0-7
* deviceType - 
	* 1 - Hard Drive
	* 2 - Removable
	* 3 - CD ROM
	* 4 - 3.5" Floppy Drive
	* 5 - Magneto Optical
* deviceTypeModifier - not set from GUI
* scsiSectors - the number of sectors on the device
* bytesPerSector - number of bytes per sector
* sectorsPerTrack - number of sectures per track
* headsPerCylinder - 0-255 heads
* vendor - freeform text field
* prodId - freeform text field
* revision - freeform text field
* serial - freeform text field

* quirks:
```C
typedef enum
{
	CONFIG_QUIRKS_NONE = 0,
	CONFIG_QUIRKS_APPLE = 1,
	CONFIG_QUIRKS_OMTI = 2,
	CONFIG_QUIRKS_XEBEC = 4
} CONFIG_QUIRKS;
```

These structures should allow any device to configure the device so long as it can access the USB HID directly.  This document will be updated periodically as the specification changes. 

## Memory map:

### Location in memory on device

The information written above on the flash on the device is used on the device in the following way:

* The base address of the Cypress FLASH is defined in the code by CYDEV_FLASH_BASE.  This is where
the configuration data resides and where the SCSI.c code which is running on the device reads it.  The main.c file in the PSoC creator project runs through the SCSI processes in an infinite loop and pulls information from this section as needed.

```
// CYDEF_FLASH_BASE starts at 0x00000000u.

// Starting from this base, the adresses are as follows:
0x00000000u - 0x00000003u -> 'BCFG' // board configuration data
0x00000004u - 0x00000004u -> flags - as defined in the structure BoardCfg
0x00000005u - 0x00000005u -> 0-255 - startupDelay
0x00000006u - 0x00000006u -> 0-255 - selectionDelay
0x00000007u - 0x00000007u -> 0-255 - flags6, not currently used for v5.x
0x00000008u - 0x00000008u -> 0-255 - scsiSpeed
0x00000009u - 0x00000100u -> 0 -- filler, nulls (247 of them)

// Target configuration
0x00000101u - 0x00000101u -> scsiId - 0-4
0x00000102u - 0x00000102u -> deviceType
0x00000103u - 0x00000103u -> flagsDEPRECATED
0x00000104u - 0x00000104u -> deviceTypeModifier
0x00000105u - 0x00000108u -> sdSectorStart // 32bit integer
0x00000109u - 0x0000010cu -> scsiSectors // 32bit integer
0x0000010du - 0x0000010eu -> bytesPerSector // 16 bits
0x0000010fu - 0x00000110u -> sectorsPerTrack // 16 bits
0x00000111u - 0x00000112u -> headsPerCylinder // 16 bits
0x00000113u - 0x0000011eu -> vendor
0x0000011fu - 0x0000012fu -> prodId
0x0000012fu - 0x00000133u -> revision
0x00000133u - 0x00000134u -> serial
0x0000013fu - 0x00000140u -> quirks   // all of the above should add up to 64 bytes
0x00000140u - 0x00000400u -> // pad out to 1024
0x00000401u - 0x00000800u -> modePages
0x00000801u - 0x00001000u -> vpd
0x00001000u - 0x00001400u -> unused

// Target config repeats 3 more times beginning at 1401... etc  Each TargetConfig block is 4K long precisely.
```

The board reads this information directly from the flash memory by applying the BoardConfig and TargetConfig structures.  This information is used in both the main.c and scsi.c/h code primarily to determine the behavior of the hardware.

GC



