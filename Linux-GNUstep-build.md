# Linux GNUstep build instructions
--

Building the application
--
* cd into the project directory SCSI2SD-util
* cd software/Xcode/SCSI2SD-util
* run buildtool
	* buildtool should comple both targets in the xcodeproj file

Setup USB permissions
--
* go to /etc/udev/rules.d
* create a file called 99-hidraw-permissions.rules (the name is not important)
* add the line: KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev" to the file
* go to /dev
* sudo chmod 664 /dev/hidraw*
* sudo chgrp plugdev /dev/hidraw*

