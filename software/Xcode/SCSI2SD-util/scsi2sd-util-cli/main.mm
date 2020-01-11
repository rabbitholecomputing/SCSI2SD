//
//  main.m
//  scsi2sd-util-cli
//
//  Created by Gregory Casamento on 1/10/20.
//  Copyright © 2020 RabbitHole Computing, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SCSI2SDTask.hh"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSProcessInfo *info = [NSProcessInfo processInfo];
        SCSI2SDTask *task = [SCSI2SDTask task];
        
        BOOL parseSuccessful = NO;
        // NSLog(@"info.arguments = %@", info.arguments);
        
        if([info.arguments count] == 3) // arguments includes the command...
        {
            puts("=== SCSI2SD-util-cli utility v1.0 ===");
            NSString *filename = [info.arguments objectAtIndex: 2];
            const char *f = (const char *)[filename cStringUsingEncoding:NSUTF8StringEncoding];
            if([[info.arguments objectAtIndex:1] isEqualToString:@"-s"])
            {
                printf("Saving file to filesystem from device: %s\n", f);
                parseSuccessful = YES;
                [task saveFromDeviceFromFilename:filename];
            }
            else if([[info.arguments objectAtIndex:1] isEqualToString:@"-l"])
            {
                printf("Loading file from filesystem to device: %s\n", f);
                parseSuccessful = YES;
                [task saveToDeviceFromFilename:filename];

            }
            else if([[info.arguments objectAtIndex:1] isEqualToString:@"-f"])
            {
                printf("Loading firmware from filesystem to device: %s\n", f);
                parseSuccessful = YES;
                [task upgradeFirmwareDeviceFromFilename:filename];
            }
            puts("=== Operation completed.");
        }
        
        if(parseSuccessful == NO)
        {
            // insert code here...
            puts("scsi2sd-util-cli usage: ");
            puts("\t-s save_file.xml -- save file from scsi2sd device");
            puts("\t-l load_file.xml -- save file to scsi2sd device");
            puts("\t-f firmware-file.scsi2sd -- update firmware from file");
        }
    }
    return 0;
}
