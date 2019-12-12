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

@interface DeviceController : NSObject

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

- (void) setTargetConfig: (TargetConfig)config;
- (TargetConfig) getTargetConfig;

- (NSString *) toXml;
- (void) fromXml: (NSXMLElement *)node;

- (BOOL) evaluate;
@end

NS_ASSUME_NONNULL_END
