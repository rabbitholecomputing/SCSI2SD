#!/bin/sh

echo "Resetting..."

FILE=./software/scsi2sd-util/hidapi-mac/hid.c
ORIG=${FILE}.orig
WINFILE=./windows/hid.c

echo "Copying Windows file..."
cp $WINFILE $FILE

echo "Done..."
