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
    BoardConfig config = [self getConfig];
    std::string s = SCSI2SD::ConfigUtil::toXML(config);
    NSString *string = [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding];
    return string;
}

/*
- (void) fromXml: (NSXMLElement *)node
{
    NSLog(@"fromXml");
}
*/
- (void) awakeFromNib
{
    self.enableParity.toolTip = @"Enable to require valid SCSI parity bits when receiving data. Some hosts don't provide parity. SCSI2SD always outputs valid parity bits.";
    self.enableUnitAttention.toolTip = @"Enable this to inform the host of changes after hot-swapping SD cards. Causes problems with Mac Plus.";
    self.enableSCSI2Mode.toolTip = @"Enable high-performance mode. May cause problems with SASI/SCSI1 hosts.";
    self.enableSCSITerminator.toolTip = @"Enable active terminator. Both ends of the SCSI chain must be terminated.";
    self.enableGlitch.toolTip = @"Improve performance at the cost of noise immunity. Only use with short cables";
    self.enableCache.toolTip = @"SD IO commands aren't completed when SCSI commands complete";
    self.enableDisconnect.toolTip = @"Release the SCSI bus while waiting for SD card writes to complete. Must also be enabled in host OS";
    self.respondToShortSCSISelection.toolTip = @"Respond to very short duration selection attempts. This supports non-standard hardware, but is generally safe to enable.  Required for Philips P2000C.";
    self.mapLUNStoSCSIIDs.toolTip = @"create LUNS as IDs instead. Supports multiple drives on XEBEC S1410 SASI Bridge";
    self.startupDelay.toolTip = @"Extra delay on power on, normally set to 0";
    self.speedLimit.toolTip = @"Limit SCSI interface speed";
    self.startupSelectionDelay.toolTip = @"Delay before responding to SCSI selection. SCSI1 hosts usually require 1ms delay, however some require no delay.";
}

- (void) setConfig: (BoardConfig)config
{
    // NSLog(@"setConfig");
    self.enableParity.state = (config.flags & CONFIG_ENABLE_PARITY) ? NSOnState : NSOffState;
    self.enableUnitAttention.state = (config.flags & CONFIG_ENABLE_UNIT_ATTENTION) ? NSOnState : NSOffState;
    self.enableSCSI2Mode.state = (config.flags & CONFIG_ENABLE_SCSI2) ? NSOnState : NSOffState;
    self.enableSCSITerminator.state = (config.flags & S2S_CFG_ENABLE_TERMINATOR) ? NSOnState : NSOffState;
    self.enableGlitch.state = (config.flags & CONFIG_DISABLE_GLITCH) ? NSOnState : NSOffState;
    self.enableCache.state = (config.flags & CONFIG_ENABLE_CACHE) ? NSOnState : NSOffState;
    self.enableDisconnect.state = (config.flags & CONFIG_ENABLE_DISCONNECT) ? NSOnState : NSOffState;
    self.respondToShortSCSISelection.state = (config.flags & CONFIG_ENABLE_SEL_LATCH) ? NSOnState : NSOffState;
    self.mapLUNStoSCSIIDs.state = (config.flags & CONFIG_MAP_LUNS_TO_IDS) ? NSOnState : NSOffState;
    self.startupDelay.intValue = config.startupDelay;
    self.startupSelectionDelay.intValue = config.selectionDelay;
    [self.speedLimit selectItemAtIndex: config.scsiSpeed];
}

- (void) setConfigData:(NSData *)data
{
    BoardConfig config;
    const void *bytes;
    bytes = [data bytes];
    memcpy(&config, bytes, sizeof(BoardConfig));
    [self setConfig: config];
}

- (BoardConfig) getConfig
{
    BoardConfig config;
    // NSLog(@"getConfig");
    config.flags |= self.enableSCSITerminator.intValue;

    config.flags =
        (self.enableParity.state == NSOnState ? CONFIG_ENABLE_PARITY : 0) |
        (self.enableUnitAttention.state == NSOnState ? CONFIG_ENABLE_UNIT_ATTENTION : 0) |
        (self.enableSCSI2Mode.state == NSOnState ? CONFIG_ENABLE_SCSI2 : 0) |
        (self.enableGlitch.state == NSOnState ? CONFIG_DISABLE_GLITCH : 0) |
        (self.enableCache.state == NSOnState ? CONFIG_ENABLE_CACHE: 0) |
        (self.enableDisconnect.state == NSOnState ? CONFIG_ENABLE_DISCONNECT: 0) |
        (self.respondToShortSCSISelection.state == NSOnState ? CONFIG_ENABLE_SEL_LATCH : 0) |
        (self.mapLUNStoSCSIIDs.state == NSOnState ? CONFIG_MAP_LUNS_TO_IDS : 0) |
        (self.enableSCSITerminator.state == NSOnState ? S2S_CFG_ENABLE_TERMINATOR : 0);
    config.startupDelay = self.startupDelay.intValue;
    config.selectionDelay = self.startupSelectionDelay.intValue;
    config.scsiSpeed = self.speedLimit.indexOfSelectedItem;
    
    return config;
}

@end
