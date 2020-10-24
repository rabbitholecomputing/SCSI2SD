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

These structures should allow any device to configure the device so long as it can access the USB HID directly.  This document will be updated periodically as the specification changes. GC