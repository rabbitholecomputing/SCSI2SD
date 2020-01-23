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

#ifndef SCSI2SD_HID_H
#define SCSI2SD_HID_H

#include "hidapi.h"

#if __cplusplus >= 201103L
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <string>
#include <vector>

namespace SCSI2SD
{

class HID
{
public:
	static HID* Open();

	~HID();

	uint16_t getFirmwareVersion() const { return myFirmwareVersion; }
	std::string getFirmwareVersionStr() const;
	uint32_t getSDCapacity() const { return mySDCapacity; }
	std::vector<uint8_t> getSD_CSD();
	std::vector<uint8_t> getSD_CID();

	bool scsiSelfTest(int& code);

	void enterBootloader();

	void readSector(uint32_t sector, std::vector<uint8_t>& out);
	void writeSector(uint32_t sector, const std::vector<uint8_t>& in);
	bool ping();

	bool readSCSIDebugInfo(std::vector<uint8_t>& buf);

private:
	HID(hid_device_info* hidInfo);
	void destroy();
	void readNewDebugData();
	void readDebugData();
	void readHID(uint8_t* buffer, size_t len);
	void sendHIDPacket(
		const std::vector<uint8_t>& cmd,
		std::vector<uint8_t>& out,
		size_t responseLength
		);

	hid_device_info* myHidInfo;
	hid_device* myConfigHandle;

	// Read-only data from the debug interface.
	uint16_t myFirmwareVersion;
	uint32_t mySDCapacity;
};

} // namespace

#endif
