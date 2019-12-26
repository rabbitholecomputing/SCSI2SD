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

- (void) setTargetConfig: (TargetConfig)config;
- (TargetConfig) getTargetConfig;
- (NSString *) toXml;
- (BOOL) evaluate;

// Getters and setters for properties of the GUI...
- (BOOL) isEnabled;
- (NSUInteger) getSCSIId;
- (void) setDuplicateID: (BOOL)flag;
- (void) setSDSectorOverlap: (BOOL)flag;
- (NSRange) getSDSectorRange;
- (void) setAutoStartSectorValue: (NSUInteger)sector;

@end

NS_ASSUME_NONNULL_END
