//
//  DeviceController.m
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "DeviceController.hh"
#import "NSString+Extensions.h"

#include "ConfigUtil.hh"
@interface DeviceController ()

@property (weak, nonatomic) IBOutlet NSButton *enableSCSITarget;
@property (weak, nonatomic) IBOutlet NSComboBox *SCSIID;
@property (weak, nonatomic) IBOutlet NSPopUpButton *deviceType;
@property (weak, nonatomic) IBOutlet NSTextField *sdCardStartSector;
@property (weak, nonatomic) IBOutlet NSTextField *sectorSize;
@property (weak, nonatomic) IBOutlet NSTextField *sectorCount;
@property (weak, nonatomic) IBOutlet NSTextField *deviceSize;
@property (weak, nonatomic) IBOutlet NSPopUpButton *deviceUnit;
@property (weak, nonatomic) IBOutlet NSTextField *vendor;
@property (weak, nonatomic) IBOutlet NSTextField *productId;
@property (weak, nonatomic) IBOutlet NSTextField *revsion;
@property (weak, nonatomic) IBOutlet NSTextField *serialNumber;
@property (weak, nonatomic) IBOutlet NSButton *autoStartSector;
@property (weak, nonatomic) IBOutlet NSTextField *sectorsPerTrack;
@property (weak, nonatomic) IBOutlet NSTextField *headsPerCylinder;

@property (weak, nonatomic) IBOutlet NSTextField *autoErrorText;
@property (weak, nonatomic) IBOutlet NSTextField *scsiIdErrorText;

@property BOOL duplicateId;
@property BOOL sectorOverlap;

@end

@implementation DeviceController

- (void) awakeFromNib
{
    self.enableSCSITarget.toolTip = @"Enable this device";
    self.SCSIID.toolTip = @"Unique SCSI ID for target device";
    self.deviceType.toolTip = @"Dervice type: HD, Removable, etc";
    self.sdCardStartSector.toolTip = @"Sector to start SCSI emulation from";
    self.sectorSize.toolTip = @"Size of each sector, default is 512";
    self.sectorCount.toolTip = @"Number of sectors";
    self.deviceSize.toolTip = @"Size in units specified";
    self.deviceUnit.toolTip = @"Units for device: GB, MB, etc";
    self.vendor.toolTip = @"Vendor identification number.";
    self.productId.toolTip = @"Unique product identifier";
    self.revsion.toolTip = @"Revision";
    self.serialNumber.toolTip = @"Serial number for device";
    self.autoStartSector.toolTip = @"Auto start sector based on other targets";
    self.sectorsPerTrack.toolTip = @"Number of sectors in each track";
    self.headsPerCylinder.toolTip = @"Number of heads in cylinder";
    
    // Initial values
    self.autoErrorText.stringValue = @"";
    self.scsiIdErrorText.stringValue = @"";
}

- (void) setTargetConfig: (TargetConfig)config
{
    // NSLog(@"setTargetConfig");
    self.enableSCSITarget.state = (config.scsiId & 0x80) ? NSOnState : NSOffState;
    [self.SCSIID setStringValue:
     [NSString stringWithFormat: @"%d", (config.scsiId & 0x80) ?
      (config.scsiId - 0x80) : config.scsiId]];
    [self.deviceType selectItemAtIndex: config.deviceType];
    [self.sdCardStartSector setStringValue:[NSString stringWithFormat:@"%d", config.sdSectorStart]];
    [self.sectorSize setStringValue: [NSString stringWithFormat: @"%d", config.bytesPerSector]];
    [self.sectorCount setStringValue: [NSString stringWithFormat: @"%d", config.scsiSectors]];
    [self.deviceSize setStringValue: [NSString stringWithFormat: @"%d", (((config.scsiSectors * config.bytesPerSector) / (1024 * 1024)) + 1) / 1024]];
    // Heads per cylinder is missing... should add it here.
    // Sectors per track...
    [self.vendor setStringValue: [NSString stringWithCString:config.vendor length:8]];
    [self.productId setStringValue: [NSString stringWithCString:config.prodId length:16]];
    [self.revsion setStringValue: [NSString stringWithCString:config.revision length:4]];
    [self.serialNumber setStringValue: [NSString stringWithCString:config.serial length:16]];
    [self.sectorsPerTrack setStringValue: [NSString stringWithFormat: @"%d", config.sectorsPerTrack]];
    [self.headsPerCylinder setStringValue: [NSString stringWithFormat: @"%d", config.headsPerCylinder]];
    // [self.autoStartSector setState:]
}

- (void) setTargetConfigData: (NSData *)data
{
    TargetConfig config;
    const void *bytes;
    bytes = [data bytes];
    memcpy(&config, bytes, sizeof(TargetConfig));
    [self setTargetConfig: config];
}

- (TargetConfig) getTargetConfig
{
    TargetConfig targetConfig;
    
    NSLog(@"getTargetConfig");
    targetConfig.scsiId = self.SCSIID.intValue + self.enableSCSITarget.state == NSOnState ? 0x80 : 0x0;
    targetConfig.deviceType = self.deviceType.indexOfSelectedItem;
    targetConfig.sdSectorStart = self.sdCardStartSector.intValue;
    targetConfig.bytesPerSector = self.sectorSize.intValue;
    targetConfig.scsiSectors = self.sectorCount.intValue;
    strncpy(targetConfig.vendor, [self.vendor.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 8);
    strncpy(targetConfig.prodId, [self.productId.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 16);
    strncpy(targetConfig.revision, [self.revsion.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 4);
    strncpy(targetConfig.serial, [self.serialNumber.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 16);

    return targetConfig;
}

- (NSString *) toXml
{
    TargetConfig config = [self getTargetConfig];
    std::string str = SCSI2SD::ConfigUtil::toXML(config);
    NSString *result = [NSString stringWithCString:str.c_str() encoding:NSUTF8StringEncoding];
    return result;
}

- (BOOL) evaluate
{
    // NSLog(@"fromXml");
    return YES;
}

- (BOOL) isEnabled
{
    return self.enableSCSITarget.state == NSOnState;
}

- (NSUInteger) getSCSIId
{
    return (NSUInteger)self.SCSIID.integerValue;
}

- (void) setDuplicateID: (BOOL)flag
{
    self.duplicateId = flag;
    if(flag)
        self.scsiIdErrorText.stringValue = @"Duplicate IDs.";
    else
        self.scsiIdErrorText.stringValue = @"";
}
- (void) setSDSectorOverlap: (BOOL)flag
{
    self.sectorOverlap = flag;
    if(flag)
        self.autoErrorText.stringValue = @"Sectors overlap.";
    else
        self.autoErrorText.stringValue = @"";
}

- (NSRange) getSDSectorRange
{
    return NSMakeRange(self.sdCardStartSector.integerValue,
                       self.sectorCount.integerValue);
}

- (void) setAutoStartSectorValue: (NSUInteger)sector
{
    self.sdCardStartSector.integerValue = (NSInteger)sector;
}
@end
