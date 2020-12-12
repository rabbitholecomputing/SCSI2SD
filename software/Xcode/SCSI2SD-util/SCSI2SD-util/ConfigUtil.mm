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

#include "ConfigUtil.hh"

#include <limits>
#include <sstream>
#include <stdexcept>
#include <string.h>

#import <Foundation/NSXMLNode.h>
#import <Foundation/NSXMLDocument.h>
#import <Foundation/NSData.h>
#import <Foundation/NSString.h>
#import <Foundation/NSXMLElement.h>

#import "MF_Base64Additions.h"

using namespace SCSI2SD;

namespace
{
	// Endian conversion routines.
	// The Cortex-M3 inside the Cypress PSoC 5LP is a
	// little-endian device.

	bool isHostLE()
	{
		union
		{
			int i;
			char c[sizeof(int)];
		} x;
		x.i = 1;
		return (x.c[0] == 1);
	}

	uint16_t toLE16(uint16_t in)
	{
		if (isHostLE())
		{
			return in;
		}
		else
		{
			return (in >> 8) | (in << 8);
		}
	}
	uint16_t fromLE16(uint16_t in)
	{
		return toLE16(in);
	}

	uint32_t toLE32(uint32_t in)
	{
		if (isHostLE())
		{
			return in;
		}
		else
		{
			return (in >> 24) |
				((in >> 8) & 0xff00) |
				((in << 8) & 0xff0000) |
				(in << 24);
		}
	}
	uint32_t fromLE32(uint32_t in)
	{
		return toLE32(in);
	}

	std::vector<uint8_t> getModePages(const TargetConfig& cfg)
	{
		std::vector<uint8_t> result;
		unsigned i = 0;
		while (i < sizeof(cfg.modePages) - 2)
		{
			int pageLen = cfg.modePages[i+1];
			if (pageLen == 0) break;
			std::copy(
				&cfg.modePages[i],
				&cfg.modePages[i+pageLen+2],
				std::back_inserter(result));
			i += pageLen + 2;
		}
		return result;
	}

	std::vector<uint8_t> getVPDPages(const TargetConfig& cfg)
	{
		std::vector<uint8_t> result;
		unsigned i = 0;
		while (i < sizeof(cfg.vpd) - 4)
		{
			int pageLen = cfg.vpd[i+3];
			if (pageLen == 0) break;
			std::copy(
				&cfg.vpd[i],
				&cfg.vpd[i+pageLen+4],
				std::back_inserter(result));
			i += pageLen + 4;
		}
		return result;
	}
}

BoardConfig
ConfigUtil::DefaultBoardConfig()
{
	BoardConfig config;
	memset(&config, 0, sizeof(config));

	memcpy(config.magic, "BCFG", 4);


	// Default to maximum fail-safe options.
	config.flags = 0;
	config.flags6 = S2S_CFG_ENABLE_TERMINATOR;
	config.selectionDelay = 255; // auto

	return config;
}

TargetConfig
ConfigUtil::Default(size_t targetIdx)
{
	TargetConfig config;
	memset(&config, 0, sizeof(config));

	config.scsiId = targetIdx;
	if (targetIdx == 0)
	{
		config.scsiId = config.scsiId | CONFIG_TARGET_ENABLED;
	}
	config.deviceType = CONFIG_FIXED;

	// Default to maximum fail-safe options.
	config.flagsDEPRECATED = 0;
	config.deviceTypeModifier = 0;
	config.sdSectorStart = 0;

	// Default to 2GB. Many systems have trouble with > 2GB disks, and
	// a few start to complain at 1GB.
	config.scsiSectors = 4194303; // 2GB - 1 sector
	config.bytesPerSector = 512;
	config.sectorsPerTrack = 63;
	config.headsPerCylinder = 255;
	memcpy(config.vendor, " codesrc", 8);
	memcpy(config.prodId, "         SCSI2SD", 16);
	memcpy(config.revision, " 4.2", 4);
	memcpy(config.serial, "1234567812345678", 16);

	// Reserved fields, already set to 0
	// config.reserved

	// not supported yet.
	// config.vpd

	return config;
}


