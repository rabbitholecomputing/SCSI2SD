#include "SCSI2SD_Bootloader.hh"
#include "SCSI2SD_HID.hh"
#include "Firmware.hh"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <set>
#include <sstream>

#if __cplusplus >= 201103L
#include <cstdint>
#include <memory>
using std::shared_ptr;
#else
#include <stdint.h>
#include <tr1/memory>
using std::tr1::shared_ptr;
#endif

#include <unistd.h>


#define MIN_FIRMWARE_VERSION 0x0400

using namespace SCSI2SD;

extern "C"
void ProgressUpdate(unsigned char arrayId, unsigned short rowNum)
{
	std::cout << "." << std::flush;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " {firmware.cyacd}" << std::endl;
		exit(1);
	}
	std::string filename(argv[1]);

	std::cout << "Searching for SCSI2SD device ..." << std::endl;

	while (true)
	{
		shared_ptr<Bootloader> bootloader;
		try
		{
			shared_ptr<HID> hid(HID::Open());
			if (hid)
			{
				std::cout << "Resetting SCSI2SD into bootloader" << std::endl;
				usleep(100000);
				hid->enterBootloader();
				usleep(100000);
			}

			bootloader.reset(Bootloader::Open());
		} catch (...) {}

		if (bootloader)
		{
			std::cout << "Upgrading firmware from file: " << filename << std::endl;

			try
			{
				bootloader->load(filename, &ProgressUpdate);
				usleep(2000000);
				std::cerr << "\nFirmware update successful" << std::endl;


				// Wait for disconnect
				shared_ptr<HID> hid(HID::Open());

				while (bootloader || hid)
				{
					usleep(100000);
					try {
						hid.reset(HID::Open());
						usleep(100000);
						bootloader.reset(Bootloader::Open());
						usleep(100000);
					} catch (...) {}
				}
				std::cout << "Searching for SCSI2SD device ..." << std::endl;
			}
			catch (std::exception& e)
			{
				std::cerr << "Firmware update failed: " << e.what() << std::endl;
				abort();
			}

		}
		else
		{
			usleep(100000);
		}
	}
}
