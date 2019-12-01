#!/bin/sh

echo "******** Creating application wrapper...."

rm -rf SCSI2SD.app
mkdir -p SCSI2SD.app/Contents/MacOS
mkdir -p SCSI2SD.app/Contents/Resources
cd SCSI2SD.app/Contents
cp ../../build/mac/scsi2sd-util ./MacOS
cp ../../images/*.icns ./Resources

cat << !EOF! > Info.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDevelopmentRegion</key>
        <string>en</string>
        <key>CFBundleExecutable</key>
        <string>scsi2sd-util</string>
        <key>CFBundleGetInfoString</key>
        <string>©2019 Rabbithole Computing</string>
        <key>CFBundleIconFile</key>
        <string>SCSI2SD.icns</string>
        <key>CFBundleIdentifier</key>
        <string>com.rabbitholecomputing.SCSI2SD</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundleName</key>
        <string>SCSI2SD</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>1.4</string>
        <key>CFBundleSignature</key>
        <string>RHC</string>
        <key>CFBundleVersion</key>
        <string>1.0</string>
        <key>LSApplicationCategoryType</key>
        <string>public.app-category.utilities</string>
        <key>LSMinimumSystemVersion</key>
        <string>10.7</string>
        <key>NSHighResolutionCapable</key>
        <true/>
        <key>NSRequiresAquaSystemAppearance</key>
        <false/>
        <key>CFBundleSupportedPlatforms</key>
        <array>
        <string>MacOSX</string>
        </array>
</dict>
</plist>
!EOF!

echo "******** Wrapper creation complete...."