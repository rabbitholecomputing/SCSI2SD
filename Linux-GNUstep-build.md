# Linux GNUstep build instructions

Building the application
--
* cd into the project directory SCSI2SD-util
* cd software/Xcode/SCSI2SD-util
* run buildtool
	* buildtool should comple both targets in the xcodeproj file

Buildtool can be found here
--
You must build and install both of these to build the application...

* [buildtool](https://github.com/gnustep/tools-buildtool) the buildtool front end..  this doesn't change much.  You will also need
* [libs-xcode](https://github.com/gnustep/libs-xcode) the library which loads and interprets the xcodeproj

The app and these tools requires you to install [GNUstep](http://www.gnustep.org) from [github](https://github.com/gnustep).

Setup USB permissions
--
* go to /etc/udev/rules.d
* create a file called 99-hidraw-permissions.rules (the name is not important)
* add the line: KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev" to the file
* go to /dev
* sudo chmod 664 /dev/hidraw*
* sudo chgrp plugdev /dev/hidraw*

