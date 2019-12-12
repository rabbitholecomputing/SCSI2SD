//
//  SettingsController.h
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "ConfigUtil.hh"

NS_ASSUME_NONNULL_BEGIN

@interface SettingsController : NSObject

@property (weak, nonatomic) IBOutlet NSButton *enableSCSITerminator;
@property (weak, nonatomic) IBOutlet NSPopUpButton *speedLimit;
@property (weak, nonatomic) IBOutlet NSTextField *startupDelay;
@property (weak, nonatomic) IBOutlet NSTextField *startupSelectionDelay;
@property (weak, nonatomic) IBOutlet NSButton *enableParity;
@property (weak, nonatomic) IBOutlet NSButton *enableUnitAttention;
@property (weak, nonatomic) IBOutlet NSButton *enableSCSI2Mode;
@property (weak, nonatomic) IBOutlet NSButton *respondToShortSCSISelection;
@property (weak, nonatomic) IBOutlet NSButton *mapLUNStoSCSIIDs;

- (NSString *) toXml;
- (void) fromXml: (NSXMLElement *)node;
- (void) setConfig: (BoardConfig)config;

@end

NS_ASSUME_NONNULL_END
