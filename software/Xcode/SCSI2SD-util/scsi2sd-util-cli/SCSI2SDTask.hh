//
//  SCSI2SDTask.h
//  scsi2sd-util-cli
//
//  Created by Gregory Casamento on 1/10/20.
//  Copyright © 2020 Rabbit Hole Computing, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

#include "SCSI2SD_Bootloader.hh"
#include "SCSI2SD_HID.hh"
#include "Firmware.hh"
#include "scsi2sd.h"
#include "Functions.hh"
#include "ConfigUtil.hh"

NS_ASSUME_NONNULL_BEGIN

@interface SCSI2SDTask : NSObject
{
    std::shared_ptr<SCSI2SD::HID> myHID;
    std::shared_ptr<SCSI2SD::Bootloader> myBootloader;
}

@property (nonatomic, assign) BOOL repeatMode;

+ (instancetype) task;
- (BOOL) getHid;
- (void) waitForHidConnection;
- (void) saveFromDeviceFromFilename: (NSString *)filename;
- (void) saveToDeviceFromFilename: (NSString *)filename;
- (void) upgradeFirmwareDeviceFromFilename: (NSString *)filename;

@end

NS_ASSUME_NONNULL_END