TargetConfig
ConfigUtil::fromBytes(const uint8_t* data)
{
	TargetConfig result;
	memcpy(&result, data, sizeof(TargetConfig));
	result.sdSectorStart = toLE32(result.sdSectorStart);
	result.scsiSectors = toLE32(result.scsiSectors);
	result.bytesPerSector = toLE16(result.bytesPerSector);
	result.sectorsPerTrack = toLE16(result.sectorsPerTrack);
	result.headsPerCylinder = toLE16(result.headsPerCylinder);
	return result;
}


std::vector<uint8_t>
ConfigUtil::toBytes(const TargetConfig& _config)
{
	TargetConfig config(_config);
	config.sdSectorStart = fromLE32(config.sdSectorStart);
	config.scsiSectors = fromLE32(config.scsiSectors);
	config.bytesPerSector = fromLE16(config.bytesPerSector);
	config.sectorsPerTrack = fromLE16(config.sectorsPerTrack);
	config.headsPerCylinder = fromLE16(config.headsPerCylinder);

	const uint8_t* begin = reinterpret_cast<const uint8_t*>(&config);
	return std::vector<uint8_t>(begin, begin + sizeof(config));
}

BoardConfig
ConfigUtil::boardConfigFromBytes(const uint8_t* data)
{
	BoardConfig result;
	memcpy(&result, data, sizeof(BoardConfig));

	if (memcmp("BCFG", result.magic, 4))
	{
		return DefaultBoardConfig();
	}

	return result;
}


std::vector<uint8_t>
ConfigUtil::boardConfigToBytes(const BoardConfig& _config)
{
	BoardConfig config(_config);

	memcpy(config.magic, "BCFG", 4);
	const uint8_t* begin = reinterpret_cast<const uint8_t*>(&config);
	return std::vector<uint8_t>(begin, begin + sizeof(config));
}

