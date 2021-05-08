//
//  DeviceController.h
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
// #import "TargetConfig.h"
#include "scsi2sd.h"

NS_ASSUME_NONNULL_BEGIN

@interface DeviceController : NSObject <NSTextFieldDelegate>
{
}

@property (readonly) IBOutlet NSButton *enableSCSITarget;
@property (readonly) IBOutlet NSComboBox *SCSIID;
@property (readonly) IBOutlet NSPopUpButton *deviceType;
@property (readonly) IBOutlet NSTextField *sdCardStartSector;
@property (readonly) IBOutlet NSTextField *sectorSize;
@property (readonly) IBOutlet NSTextField *sectorCount;
@property (readonly) IBOutlet NSTextField *deviceSize;
@property (readonly) IBOutlet NSPopUpButton *deviceUnit;
@property (readonly) IBOutlet NSTextField *vendor;
@property (readonly) IBOutlet NSTextField *productId;
@property (readonly) IBOutlet NSTextField *revsion;
@property (readonly) IBOutlet NSTextField *serialNumber;
@property (readonly) IBOutlet NSButton *autoStartSector;
@property (readonly) IBOutlet NSTextField *sectorsPerTrack;
@property (readonly) IBOutlet NSTextField *headsPerCylinder;

@property (readonly) IBOutlet NSTextField *autoErrorText;
@property (readonly) IBOutlet NSTextField *scsiIdErrorText;

@property BOOL duplicateId;
@property BOOL sectorOverlap;

- (void) setTargetConfigData: (NSData *)data;
- (void) setTargetConfig: (TargetConfig)config;
- (TargetConfig) getTargetConfig;
- (NSString *) toXml;
- (BOOL) evaluate;
- (void) evaluateSize;
- (NSUInteger) convertUnitsToSectors;

// Getters and setters for properties of the GUI...
- (BOOL) isEnabled;
- (NSUInteger) getSCSIId;
- (void) setDuplicateID: (BOOL)flag;
- (void) setSDSectorOverlap: (BOOL)flag;
- (NSRange) getSDSectorRange;
- (void) setAutoStartSectorValue: (NSUInteger)sector;

@end

NS_ASSUME_NONNULL_END
