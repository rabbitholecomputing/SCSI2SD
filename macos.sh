#!/bin/sh

echo "Resetting..."

FILE=./software/scsi2sd-util/hidapi-mac/hid.c
ORIG=${FILE}.orig
MACFILE=./macos/hid.c

echo "Copying Mac file..."
cp $MACFILE $FILE

echo "Done..."