std::string
ConfigUtil::toXML(const TargetConfig& config)
{
	std::stringstream s;
	std::vector<uint8_t> modePages(getModePages(config));
	std::vector<uint8_t> vpd(getVPDPages(config));

	s <<
		"<SCSITarget id=\"" <<
			static_cast<int>(config.scsiId & CONFIG_TARGET_ID_BITS) << "\">\n" <<

		"	<enabled>" <<
			(config.scsiId & CONFIG_TARGET_ENABLED ? "true" : "false") <<
			"</enabled>\n" <<

		"\n" <<
		"	<!-- ********************************************************\n" <<
		"	Space separated list. Available options:\n" <<
		"	apple\t\tReturns Apple-specific mode pages\n" <<
		"	omti\t\tOMTI host non-standard link control\n" <<
		"	xebec\t\tXEBEC ignore step options in control byte\n" <<
		"	********************************************************* -->\n" <<
		"	<quirks>";
	if (config.quirks == CONFIG_QUIRKS_APPLE)
	{
		s << "apple";
	}
	else if (config.quirks == CONFIG_QUIRKS_OMTI)
	{
		s << "omti";
	}
	else if (config.quirks == CONFIG_QUIRKS_XEBEC)
	{
		s << "xebec";
	}

	s <<
			"</quirks>\n" <<

		"\n\n" <<
		"	<!-- ********************************************************\n" <<
		"	0x0    Fixed hard drive.\n" <<
		"	0x1    Removable drive.\n" <<
		"	0x2    Optical drive  (ie. CD drive).\n" <<
		"	0x3    1.44MB Floppy Drive.\n" <<
		"	********************************************************* -->\n" <<
		"	<deviceType>0x" <<
				std::hex << static_cast<int>(config.deviceType) <<
			"</deviceType>\n" <<

		"\n\n" <<
		"	<!-- ********************************************************\n" <<
		"	Device type modifier is usually 0x00. Only change this if your\n" <<
		"	OS requires some special value.\n" <<
		"\n" <<
		"	0x4C    Data General Micropolis disk\n" <<
		"	********************************************************* -->\n" <<
		"	<deviceTypeModifier>0x" <<
				std::hex << static_cast<int>(config.deviceTypeModifier) <<
			"</deviceTypeModifier>\n" <<

		"\n\n" <<
		"	<!-- ********************************************************\n" <<
		"	SD card offset, as a sector number (always 512 bytes).\n" <<
		"	********************************************************* -->\n" <<
		"	<sdSectorStart>" << std::dec << config.sdSectorStart << "</sdSectorStart>\n" <<
		"\n\n" <<
		"	<!-- ********************************************************\n" <<
		"	Drive geometry settings.\n" <<
		"	********************************************************* -->\n" <<
		"\n"
		"	<scsiSectors>" << std::dec << config.scsiSectors << "</scsiSectors>\n" <<
		"	<bytesPerSector>" << std::dec << config.bytesPerSector << "</bytesPerSector>\n" <<
		"	<sectorsPerTrack>" << std::dec << config.sectorsPerTrack<< "</sectorsPerTrack>\n" <<
		"	<headsPerCylinder>" << std::dec << config.headsPerCylinder << "</headsPerCylinder>\n" <<
		"\n\n" <<
		"	<!-- ********************************************************\n" <<
		"	Drive identification information. The SCSI2SD doesn't\n" <<
		"	care what these are set to. Use these strings to trick a OS\n" <<
		"	thinking a specific hard drive model is attached.\n" <<
		"	********************************************************* -->\n" <<
		"\n"
		"	<!-- 8 character vendor string -->\n" <<
		"	<!-- For Apple HD SC Setup/Drive Setup, use ' SEAGATE' -->\n" <<
		"	<vendor>" << std::string(config.vendor, 8) << "</vendor>\n" <<
		"\n" <<
		"	<!-- 16 character produce identifier -->\n" <<
		"	<!-- For Apple HD SC Setup/Drive Setup, use '          ST225N' -->\n" <<
		"	<prodId>" << std::string(config.prodId, 16) << "</prodId>\n" <<
		"\n" <<
		"	<!-- 4 character product revision number -->\n" <<
		"	<!-- For Apple HD SC Setup/Drive Setup, use '1.0 ' -->\n" <<
		"	<revision>" << std::string(config.revision, 4) << "</revision>\n" <<
		"\n" <<
		"	<!-- 16 character serial number -->\n" <<
		"	<serial>" << std::string(config.serial, 16) << "</serial>\n" <<
		"\n" <<
		"	<!-- Custom mode pages, base64 encoded, up to 1024 bytes.-->\n" <<
		"	<modePages>\n" <<
            (modePages.size() == 0 ? "" :
             [[[NSData dataWithBytes:&modePages[0] length:modePages.size()] base64String]
              cStringUsingEncoding:NSUTF8StringEncoding])
				<< "\n" <<
		"	</modePages>\n" <<
		"\n" <<
		"	<!-- Custom inquiry VPD pages, base64 encoded, up to 1024 bytes.-->\n" <<
		"	<vpd>\n" <<
			(vpd.size() == 0 ? "" :
            [[[NSData dataWithBytes:&vpd[0] length:vpd.size()] base64String]
             cStringUsingEncoding:NSUTF8StringEncoding])
				<< "\n" <<
		"	</vpd>\n" <<
		"</SCSITarget>\n";

	return s.str();
}

