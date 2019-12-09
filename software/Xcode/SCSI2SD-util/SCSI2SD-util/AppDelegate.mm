//
//  AppDelegate.m
//  scsi2sd
//
//  Created by Gregory Casamento on 7/23/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "AppDelegate.hh"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property (weak, nonatomic) IBOutlet NSWindow *mainWindow;
@property (weak, nonatomic) IBOutlet NSTextField *infoLabel;
@property (weak, nonatomic) IBOutlet NSPanel *logPanel;
@property (weak, nonatomic) IBOutlet NSTextView *logTextView;

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
