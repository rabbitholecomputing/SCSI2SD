#!/bin/sh

CURRENTDIR=`pwd`

case `uname -s` in
Linux)
	# Builds all of the utilities (not firmware) under Linux.
	# Requires mingw installed to cross-compile Windows targets.
	(cd scsi2sd-util && ./build.sh)
	if [ $? -eq 0 ]; then
		mkdir -p build/linux
		mkdir -p build/windows/64bit
		mkdir -p build/windows/32bit
		cp scsi2sd-util/build/linux/scsi2sd-util build/linux
		cp scsi2sd-util/build/windows/32bit/scsi2sd-util.exe build/windows/32bit
		cp scsi2sd-util/build/windows/64bit/scsi2sd-util.exe build/windows/64bit
	fi
;;

Darwin)
	make -C scsi2sd-util
	if [ $? -eq 0 ]; then
		mkdir -p build/mac
		cp scsi2sd-util/build/mac/scsi2sd-util build/mac
	fi
	cd ${CURRENTDIR}
	./create-wrapper.sh
	rm -rf ./build/mac/SCSI2SD
	mv ./build/mac/dmg ./build/mac/SCSI2SD
	cp -r *.app ./build/mac/SCSI2SD
	rm -rf *.dmg
	echo "Making DMG"
	hdiutil create -srcfolder ./build/mac/SCSI2SD ./scsi2sd-util.dmg
	rm -rf Products
	mkdir -p Products/WXMacApp
	mv *.app *.dmg Products/WXMacApp
	rm -rf *.app *.dmg
	./create_library.sh
	# build xcode proj based on library...
	./build_xcode.sh
;;

MINGW32_NT-6.2)
	make -C scsi2sd-util
	if [ $? -eq 0 ]; then
		mkdir -p build/win
		cp scsi2sd-util/build/win/scsi2sd-util build/win
	fi
;;
esac
