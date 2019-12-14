//
//  DeviceController.m
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "DeviceController.hh"

@implementation DeviceController

- (void) setTargetConfig: (TargetConfig)config
{
    NSLog(@"setTargetConfig");
}

- (TargetConfig) getTargetConfig
{
    TargetConfig targetConfig;
    NSLog(@"getTargetConfig");
    return targetConfig;
}

- (NSString *) toXml
{
    return nil;
}

- (void) fromXml: (NSXMLElement *)node
{
    NSLog(@"fromXml");
}

- (BOOL) evaluate
{
    NSLog(@"fromXml");
    return YES;
}

@end
