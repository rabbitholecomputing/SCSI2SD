//
//  NSString+Extensions.m
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/14/19.
//  Copyright © 2019 RabbitHole Computing, LLC. All rights reserved.
//

#import "NSString+Extensions.h"

#import <AppKit/AppKit.h>


@implementation NSString (Extensions)

+ (NSString *) stringWithCString: (char *)cstring length: (NSUInteger)length
{
    NSData *data = [NSData dataWithBytes:cstring length:length];
    NSString *result = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    return result;
}


@end
