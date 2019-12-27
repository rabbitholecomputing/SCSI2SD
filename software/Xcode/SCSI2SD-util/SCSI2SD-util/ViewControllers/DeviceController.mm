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

@property BOOL duplicateId;
@property BOOL sectorOverlap;

@end

@implementation DeviceController

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

/*
- (void) fromXml: (NSString *)file
{
    std::string filename = std::string([file cStringUsingEncoding:NSUTF8StringEncoding]);
    TargetConfig config = SCSI2SD::ConfigUtil::fromXML(filename);
    NSLog(@"fromXml");
    [self setTargetConfig: config];
}
*/

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
}
- (void) setSDSectorOverlap: (BOOL)flag
{
    self.sectorOverlap = flag;
}

- (NSRange) getSDSectorRange
{
    return NSMakeRange(0,0);
}

- (void) setAutoStartSectorValue: (NSUInteger)sector
{
    
}
@end
