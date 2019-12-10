//
//  AppDelegate.m
//  scsi2sd
//
//  Created by Gregory Casamento on 7/23/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "AppDelegate.hh"
#import "DeviceController.h"
#import "SettingsController.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property (weak, nonatomic) IBOutlet NSWindow *mainWindow;
@property (weak, nonatomic) IBOutlet NSTextField *infoLabel;
@property (weak, nonatomic) IBOutlet NSPanel *logPanel;
@property (weak, nonatomic) IBOutlet NSTextView *logTextView;

@property (weak, nonatomic) IBOutlet DeviceController *device1;
@property (weak, nonatomic) IBOutlet DeviceController *device2;
@property (weak, nonatomic) IBOutlet DeviceController *device3;
@property (weak, nonatomic) IBOutlet DeviceController *device4;
@property (weak, nonatomic) IBOutlet DeviceController *device5;
@property (weak, nonatomic) IBOutlet DeviceController *device6;
@property (weak, nonatomic) IBOutlet DeviceController *device7;

@property (weak, nonatomic) IBOutlet SettingsController *settings;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    myHID = SCSI2SD::HID::Open();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