std::string
ConfigUtil::toXML(const BoardConfig& config)
{
	std::stringstream s;

	s << "<BoardConfig>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Enable the onboard active terminator (v5.1 or greater).\n"
		"	Both ends of the SCSI chain should be terminated. Disable\n" <<
		"	only if the SCSI2SD is in the middle of a chain with other\n" <<
		"	devices.\n" <<
		"	********************************************************* -->\n" <<
		"	<enableTerminator>" <<
			(config.flags6 & S2S_CFG_ENABLE_TERMINATOR ? "true" : "false") <<
			"</enableTerminator>\n" <<

		"	<unitAttention>" <<
			(config.flags & CONFIG_ENABLE_UNIT_ATTENTION ? "true" : "false") <<
			"</unitAttention>\n" <<

		"	<parity>" <<
			(config.flags & CONFIG_ENABLE_PARITY ? "true" : "false") <<
			"</parity>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Only set to true when using with a fast SCSI2 host\n " <<
		"	controller. This can cause problems with older/slower\n" <<
		"	hardware.\n" <<
		"	********************************************************* -->\n" <<
		"	<enableScsi2>" <<
			(config.flags & CONFIG_ENABLE_SCSI2 ? "true" : "false") <<
			"</enableScsi2>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Setting to 'true' will result in increased performance at the\n" <<
		"	cost of lower noise immunity.\n" <<
		"	Only set to true when using short cables with only 1 or two\n" <<
		"	devices. This should remain off when using external SCSI1 DB25\n" <<
		"	cables.\n" <<
		"	********************************************************* -->\n" <<
		"	<disableGlitchFilter>" <<
			(config.flags & CONFIG_DISABLE_GLITCH ? "true" : "false") <<
			"</disableGlitchFilter>\n" <<

		"	<enableCache>" <<
			(config.flags & CONFIG_ENABLE_CACHE ? "true" : "false") <<
			"</enableCache>\n" <<

		"	<enableDisconnect>" <<
			(config.flags & CONFIG_ENABLE_DISCONNECT ? "true" : "false") <<
			"</enableDisconnect>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Respond to very short duration selection attempts. This supports\n" <<
		"	non-standard hardware, but is generally safe to enable.\n" <<
		"	Required for Philips P2000C.\n" <<
		"	********************************************************* -->\n" <<
		"	<selLatch>" <<
			(config.flags & CONFIG_ENABLE_SEL_LATCH? "true" : "false") <<
			"</selLatch>\n" <<


		"	<!-- ********************************************************\n" <<
		"	Convert luns to IDs. The unit must already be configured to respond\n" <<
		"	on the ID. Allows dual drives to be accessed from a \n" <<
		"	XEBEC S1410 SASI bridge.\n" <<
		"	eg. Configured for dual drives as IDs 0 and 1, but the XEBEC will\n" <<
		"	access the second disk as ID0, lun 1.\n" <<
		"	See ttp://bitsavers.trailing-edge.com/pdf/xebec/104524C_S1410Man_Aug83.pdf\n" <<
		"	********************************************************* -->\n" <<
		"	<mapLunsToIds>" <<
			(config.flags & CONFIG_MAP_LUNS_TO_IDS ? "true" : "false") <<
			"</mapLunsToIds>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Delay (in milliseconds) before responding to a SCSI selection.\n" <<
		"	255 (auto) sets it to 0 for SCSI2 hosts and 1ms otherwise.\n" <<
		"	Some samplers need this set to 1 manually.\n" <<
		"	********************************************************* -->\n" <<
		"	<selectionDelay>" << static_cast<int>(config.selectionDelay) << "</selectionDelay>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Startup delay (in seconds) before responding to the SCSI bus \n" <<
		"	after power on. Default = 0.\n" <<
		"	********************************************************* -->\n" <<
		"	<startupDelay>" << static_cast<int>(config.startupDelay) << "</startupDelay>\n" <<

		"	<!-- ********************************************************\n" <<
		"	Speed limit the SCSI interface. This is the -max- speed the \n" <<
		"	device will run at. The actual spee depends on the capability\n" <<
		"	of the host controller.\n" <<
		"	0	No limit\n" <<
		"	1	Async 1.5MB/s\n" <<
		"	********************************************************* -->\n" <<
		"	<scsiSpeed>" << static_cast<int>(config.scsiSpeed) << "</scsiSpeed>\n" <<
		"</BoardConfig>\n";

	return s.str();
}


static uint64_t parseInt(NSXMLNode* node, uint64_t limit)
{
	std::string str([[node stringValue] cStringUsingEncoding:NSUTF8StringEncoding]);
	if (str.empty())
	{
        throw std::runtime_error("Empty XML node");
	}

	std::stringstream s;
	if (str.find("0x") == 0)
	{
		s << std::hex << str.substr(2);
	}
	else
	{
		s << str;
	}

	uint64_t result;
	s >> result;
	if (!s)
	{
		throw std::runtime_error("Invalid value for XML node");
	}

	if (result > limit)
	{
		std::stringstream msg;
		msg << "Invalid value";
		throw std::runtime_error(msg.str());
	}
	return result;
}

