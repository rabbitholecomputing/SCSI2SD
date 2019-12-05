//
//  AppDelegate.m
//  scsi2sd
//
//  Created by Gregory Casamento on 7/23/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "AppDelegate.h"
#include "SCSI2SD_Bootloader.hh"
#include "SCSI2SD_HID.hh"
#include "Firmware.hh"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
