#!/bin/sh

echo "Copying libraries"
cp -r ./libs/lib/*.a Xcode/SCSI2SD-util/libs/lib

echo "Building xcode project"
cd Xcode/SCSI2SD-util
pod install
xcodebuild -workspace SCSI2SD-util.xcworkspace -scheme SCSI2SD-util
