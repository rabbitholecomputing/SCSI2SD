//
//  AppDelegate.h
//  scsi2sd
//
//  Created by Gregory Casamento on 7/23/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "SCSI2SD_Bootloader.hh"
#include "DataBootloader.h"
#include "SCSI2SD_HID.hh"
#include "Firmware.hh"
#include "scsi2sd.h"
#include "Functions.hh"

@interface AppDelegate : NSObject <NSApplicationDelegate, NSComboBoxDataSource>
{
    std::shared_ptr<SCSI2SD::HID> myHID;
    std::shared_ptr<DataBootloader> myBootloader;
    
    bool myInitialConfig;
    //std::vector<TargetConfig *> myTargets;
    
    uint8_t myTickCounter;
    time_t myLastPollTime;
    
    NSTimer *pollDeviceTimer;
    
    BOOL shouldLogScsiData;
    BOOL doScsiSelfTest;
}

@end

