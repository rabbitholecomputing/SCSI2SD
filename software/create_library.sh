#!/bin/sh

echo "Building libraries for Xcode project..."
SCSI2SD_FILES=`ls -C1 build/mac/*.o | grep -v scsi2sd-util.o | grep -v BoardPanel.o | grep -v ConfigUtil.o | grep -v TargetPanel.o | grep -v scsi2sd-bulk.o | grep -v scsi2sd-monitor.o`

mkdir libs
mkdir libs/include
mkdir libs/lib

echo "Making libraries using libtool..."
libtool -static -o libs/lib/libscsi2sd-util.a ${SCSI2SD_FILES}
libtool -static -o libs/lib/libz.a build/mac/zlib/*.o
libtool -static -o libs/lib/libzipper.a build/mac/libzipper/*.o

echo "Copying headers..."
HFILES=`find . | grep \\.h | grep -v build | grep -v wxWidgets | grep -v Panel | grep -v SCSI2SD | grep -v testgui | grep -v \\.sh`
for i in ${HFILES}
do
	file=`echo ${i} | sed 's/\.\/scsi2sd-util\///'`
	echo "copying ${i} -> libs/include/${file}"
	tar cf - ${i} | (cd ./libs/include; tar xf -)
	# echo $file
done

echo "Done."
exit 0
