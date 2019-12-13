//
//  SettingsController.m
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "SettingsController.hh"

@implementation SettingsController

- (NSString *) toXml
{
    NSLog(@"toXML");
    return nil;
}

- (void) fromXml: (NSXMLElement *)node
{
    NSLog(@"fromXml");

}

- (void) setConfig: (BoardConfig)config
{
    NSLog(@"setConfig");

}

- (BoardConfig) getConfig
{
    BoardConfig config;
    NSLog(@"getConfig");
    // return NULL;
    return config;
}

@end
