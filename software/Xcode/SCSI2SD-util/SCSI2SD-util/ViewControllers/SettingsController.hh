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

- (NSString *) toXml;
- (void) setConfig: (BoardConfig)config;
- (void) setConfigData: (NSData *)data;
- (BoardConfig) getConfig;

@end

NS_ASSUME_NONNULL_END
