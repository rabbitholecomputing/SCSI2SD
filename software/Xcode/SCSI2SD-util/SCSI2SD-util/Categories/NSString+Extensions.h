//
//  NSString+Extensions.h
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/14/19.
//  Copyright © 2019 Rabbit Hole Computing, LLC. All rights reserved.
//

#import <AppKit/AppKit.h>


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSString (Extensions)

+ (NSString *) stringWithCString: (char *)cstring length: (NSUInteger)length;

@end

NS_ASSUME_NONNULL_END