static TargetConfig
parseTarget(NSXMLElement* node)
{
	int i = 0;
    NSXMLNode *n = [node attributeForName: @"id"];
    i = [[n stringValue] intValue];
    TargetConfig result = ConfigUtil::Default(i & 0x7);

	NSArray *children = [node children];
    NSEnumerator *en = [children objectEnumerator];
    NSXMLNode *child = [en nextObject];
	while (child)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
		if ([[child name] isEqualToString: @"enabled"])
		{
			NSString *s = [child stringValue];
			if ([s isEqualToString: @"true"])
			{
				result.scsiId |= CONFIG_TARGET_ENABLED;
			}
			else
			{
				result.scsiId = result.scsiId & ~CONFIG_TARGET_ENABLED;
			}
		}
		else if ([[child name] isEqualToString: @"quirks"])
		{
            std::stringstream s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			std::string quirk;
			while (s >> quirk)
			{
				if (quirk == "apple")
				{
					result.quirks |= CONFIG_QUIRKS_APPLE;
				}
				else if (quirk == "omti")
				{
					result.quirks |= CONFIG_QUIRKS_OMTI;
				}
				else if (quirk == "xebec")
				{
					result.quirks |= CONFIG_QUIRKS_XEBEC;
				}
			}
		}
		else if ([[child name] isEqualToString: @"deviceType"])
		{
			result.deviceType = parseInt(child, 0xFF);
		}
		else if ([[child name] isEqualToString: @"deviceTypeModifier"])
		{
			result.deviceTypeModifier = parseInt(child, 0xFF);
		}
		else if ([[child name] isEqualToString: @"sdSectorStart"])
		{
			result.sdSectorStart = parseInt(child, 0xFFFFFFFF);
		}
		else if ([[child name] isEqualToString: @"scsiSectors"])
		{
			result.scsiSectors = parseInt(child, 0xFFFFFFFF);
		}
		else if ([[child name] isEqualToString: @"bytesPerSector"])
		{
			result.bytesPerSector = parseInt(child, 8192);
		}
		else if ([[child name] isEqualToString: @"sectorsPerTrack"])
		{
			result.sectorsPerTrack = parseInt(child, 255);
		}
		else if ([[child name] isEqualToString: @"headsPerCylinder"])
		{
			result.headsPerCylinder = parseInt(child, 255);
		}
		else if ([[child name] isEqualToString: @"vendor"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			s = s.substr(0, sizeof(result.vendor));
			memset(result.vendor, ' ', sizeof(result.vendor));
			memcpy(result.vendor, s.c_str(), s.size());
		}
		else if ([[child name] isEqualToString: @"prodId"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			s = s.substr(0, sizeof(result.prodId));
			memset(result.prodId, ' ', sizeof(result.prodId));
			memcpy(result.prodId, s.c_str(), s.size());
		}
		else if ([[child name] isEqualToString: @"revision"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			s = s.substr(0, sizeof(result.revision));
			memset(result.revision, ' ', sizeof(result.revision));
			memcpy(result.revision, s.c_str(), s.size());
		}
		else if ([[child name] isEqualToString: @"serial"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			s = s.substr(0, sizeof(result.serial));
			memset(result.serial, ' ', sizeof(result.serial));
			memcpy(result.serial, s.c_str(), s.size());
		}
		else if ([[child name] isEqualToString: @"modePages"])
		{
            NSString *content = [child stringValue];
            NSData *buf = [[NSData alloc] initWithBase64Encoding:content];
			size_t len = std::min([buf length], sizeof(result.modePages));
			memcpy(result.modePages, [buf bytes], len);
		}
		else if ([[child name] isEqualToString: @"vpd"])
		{
            NSString *content = [child stringValue];
            NSData *buf = [[NSData alloc] initWithBase64Encoding:content];
            size_t len = std::min([buf length], sizeof(result.vpd));
            memcpy(result.vpd, [buf bytes], len);
		}
        
        child = [en nextObject];
#pragma GCC diagnostic pop
	}
	return result;
}

static BoardConfig
parseBoardConfig(NSXMLElement* node)
{
	BoardConfig result = ConfigUtil::DefaultBoardConfig();

	NSArray *children = [node children];
    NSEnumerator *en = [children objectEnumerator];
    NSXMLNode *child = [en nextObject];
	while (child)
	{
		if ([[child name] isEqualToString: @"selectionDelay"])
		{
			result.selectionDelay = parseInt(child, 255);
		}
		else if ([[child name] isEqualToString: @"startupDelay"])
		{
			result.startupDelay = parseInt(child, 255);
		}
		else if ([[child name] isEqualToString: @"unitAttention"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_ENABLE_UNIT_ATTENTION;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_ENABLE_UNIT_ATTENTION;
			}
		}
		else if ([[child name] isEqualToString: @"parity"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_ENABLE_PARITY;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_ENABLE_PARITY;
			}
		}
		else if ([[child name] isEqualToString: @"enableScsi2"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_ENABLE_SCSI2;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_ENABLE_SCSI2;
			}
		}
		else if ([[child name] isEqualToString: @"disableGlitchFilter"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_DISABLE_GLITCH;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_DISABLE_GLITCH;
			}
		}
		else if ([[child name] isEqualToString: @"enableCache"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_ENABLE_CACHE;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_ENABLE_CACHE;
			}
		}
		else if ([[child name] isEqualToString: @"enableDisconnect"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_ENABLE_DISCONNECT;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_ENABLE_DISCONNECT;
			}
		}
		else if ([[child name] isEqualToString: @"enableTerminator"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags6 |= S2S_CFG_ENABLE_TERMINATOR;
			}
			else
			{
				result.flags6 = result.flags & ~S2S_CFG_ENABLE_TERMINATOR;
			}
		}
		else if ([[child name] isEqualToString: @"selLatch"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_ENABLE_SEL_LATCH;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_ENABLE_SEL_LATCH;
			}
		}
		else if ([[child name] isEqualToString: @"mapLunsToIds"])
		{
			std::string s((std::string([[child stringValue] cStringUsingEncoding:NSUTF8StringEncoding])));
			if (s == "true")
			{
				result.flags |= CONFIG_MAP_LUNS_TO_IDS;
			}
			else
			{
				result.flags = result.flags & ~CONFIG_MAP_LUNS_TO_IDS;
			}
		}
		else if ([[child name] isEqualToString: @"scsiSpeed"])
		{
			result.scsiSpeed = parseInt(child, CONFIG_SPEED_ASYNC_15);
		}
        child = [en nextObject];
	}
	return result;
}


std::pair<BoardConfig, std::vector<TargetConfig>>
ConfigUtil::fromXML(const std::string& filename)
{
    NSData *data = [NSData dataWithContentsOfFile: [NSString stringWithUTF8String:filename.c_str()]];
	NSXMLDocument *doc = [[NSXMLDocument alloc] initWithData: data
                                                     options: NSXMLNodeOptionsNone
                                                       error: NULL];
	if (doc == nil)
	{
		throw std::runtime_error("Could not load XML file");
	}

	// start processing the XML file
    if ([[[doc rootElement] name] isEqualToString: @"SCSI2SD"] == NO)
	{
		throw std::runtime_error("Invalid root node, expected <SCSI2SD>");
	}

	BoardConfig boardConfig = DefaultBoardConfig();
	int boardConfigFound = 0;

	std::vector<TargetConfig> targets;
    NSArray *children = [[doc rootElement] children]; // doc.GetRoot()->GetChildren();
    NSEnumerator *en = [children objectEnumerator];
    NSXMLElement *child = [en nextObject];
	while (child)
	{
		if ([[child name] isEqualToString: @"SCSITarget"])
		{
			targets.push_back(parseTarget(child));
		}
		else if ([[child name] isEqualToString: @"BoardConfig"])
		{
			boardConfig = parseBoardConfig(child);
			boardConfigFound = 1;
		}
        child = [en nextObject];
	}

	if (!boardConfigFound && targets.size() > 0)
	{
		boardConfig.flags = targets[0].flagsDEPRECATED;
	}
	return make_pair(boardConfig, targets);
}

