#!/bin/sh

echo "Configuring..."

FILE=./software/scsi2sd-util/hidapi-mac/hid.c
LINUXFILE=./linux/hid.c

echo "Copying Linux file..."
cp $LINUXFILE $FILE

echo "Done..."
