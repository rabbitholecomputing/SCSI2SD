//
//  SettingsController.m
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "SettingsController.hh"

@interface SettingsController ()

@property (nonatomic) IBOutlet NSButton *enableSCSITerminator;
@property (nonatomic) IBOutlet NSPopUpButton *speedLimit;
@property (nonatomic) IBOutlet NSTextField *startupDelay;
@property (nonatomic) IBOutlet NSTextField *startupSelectionDelay;
@property (nonatomic) IBOutlet NSButton *enableParity;
@property (nonatomic) IBOutlet NSButton *enableUnitAttention;
@property (nonatomic) IBOutlet NSButton *enableSCSI2Mode;
@property (nonatomic) IBOutlet NSButton *respondToShortSCSISelection;
@property (nonatomic) IBOutlet NSButton *mapLUNStoSCSIIDs;
@property (nonatomic) IBOutlet NSButton *enableGlitch;
@property (nonatomic) IBOutlet NSButton *enableCache;
@property (nonatomic) IBOutlet NSButton *enableDisconnect;

@end

@implementation SettingsController

- (NSString *) toXml
{
    NSLog(@"toXML");
    return nil;
}

/*
- (void) fromXml: (NSXMLElement *)node
{
    NSLog(@"fromXml");
}
*/

- (void) setConfig: (BoardConfig)config
{
    NSLog(@"setConfig");
    
}

- (BoardConfig) getConfig
{
    BoardConfig config;
    NSLog(@"getConfig");
    config.flags |= self.enableSCSITerminator.intValue;

    config.flags =
        (self.enableParity.state == NSOnState ? CONFIG_ENABLE_PARITY : 0) |
        (self.enableUnitAttention.state == NSOnState ? CONFIG_ENABLE_UNIT_ATTENTION : 0) |
        (self.enableSCSI2Mode.state == NSOnState ? CONFIG_ENABLE_SCSI2 : 0) |
        (self.enableGlitch.state == NSOnState ? CONFIG_DISABLE_GLITCH : 0) |
        (self.enableCache.state == NSOnState ? CONFIG_ENABLE_CACHE: 0) |
        (self.enableDisconnect.state == NSOnState ? CONFIG_ENABLE_DISCONNECT: 0) |
        (self.respondToShortSCSISelection.state == NSOnState ? CONFIG_ENABLE_SEL_LATCH : 0) |
        (self.mapLUNStoSCSIIDs.state == NSOnState ? CONFIG_MAP_LUNS_TO_IDS : 0);
    config.startupDelay = self.startupDelay.intValue;
    config.scsiSpeed = self.speedLimit.indexOfSelectedItem;
    
    // return NULL;
    return config;
}

@end
