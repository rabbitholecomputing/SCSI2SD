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

@property (nonatomic) IBOutlet NSButton *enableSCSITarget;
@property (nonatomic) IBOutlet NSComboBox *SCSIID;
@property (nonatomic) IBOutlet NSPopUpButton *deviceType;
@property (nonatomic) IBOutlet NSTextField *sdCardStartSector;
@property (nonatomic) IBOutlet NSTextField *sectorSize;
@property (nonatomic) IBOutlet NSTextField *sectorCount;
@property (nonatomic) IBOutlet NSTextField *deviceSize;
@property (nonatomic) IBOutlet NSPopUpButton *deviceUnit;
@property (nonatomic) IBOutlet NSTextField *vendor;
@property (nonatomic) IBOutlet NSTextField *productId;
@property (nonatomic) IBOutlet NSTextField *revsion;
@property (nonatomic) IBOutlet NSTextField *serialNumber;
@property (nonatomic) IBOutlet NSButton *autoStartSector;
@property (nonatomic) IBOutlet NSTextField *sectorsPerTrack;
@property (nonatomic) IBOutlet NSTextField *headsPerCylinder;

@property (nonatomic) IBOutlet NSTextField *autoErrorText;
@property (nonatomic) IBOutlet NSTextField *scsiIdErrorText;

@property BOOL duplicateId;
@property BOOL sectorOverlap;

- (void) setTargetConfigData: (NSData *)data;
- (void) setTargetConfig: (TargetConfig)config;
- (TargetConfig) getTargetConfig;
- (NSString *) toXml;
- (BOOL) evaluate;
- (void) evaluateSize;

// Getters and setters for properties of the GUI...
- (BOOL) isEnabled;
- (NSUInteger) getSCSIId;
- (void) setDuplicateID: (BOOL)flag;
- (void) setSDSectorOverlap: (BOOL)flag;
- (NSRange) getSDSectorRange;
- (void) setAutoStartSectorValue: (NSUInteger)sector;

@end

NS_ASSUME_NONNULL_END
