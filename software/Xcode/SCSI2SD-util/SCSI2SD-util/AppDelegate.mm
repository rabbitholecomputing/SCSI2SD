//
//  AppDelegate.m
//  scsi2sd
//
//  Created by Gregory Casamento on 7/23/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "AppDelegate.hh"
#import "DeviceController.hh"
#import "SettingsController.hh"

#include <vector>
#include <string>
#include "zipper.hh"
#include <signal.h>

#define TIMER_INTERVAL 0.1

void clean_exit_on_sig(int sig_num)
{
    NSLog(@"Signal %d received\n",sig_num);
    exit( 0 ); // exit cleanly...
}

#define MIN_FIRMWARE_VERSION 0x0400
#define MIN_FIRMWARE_VERSION 0x0400

static uint8_t sdCrc7(uint8_t* chr, uint8_t cnt, uint8_t crc)
{
    uint8_t a;
    for(a = 0; a < cnt; a++)
    {
        uint8_t data = chr[a];
        uint8_t i;
        for(i = 0; i < 8; i++)
        {
            crc <<= 1;
            if ((data & 0x80) ^ (crc & 0x80))
            {
                crc ^= 0x09;
            }
            data <<= 1;
        }
    }
    return crc & 0x7F;
}


BOOL RangesIntersect(NSRange range1, NSRange range2) {
    if(range1.location > range2.location + range2.length - 1) return NO;
    if(range2.location > range1.location + range1.length - 1) return NO;
    return YES;
}

@interface AppDelegate ()
{
    NSMutableArray *deviceControllers;
}

@property (nonatomic) IBOutlet NSWindow *window;
@property (nonatomic) IBOutlet NSWindow *mainWindow;
@property (nonatomic) IBOutlet NSTextField *infoLabel;
@property (nonatomic) IBOutlet NSPanel *logPanel;
@property (nonatomic) IBOutlet NSTextView *logTextView;
@property (nonatomic) IBOutlet NSTabView *tabView;

@property (nonatomic) IBOutlet DeviceController *device1;
@property (nonatomic) IBOutlet DeviceController *device2;
@property (nonatomic) IBOutlet DeviceController *device3;
@property (nonatomic) IBOutlet DeviceController *device4;
@property (nonatomic) IBOutlet DeviceController *device5;
@property (nonatomic) IBOutlet DeviceController *device6;
@property (nonatomic) IBOutlet DeviceController *device7;

@property (nonatomic) IBOutlet NSProgressIndicator *progress;

@property (nonatomic) IBOutlet NSMenuItem *saveMenu;
@property (nonatomic) IBOutlet NSMenuItem *openMenu;
@property (nonatomic) IBOutlet NSMenuItem *readMenu;
@property (nonatomic) IBOutlet NSMenuItem *writeMenu;
@property (nonatomic) IBOutlet NSMenuItem *scsiSelfTest;
@property (nonatomic) IBOutlet NSMenuItem *scsiLogData;
@property (nonatomic) IBOutlet NSMenuItem *upgradeFirmware;

@property (nonatomic) IBOutlet SettingsController *settings;
@property (nonatomic) IBOutlet NSWindow *customAboutWindow;

@end

@implementation AppDelegate

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
// Update progress...
- (NSString *) percentString: (NSNumber *)prog
{
    NSUInteger n = [prog unsignedIntegerValue];
    
    NSString *t1 = (n >= 10) ? @"X" :  @"-";
    NSString *t11 = (n >= 15) ? @"X" :  @"-";
    NSString *t2 = (n >= 20) ? @"X" :  @"-";
    NSString *t21 = (n >= 25) ? @"X" :  @"-";
    NSString *t3 = (n >= 30) ? @"X" :  @"-";
    NSString *t31 = (n >= 35) ? @"X" :  @"-";
    NSString *t4 = (n >= 40) ? @"X" :  @"-";
    NSString *t41 = (n >= 45) ? @"X" :  @"-";
    NSString *t5 = (n >= 50) ? @"X" :  @"-";
    NSString *t51 = (n >= 55) ? @"X" :  @"-";
    NSString *t6 = (n >= 60) ? @"X" :  @"-";
    NSString *t61 = (n >= 65) ? @"X" :  @"-";
    NSString *t7 = (n >= 70) ? @"X" :  @"-";
    NSString *t71 = (n >= 75) ? @"X" :  @"-";
    NSString *t8 = (n >= 80) ? @"X" :  @"-";
    NSString *t81 = (n >= 85) ? @"X" :  @"-";
    NSString *t9 = (n >= 90) ? @"X" :  @"-";
    NSString *t91 = (n >= 95) ? @"X" :  @"-";
    NSString *t10 = (n >= 100) ? @"X" :  @"-";
    NSString *percString = [NSString stringWithFormat:
                            @"[%@%@%@%@%@%@%@%@%@%@%@%@%@%@%@%@%@%@%@] %3u%%\n",
                            t1,
                            t11,
                            t2,
                            t21,
                            t3,
                            t31,
                            t4,
                            t41,
                            t5,
                            t51,
                            t6,
                            t61,
                            t7,
                            t71,
                            t8,
                            t81,
                            t9,
                            t91,
                            t10,
                            (unsigned int)n];
    return percString;
}

- (void) updateProgress: (NSNumber *)prog
{
    //self.logTextView.string =
    //    [self.logTextView.string substringToIndex: [self.logTextView.string length] - 24];
    [self logStringToPanel: [self percentString:prog]];
    [self.progress setDoubleValue: [prog doubleValue]];
}

- (void) showProgress: (id)sender
{
    [self.progress setHidden:NO];
}

- (void) hideProgress: (id)sender
{
    [self.progress setHidden:YES];
}

// Output to the debug info panel...
- (void) logStringToPanel: (NSString *)logString
{
    NSString *string = [self.logTextView string];
    string = [string stringByAppendingString: logString];
    [self.logTextView setString: string];
    [self.logTextView scrollToEndOfDocument:self];
}

// Output to the label...
- (void) logStringToLabel: (NSString *)logString
{
    [self.infoLabel setStringValue:logString];
}

// Start polling for the device...
- (void) startTimer
{
    pollDeviceTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)TIMER_INTERVAL
                                                       target:self
                                                     selector:@selector(doTimer)
                                                     userInfo:nil
                                                      repeats:YES];
}

// Pause the timer...
- (void) stopTimer
{
    [pollDeviceTimer invalidate];
}

// Reset the HID...
- (void) reset_hid
{
    try
    {
        myHID = SCSI2SD::HID::Open();
        if(myHID)
        {
            NSString *msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version %s",myHID->getFirmwareVersionStr().c_str()];
            [self logStringToLabel:msg];
        }
    }
    catch (std::exception& e)
    {
        NSLog(@"reset_hid: Exception caught : %s\n", e.what());
    }
}

- (void) reset_bootloader
{
    try
    {
        myBootloader = SCSI2SD::Bootloader::Open();
    }
    catch (std::exception& e)
    {
        NSLog(@"reset_bootloader: Exception caught : %s\n", e.what());
    }
}

// Initialize everything once we finish launching...
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    signal(SIGINT , clean_exit_on_sig);
    signal(SIGABRT , clean_exit_on_sig);
    signal(SIGILL , clean_exit_on_sig);
    signal(SIGFPE , clean_exit_on_sig);
    signal(SIGSEGV, clean_exit_on_sig); // <-- this one is for segmentation fault
    signal(SIGTERM , clean_exit_on_sig);
    
    try
    {
        myHID = SCSI2SD::HID::Open();
        myBootloader = SCSI2SD::Bootloader::Open();
    }
    catch (std::exception& e)
    {
        NSLog(@"launch: Exception caught : %s\n", e.what());
    }
    
    deviceControllers = [[NSMutableArray alloc] initWithCapacity: 7];
    [deviceControllers addObject: _device1];
    [deviceControllers addObject: _device2];
    [deviceControllers addObject: _device3];
    [deviceControllers addObject: _device4];
    
    [self.tabView selectTabViewItemAtIndex:0];
    [self.progress setMinValue: 0.0];
    [self.progress setMaxValue: 100.0];
    self.logTextView.font = [NSFont userFixedPitchFontOfSize:10.0];
    
    doScsiSelfTest = NO;
    shouldLogScsiData = NO;
    
    [[self window] makeKeyAndOrderFront:self];
    [self startTimer];
    [self loadDefaults: nil];
}

// Shutdown everything when termination happens...
- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    // Insert code here to tear down your application
    [pollDeviceTimer invalidate];
    [deviceControllers removeAllObjects];
}

- (void) dumpScsiData: (std::vector<uint8_t>) buf
{
    NSString *msg = @"";
    for (size_t i = 0; i < 32 && i < buf.size(); ++i)
    {
        msg = [msg stringByAppendingFormat:@"%02x ", static_cast<int>(buf[i])];
    }
    [self logStringToPanel: msg];
    [self logStringToPanel: @"\n"];
}

- (IBAction)handleAboutPanel:(id)sender
{
    [self.customAboutWindow orderFrontRegardless];
}

- (BOOL) validateMenuItem:(NSMenuItem *)menuItem
{
    if (menuItem == self.writeMenu ||
        menuItem == self.readMenu ||
        // menuItem == self.upgradeFirmware ||
        menuItem == self.scsiLogData ||
        menuItem == self.scsiSelfTest)
    {
        return (myHID != NULL); // [self evaluate];
    }
    return YES;
}

- (void) connectionTests
{
    NSString *msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version %s",myHID->getFirmwareVersionStr().c_str()];
    [self logStringToLabel:msg];
    [self logStringToPanel:[NSString stringWithFormat: @"%@: %@", [NSDate date], msg]];
    
    std::vector<uint8_t> csd(myHID->getSD_CSD());
    std::vector<uint8_t> cid(myHID->getSD_CID());
    msg = [NSString stringWithFormat: @"\nSD Capacity (512-byte sectors): %d",
        myHID->getSDCapacity()];
    [self logStringToPanel:msg];

    msg = [NSString stringWithFormat: @"\nSD CSD Register: "];
    [self logStringToPanel:msg];
    if (sdCrc7(&csd[0], 15, 0) != (csd[15] >> 1))
    {
        msg = [NSString stringWithFormat: @"\nBADCRC"];
        [self logStringToPanel:msg];
    }
    for (size_t i = 0; i < csd.size(); ++i)
    {
        [self logStringToPanel:[NSString stringWithFormat: @"%x", static_cast<int>(csd[i])]];
    }
    [self logStringToPanel:@"\n"];
    
    msg = [NSString stringWithFormat: @"SD CID Register: "];
    [self logStringToPanel:msg];
    if (sdCrc7(&cid[0], 15, 0) != (cid[15] >> 1))
    {
        msg = [NSString stringWithFormat: @"BADCRC"];
        [self logStringToPanel:msg];
    }
    for (size_t i = 0; i < cid.size(); ++i)
    {
        [self logStringToPanel:[NSString stringWithFormat: @"%x", static_cast<int>(cid[i])]];
    }
    [self logStringToPanel:@"\n"];
    
    if(doScsiSelfTest)
    {
        BOOL passed = myHID->scsiSelfTest();
        NSString *status = passed ? @"Passed" : @"FAIL";
        [self logStringToPanel:[NSString stringWithFormat: @"\nSCSI Self Test: %@\n", status]];
    }
    
    if (!myInitialConfig)
    {
    }
    
    // update menu items...
    self.writeMenu.enabled = YES;
    self.readMenu.enabled = YES;
    self.upgradeFirmware.enabled = YES;
}

// Periodically check to see if Device is present...
- (void) doTimer
{
    static BOOL checked = NO;
    if(shouldLogScsiData == YES)
    {
        [self logScsiData];
    }
    time_t now = time(NULL);
    if (now == myLastPollTime) return;
    myLastPollTime = now;

    // update menu items...
    self.writeMenu.enabled = NO;
    self.readMenu.enabled = NO;
    self.upgradeFirmware.enabled = NO;
    
    // Check if we are connected to the HID device.
    // AND/or bootloader device.
    try
    {
        if (myBootloader)
        {
            // Verify the USB HID connection is valid
            if (!myBootloader->ping())
            {
                [self reset_bootloader];
            }
        }

        if (!myBootloader)
        {
            [self reset_bootloader];
            if (myBootloader)
            {
                [self logStringToPanel:@"SCSI2SD Bootloader Ready"];
                NSString *msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version %s",myHID->getFirmwareVersionStr().c_str()];
                [self logStringToLabel:msg];
            }
        }
        else
        {
            [self logStringToPanel:@"SCSI2SD Bootloader Ready"];
            if (myHID)
            {
                NSString *msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version %s",myHID->getFirmwareVersionStr().c_str()];
                [self logStringToLabel:msg];
            }
        }

        BOOL supressLog = NO;
        if (myHID && myHID->getFirmwareVersion() < MIN_FIRMWARE_VERSION)
        {
            // No method to check connection is still valid.
            [self reset_hid];
            if (myHID == NULL)
               return;
          
            std::string vers = myHID->getFirmwareVersionStr();
            NSString *version = [NSString stringWithCString: vers.c_str()
                                                   encoding: NSUTF8StringEncoding];
            NSString *msg = nil;
            if ([version rangeOfString: @"Unknown"].location != NSNotFound)
              {
                [self reset_hid];
                [self reset_bootloader];
                /*
                // Put into the panel...
                msg = @"SCSI2SD NOT Ready, Firmware invalid.  Check that board version is 5.x.";
                [self logStringToLabel:msg];
                
                // Show alert...
                NSAlert *alert = [NSAlert alertWithMessageText: @"SCSI2SD NOT Ready"
                                                 defaultButton: @"Ok"
                                               alternateButton: nil
                                                   otherButton: nil
                                     informativeTextWithFormat: @"Firmware invalid.  Check that board version is 5.x."];
                [alert runModal];
                 */
              }
           else
             {
               msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version %@",version];
               [self logStringToLabel:msg];
             }
        }
        else if (myHID && !myHID->ping())
        {
            // Verify the USB HID connection is valid
            [self reset_hid];
        }
        else
        {
            if(myHID)
            {
                //NSString *msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version //%s",myHID->getFirmwareVersionStr().c_str()];
                //[self logStringToLabel:msg];
                if (checked == NO)
                {
                    checked = YES;
                    [self connectionTests];
                }
                // [self logStringToPanel:[NSString stringWithFormat: @"%@: %@", [NSDate date], msg]];
            }
        }

        char ticks[] = {'/', '-', '\\', '|'};
        if (!myHID)
        {
            [self reset_hid];
            if (myHID)
            {
                if (myHID->getFirmwareVersion() < MIN_FIRMWARE_VERSION)
                {
                    if (!supressLog)
                    {
                        // Oh dear, old firmware
                        //NSString *log = [NSString stringWithFormat: @"Firmware update required.  Version %s",myHID->getFirmwareVersionStr().c_str()];
                        //[self logStringToLabel: log];
                      myTickCounter++;
                      NSString *ss = [NSString stringWithFormat:@"Searching for SCSI2SD device %c", ticks[myTickCounter % sizeof(ticks)]];
                      [self logStringToLabel: ss];
                    }
                }
                else
                {
                    [self connectionTests];
                }
            }
              
            myTickCounter++;
            NSString *ss = [NSString stringWithFormat:@"Searching for SCSI2SD device %c", ticks[myTickCounter % sizeof(ticks)]];
            [self logStringToLabel: ss];
        }
    }
    catch (std::runtime_error& e)
    {
        [self logStringToPanel:[NSString stringWithFormat:@"%s\n", e.what()]];
    }

    [self evaluate];
}

// Save XML file
- (void)saveFileEnd: (NSOpenPanel *)panel
{
    NSString *filename = [[panel directory] stringByAppendingPathComponent: [[panel filename] lastPathComponent]];
    if([filename isEqualToString:@""] || filename == nil)
        return;
    
     NSString *outputString = @"";
     filename = [filename stringByAppendingPathExtension:@"xml"];
     outputString = [outputString stringByAppendingString: @"<SCSI2SD>\n"];

     outputString = [outputString stringByAppendingString: [self->_settings toXml]];
     DeviceController *dc = nil;
     NSEnumerator *en = [self->deviceControllers objectEnumerator];
     while((dc = [en nextObject]) != nil)
     {
         outputString = [outputString stringByAppendingString: [dc toXml]];
     }
     outputString = [outputString stringByAppendingString: @"</SCSI2SD>\n"];
    
     NSError *err = nil;
     [outputString writeToFile:filename atomically:YES encoding:NSUTF8StringEncoding error:&err];
     if (err != nil)
     {
         NSAlert *alert = [NSAlert alertWithError:err];
         [alert runModal];
     }
     else
     {
         NSAlert *alert = [NSAlert alertWithMessageText:@"Save complete" defaultButton:@"Ok" alternateButton:nil otherButton:nil informativeTextWithFormat:@"Saved XML file successfully"];
         [alert runModal];
     }
}

// Save XML file....
- (IBAction)saveFile:(id)sender
{
#ifdef GNUSTEP
    // Do the save..
    NSSavePanel *panel = [NSSavePanel savePanel];
    NSString *defaultPath = [@"~/Downloads" stringByExpandingTildeInPath];
    [panel beginSheetForDirectory:defaultPath
                             file:nil
                   modalForWindow:[self mainWindow]
                    modalDelegate:self
                   didEndSelector:@selector(saveFileEnd:)
                      contextInfo:nil];
#else
    // Authorize the dir
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setMessage: @"Authorize the folder where you want to save (necessary for security)"];
    [openPanel setPrompt: @"Select"];
    [openPanel setCanChooseFiles: NO];
    [openPanel setCanChooseDirectories: YES];
    [openPanel setCanCreateDirectories: YES];
    [openPanel beginWithCompletionHandler:^(NSModalResponse result) {
        // Do the save..
        NSSavePanel *panel = [NSSavePanel savePanel];
        [panel setDirectoryURL: openPanel.URL];
        [panel beginSheetForDirectory:[[openPanel URL] path]
                                 file:nil
                       modalForWindow:[self mainWindow]
                        modalDelegate:self
                       didEndSelector:@selector(saveFileEnd:)
                          contextInfo:nil];
    }];
#endif
}

// Open XML file...
- (void) openFileEnd: (NSOpenPanel *)panel
{
    try
    {
        NSArray *paths = [panel filenames];
        if([paths count] == 0)
            return;
        
        NSString *path = [paths objectAtIndex: 0];
        char *sPath = (char *)[path cStringUsingEncoding:NSUTF8StringEncoding];
        std::pair<BoardConfig, std::vector<TargetConfig>> configs(
            SCSI2SD::ConfigUtil::fromXML(std::string(sPath)));

        // myBoardPanel->setConfig(configs.first);
        [self.settings setConfig:configs.first];
        size_t i;
        for (i = 0; i < configs.second.size() && i < [self->deviceControllers count]; ++i)
        {
            DeviceController *devCon = [self->deviceControllers objectAtIndex:i];
            [devCon setTargetConfig: configs.second[i]];
        }

        /*
        for (; i < [self->deviceControllers count]; ++i)
        {
            if (i >= 4)
            {
                break;
            }
            DeviceController *devCon = [self->deviceControllers objectAtIndex:i];
            [devCon setTargetConfig: configs.second[i]];
        } */
    }
    catch (std::exception& e)
    {
        NSArray *paths = [panel filenames];
        NSString *path = [paths objectAtIndex: 0];
        char *sPath = (char *)[path cStringUsingEncoding:NSUTF8StringEncoding];
        NSString *msg = [NSString stringWithFormat:
                         @"Cannot load settings from file '%s'.\n%s",
                         sPath,
                         e.what()];
        NSRunAlertPanel(@"Error while loading XML", msg, @"Ok", nil, nil);
        [self logStringToPanel:msg];
    }
}

// Open file panel
- (IBAction)openFile:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles: YES];
    NSString *defaultPath = [@"~/Downloads" stringByExpandingTildeInPath];
    [panel setDirectoryURL:[NSURL fileURLWithPath:defaultPath isDirectory:YES]];
    [panel setAllowedFileTypes:[NSArray arrayWithObject:@"xml"]];
    [panel beginSheetForDirectory:nil
                             file:nil
                            types:[NSArray arrayWithObject: @"xml"]
                   modalForWindow:[self mainWindow]
                    modalDelegate:self
                   didEndSelector:@selector(openFileEnd:)
                      contextInfo:NULL];
}

// Load defaults into all configs...
- (IBAction) loadDefaults: (id)sender
{
    // myBoardPanel->setConfig(ConfigUtil::DefaultBoardConfig());
    [self.settings setConfig: SCSI2SD::ConfigUtil::DefaultBoardConfig()];

    for (size_t i = 0; i < [deviceControllers count]; ++i)
    {
        // myTargets[i]->setConfig(ConfigUtil::Default(i));
        DeviceController *devCon = [self->deviceControllers objectAtIndex:i];
        [devCon setTargetConfig: SCSI2SD::ConfigUtil::Default(i)];
        [devCon autoStartSector].enabled = ([devCon enableSCSITarget].state == NSOnState);
        [devCon evaluateSize];
    }
}

// Save information to device on background thread....
- (void) saveToDeviceThread: (id)obj
{
    [self performSelectorOnMainThread:@selector(stopTimer)
                           withObject:NULL
                        waitUntilDone:NO];

    if (!myHID) return;

    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                            withObject: @"Saving configuration\n\n"
                         waitUntilDone:YES];
    
    [self performSelectorOnMainThread:@selector(updateProgress:)
                           withObject:[NSNumber numberWithDouble:0.0]
                        waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(showProgress:)
                           withObject:nil
                        waitUntilDone:NO];
    
    int currentProgress = 0;
    int totalProgress = (int)[deviceControllers count] * SCSI_CONFIG_ROWS + 1;

    // Write board config first.
    int flashRow = SCSI_CONFIG_BOARD_ROW;
    {
        NSLog(@"currentProgress = %d", currentProgress);
        double perc = ((double)currentProgress/(double)totalProgress) * 100.0;
        [self performSelectorOnMainThread:@selector(updateProgress:)
                               withObject:[NSNumber numberWithDouble: perc]
                            waitUntilDone:NO];

        std::vector<uint8_t> flashData =
        SCSI2SD::ConfigUtil::boardConfigToBytes([self.settings getConfig]);
        try
        {
            myHID->writeFlashRow(
                SCSI_CONFIG_ARRAY, flashRow, flashData);
            currentProgress += 1;
        }
        catch (std::runtime_error& e)
        {
            NSLog(@"%s",e.what());
            goto err;
        }
    }

    flashRow = SCSI_CONFIG_0_ROW;
    for (size_t i = 0;
        i < [deviceControllers count];
        ++i)
    {
        flashRow = (i <= 3)
            ? SCSI_CONFIG_0_ROW + ((int)i*SCSI_CONFIG_ROWS)
            : SCSI_CONFIG_4_ROW + ((int)(i-4)*SCSI_CONFIG_ROWS);

        TargetConfig config([[deviceControllers objectAtIndex:i] getTargetConfig]);
        std::vector<uint8_t> raw(SCSI2SD::ConfigUtil::toBytes(config));

        for (size_t j = 0; j < SCSI_CONFIG_ROWS; ++j)
        {
            currentProgress += 1;
            double perc = ((double)currentProgress/(double)totalProgress) * 100.0;
            [self performSelectorOnMainThread:@selector(updateProgress:)
                                   withObject:[NSNumber numberWithDouble: perc]
                                waitUntilDone:NO];
            if (currentProgress == totalProgress)
            {
                [self performSelectorOnMainThread:@selector(logStringToPanel:)
                                       withObject:@"\n\nSave complete"
                                    waitUntilDone:YES];
            }

            std::vector<uint8_t> flashData(SCSI_CONFIG_ROW_SIZE, 0);
            std::copy(
                &raw[j * SCSI_CONFIG_ROW_SIZE],
                &raw[(1+j) * SCSI_CONFIG_ROW_SIZE],
                flashData.begin());
            try
            {
                myHID->writeFlashRow(
                    SCSI_CONFIG_ARRAY, (int)flashRow + (int)j, flashData);
            }
            catch (std::runtime_error& e)
            {
                NSString *ss = [NSString stringWithFormat:
                                @"Error: %s", e.what()];
                [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                        withObject:ss
                                     waitUntilDone:YES];
                goto err;
            }
        }
    }

    [self reset_hid];
    goto out;

err:
    [self performSelectorOnMainThread:@selector(updateProgress:)
                           withObject:[NSNumber numberWithDouble: (double)100.0]
                        waitUntilDone:NO];
    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                            withObject:@"\n\nSave Failed"
                         waitUntilDone:YES];
    goto out;

out:
    [NSThread sleepForTimeInterval:1.0];
    [self performSelectorOnMainThread:@selector(hideProgress:)
                           withObject:nil
                        waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(startTimer)
                           withObject:NULL
                        waitUntilDone:NO];

    return;
}

- (IBAction)saveToDevice:(id)sender
{
    [NSThread detachNewThreadSelector:@selector(saveToDeviceThread:) toTarget:self withObject:self];
}

- (void) loadFromDeviceThread: (id)obj
{
    [self performSelectorOnMainThread:@selector(stopTimer)
                           withObject:NULL
                        waitUntilDone:NO];
    
    if (!myHID) // goto out;
    {
        [self reset_hid];
        if (!myHID) return;
    }
    
    if(!myHID)
    {
        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                               withObject:@"Couldn't initialize HID configuration"
                            waitUntilDone:YES];
        [self startTimer];
        return;
    }
    
    
    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                           withObject:@"\nLoading configuration\n"
                        waitUntilDone:YES];
    
    [self performSelectorOnMainThread:@selector(updateProgress:)
                           withObject:[NSNumber numberWithDouble:0.0]
                        waitUntilDone:NO];
    
    [self performSelectorOnMainThread:@selector(showProgress:)
                           withObject:nil
                        waitUntilDone:NO];


    int currentProgress = 0;
    int totalProgress = (int)([deviceControllers count] * (NSUInteger)SCSI_CONFIG_ROWS + (NSUInteger)1);

    // Read board config first.
    std::vector<uint8_t> boardCfgFlashData;
    int flashRow = SCSI_CONFIG_BOARD_ROW;
    {
        currentProgress += 2;
        try
        {
            myHID->readFlashRow(
                SCSI_CONFIG_ARRAY, flashRow, boardCfgFlashData);
            BoardConfig bConfig = SCSI2SD::ConfigUtil::boardConfigFromBytes(&boardCfgFlashData[0]);
            NSData *configBytes = [[NSData alloc] initWithBytes: &bConfig length:sizeof(BoardConfig)];
            [_settings performSelectorOnMainThread:@selector(setConfigData:)
                                        withObject:configBytes
                                     waitUntilDone:YES];
        }
        catch (std::runtime_error& e)
        {
            NSLog(@"%s",e.what());
            NSString *ss = [NSString stringWithFormat:
                            @"\n\nException: %s\n\n",e.what()];
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                    withObject:ss
                                 waitUntilDone:YES];
            goto err;
        }
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    for (size_t i = 0;
        i < [deviceControllers count];
        ++i)
    {
        flashRow = (i <= 3)
            ? SCSI_CONFIG_0_ROW + (i*SCSI_CONFIG_ROWS)
            : SCSI_CONFIG_4_ROW + ((i-4)*SCSI_CONFIG_ROWS);
        std::vector<uint8_t> raw(sizeof(TargetConfig));

        for (size_t j = 0; j < SCSI_CONFIG_ROWS; ++j)
        {
            double perc = ((double)currentProgress/(double)totalProgress) * 100.0;
            [self performSelectorOnMainThread:@selector(updateProgress:)
                                   withObject:[NSNumber numberWithDouble: perc]
                                waitUntilDone:NO];
            
            if (currentProgress == totalProgress)
            {
                [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                        withObject:@"\nRead Complete.\n"
                                     waitUntilDone:YES];
            }
            
            std::vector<uint8_t> flashData;
            try
            {
                myHID->readFlashRow(
                    SCSI_CONFIG_ARRAY, flashRow + j, flashData);
                currentProgress += 1;
            }
            catch (std::runtime_error& e)
            {
                NSLog(@"%s",e.what());
                goto err;
            }

            std::copy(
                flashData.begin(),
                flashData.end(),
                &raw[j * SCSI_CONFIG_ROW_SIZE]);
        }
#pragma GCC diagnostic pop
        TargetConfig tConfig = SCSI2SD::ConfigUtil::fromBytes(&raw[0]);
        NSData *configBytes = [[NSData alloc] initWithBytes: &tConfig length:sizeof(TargetConfig)];
        [[deviceControllers objectAtIndex: i] performSelectorOnMainThread:@selector(setTargetConfigData:)
                                                               withObject:configBytes
                                                            waitUntilDone:YES];
    }

    // Support old boards without board config set
    if (memcmp(&boardCfgFlashData[0], "BCFG", 4)) {
        BoardConfig defCfg = SCSI2SD::ConfigUtil::DefaultBoardConfig();
        defCfg.flags = [[deviceControllers objectAtIndex:0] getTargetConfig].flagsDEPRECATED;
        [_settings setConfig:defCfg];
    }

    myInitialConfig = true;
    goto out;

err:
    [self performSelectorOnMainThread:@selector(updateProgress:)
                           withObject:[NSNumber numberWithDouble:(double)100.0]
                        waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(logStringToPanel:)
                           withObject:@"Load failed"
                        waitUntilDone:NO];
    goto out;

out:
    [NSThread sleepForTimeInterval:1.0];
    [self performSelectorOnMainThread:@selector(hideProgress:)
                           withObject:nil
                        waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(startTimer)
                           withObject:NULL
                        waitUntilDone:NO];

    return;
}

- (IBAction)loadFromDevice:(id)sender
{
    [NSThread detachNewThreadSelector:@selector(loadFromDeviceThread:) toTarget:self withObject:self];
}

- (void) fakeFirmwareProgress
{
    NSInteger i = 0;
    for(i = 0; i <= 100; i++)
    {
        [self performSelectorOnMainThread:@selector(updateProgress:)
                               withObject:[NSNumber numberWithInteger:i]
                            waitUntilDone:YES];
        // [self updateProgress:[NSNumber numberWithInteger:i]];
        [NSThread sleepForTimeInterval:0.02];
    }
}

- (void) showAlertWithMessage: (NSString *)msg
{
    NSRunAlertPanel(@"Firmware Update", @"Success", @"Ok", nil, nil);
}

- (void) upgradeFirmwareThread: (NSString *)filename
{
    [self performSelectorOnMainThread:@selector(stopTimer)
                           withObject:NULL
                        waitUntilDone:NO];

    if(filename != nil)
    {
        int prog = 0;
        while (true)
        {
            try
            {
                if (!myHID)
                {
                    [self reset_hid];
                }
                
                if (myHID)
                {
                    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                            withObject: @"Resetting SCSI2SD Into Bootloader\n"
                                         waitUntilDone:YES];
                    myHID->enterBootloader();
                }

                if (!myBootloader)
                {
                    [self reset_bootloader];
                    if (myBootloader)
                    {
                        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                                withObject: @"Bootloader found\n"
                                             waitUntilDone:YES];
                        break;
                    }
                }
                else if (myBootloader)
                {
                    if (!myBootloader->ping())
                    {
                        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                                withObject: @"Bootloader ping failed\n"
                                             waitUntilDone:YES];
                    }
                    else
                    {
                        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                                withObject: @"Bootloader found\n"
                                             waitUntilDone:YES];
                        break;
                    }
                }
            }
            catch (std::exception& e)
            {
                NSLog(@"%s",e.what());
                [self reset_hid];
            }
            [NSThread sleepForTimeInterval:0.1];
        }

        int totalFlashRows = 0;
        NSString *tmpFile = [NSTemporaryDirectory()
                             stringByAppendingPathComponent:
                             [NSString stringWithFormat:
                              @"SCSI2SD_Firmware-%f.scsi2sd",
                              [[NSDate date] timeIntervalSince1970]]];
        std::string name = std::string([tmpFile cStringUsingEncoding:NSASCIIStringEncoding]);
        
        try
        {
            zipper::ReaderPtr reader(new zipper::FileReader([filename cStringUsingEncoding:NSASCIIStringEncoding]));
            zipper::Decompressor decomp(reader);
            std::vector<zipper::CompressedFilePtr> files(decomp.getEntries());
                        
            for (auto it(files.begin());it != files.end(); it++)
            {
                if (myBootloader && myBootloader->isCorrectFirmware((*it)->getPath()))
                {
                    NSString *ss = [NSString stringWithFormat:
                                    @"\nFound firmware entry %s within archive %@\n",
                                    (*it)->getPath().c_str(), filename];
                    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                            withObject: ss
                                         waitUntilDone:YES];
                    
                    zipper::FileWriter out(name);
                    (*it)->decompress(out);
                    NSString *msg = [NSString stringWithFormat:
                                     @"\nFirmware extracted to %@\n",tmpFile];
                    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                            withObject: msg
                                         waitUntilDone:YES];
                    break;
                }
            }

            if ([tmpFile isEqualToString:@""])
            {
                [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                        withObject: @"\nWrong filename\n"
                                     waitUntilDone:YES];
                return;
            }

            SCSI2SD::Firmware firmware(name);
            totalFlashRows = firmware.totalFlashRows();
        }
        catch (std::exception& e)
        {
            NSString *msg = [NSString stringWithFormat:@"Could not open firmware file: %s\n",e.what()];
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                    withObject:msg
                                 waitUntilDone:YES];
            return;
        }

        [self performSelectorOnMainThread:@selector(showProgress:) withObject:nil waitUntilDone:YES];
        [self performSelectorOnMainThread:@selector(updateProgress:)
                               withObject:[NSNumber numberWithDouble:0] // (double)((double)prog / (double)totalFlashRows)]
                            waitUntilDone:NO];

        NSString *msg2 = [NSString stringWithFormat:@"Upgrading firmware from file: %@\n", tmpFile];
        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                withObject:msg2
                             waitUntilDone:YES];
        try
        {
            [self fakeFirmwareProgress];
            myBootloader->load(name, NULL);

            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                    withObject: @"Firmware update successful\n"
                                 waitUntilDone:YES];            
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                    withObject: [NSString stringWithFormat: @"Rows written: %d\n", totalFlashRows]
                                 waitUntilDone:YES];
        }
        catch (std::exception& e)
        {
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                   withObject: [NSString stringWithFormat:@"%s\n",e.what()]
                                waitUntilDone: YES];
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                   withObject: @"Firmware update failed!\n"
                                waitUntilDone: YES];
        }
    }
    
    [self performSelectorOnMainThread:@selector(updateProgress:)
                           withObject:[NSNumber numberWithDouble:100.0]
                        waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(hideProgress:) withObject:nil waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(showAlertWithMessage:) withObject: nil waitUntilDone:NO];
    [self performSelectorOnMainThread:@selector(startTimer)
                           withObject:NULL
                        waitUntilDone:NO];
}

- (void) upgradeFirmwareEnd: (NSOpenPanel *)panel
{
    NSArray *paths = [panel filenames];
    if([paths count] == 0)
        return;
    [NSThread detachNewThreadSelector:@selector(upgradeFirmwareThread:)
                             toTarget:self
                           withObject:[paths objectAtIndex:0]];
}

- (IBAction)upgradeFirmware:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel beginSheetForDirectory:NULL
                             file:NULL
                            types:[NSArray arrayWithObjects:@"scsi2sd",@"cyacd",nil]
                   modalForWindow:[self mainWindow]
                    modalDelegate:self
                   didEndSelector: @selector(upgradeFirmwareEnd:)
                      contextInfo:NULL];
}

- (void)bootloaderUpdateThread: (NSString *)filename
{
#ifndef GNUSTEP
    NSData *fileData = [NSData dataWithContentsOfFile:filename];
    NSUInteger len = [fileData length];
    if (len != 0x2400)
    {
        NSLog(@"Incorrect size, invalid boodloader");
        return;
    }
    
    uint8_t *data = (uint8_t *)[fileData bytes];
    static char magic[] = {
        'P', 0, 'S', 0, 'o', 0, 'C', 0, '3', 0, ' ', 0,
        'B', 0, 'o', 0, 'o', 0, 't', 0, 'l', 0, 'o', 0, 'a', 0, 'd', 0, 'e', 0, 'r', 0};
    
    uint8_t* dataEnd = data + sizeof(data);
    if (std::search(data, dataEnd, magic, magic + sizeof(magic)) >= dataEnd)
    {
        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                               withObject: [NSString stringWithFormat:@"\nNot a valid bootloader file: %@\n", filename]
                            waitUntilDone: YES];
        return;
    }
    
    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                           withObject: [NSString stringWithFormat:@"\nUpgrading bootloader from file: %@\n", filename]
                        waitUntilDone: YES];

    int currentProgress = 0;
    int totalProgress = 36;
    
    for (size_t flashRow = 0; flashRow < 36; ++flashRow)
    {
        [self performSelectorOnMainThread: @selector(logStringToPanel:)
                               withObject: [NSString stringWithFormat:
                                @"\nProgramming bootloader flash array 0 row %zu",
                                flashRow]
                            waitUntilDone: YES];
        currentProgress += 1;
        
        if (currentProgress == totalProgress)
        {
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                   withObject: @"Programming bootloader complete"
                                waitUntilDone: YES];
        }
        
        uint8_t *rowData = data + (flashRow * 256);
        std::vector<uint8_t> flashData(rowData, rowData + 256);
        try
        {
            self->myHID->writeFlashRow(0, (int)flashRow, flashData);
        }
        catch (std::runtime_error& e)
        {
            [self performSelectorOnMainThread: @selector(logStringToPanel:)
                                   withObject: [NSString stringWithFormat: @"%s", e.what()]
                                waitUntilDone: YES];
            goto err;
        }
    }
    
    goto out;
    
err:
    [self performSelectorOnMainThread: @selector(logStringToPanel:)
                           withObject: @"Programming bootloader failed"
                        waitUntilDone: YES];
    [self performSelectorOnMainThread:@selector(updateProgress:)
                           withObject:[NSNumber numberWithDouble:100.0]
                        waitUntilDone:NO];
    goto out;
    
out:
    return;
#endif
}

- (void) bootLoaderUpdateEnd: (NSOpenPanel *)panel
{
    NSArray *paths = [panel filenames];
    if([paths count] == 0)
        return;

    NSString *filename = [paths objectAtIndex: 0];
    [NSThread detachNewThreadSelector:@selector(bootloaderUpdateThread:)
                             toTarget:self
                           withObject:filename];
}

- (IBAction)bootloaderUpdate:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    [panel beginSheetForDirectory:nil
                             file:nil
                            types:nil
                   modalForWindow:[self mainWindow]
                    modalDelegate:self
                   didEndSelector:@selector(bootLoaderUpdateEnd:)
                      contextInfo:nil];
}


- (IBAction)scsiSelfTest:(id)sender
{
    NSMenuItem *item = (NSMenuItem *)sender;
    if(item.state == NSControlStateValueOn)
    {
        item.state = NSControlStateValueOff;
    }
    else
    {
        item.state = NSControlStateValueOn;
    }
    doScsiSelfTest = (item.state == NSControlStateValueOn);
}

- (IBAction) shouldLogScsiData: (id)sender
{
    NSMenuItem *item = (NSMenuItem *)sender;
    if(item.state == NSControlStateValueOn)
    {
        item.state = NSControlStateValueOff;
        [self logStringToPanel:@"END Logging SCSI info \n"];
    }
    else
    {
        item.state = NSControlStateValueOn;
        [self logStringToPanel:@"START Logging SCSI info \n"];
    }
    shouldLogScsiData = (item.state == NSControlStateValueOn);
}

- (void)logScsiData
{
    BOOL checkSCSILog = shouldLogScsiData;   // replce this with checking the menu status
    if (!checkSCSILog ||
        !myHID)
    {
        return;
    }
    try
    {
        std::vector<uint8_t> info(SCSI2SD::HID::HID_PACKET_SIZE);
        if (myHID->readSCSIDebugInfo(info))
        {
            [self dumpScsiData: info];
        }
    }
    catch (std::exception& e)
    {
        NSString *warning = [NSString stringWithFormat: @"Warning: %s", e.what()];
        [self logStringToPanel: warning];
        // myHID = SCSI2SD::HID::Open();
        [self reset_hid]; // myHID->reset();
    }
}

- (IBAction) enableTarget: (id)sender
{
    [self autoButton:sender];
}

- (IBAction) autoButton: (id)sender
{
    // recalculate...
    NSUInteger secStart = 0;
    NSUInteger index = 0;
    NSEnumerator *en = [deviceControllers objectEnumerator];
    DeviceController *dev = nil;
    while ((dev = [en nextObject]) != nil)
    {
        if ([dev isEnabled] && dev.autoStartSector.state == NSOnState)
        {
            NSRange sectorRange = [dev getSDSectorRange];
            NSUInteger len = sectorRange.length;
            secStart += len + 1;
            index++; // update next one...
            
            if (index < [deviceControllers count])
            {
                DeviceController *devToUpdate = [deviceControllers objectAtIndex:index];
                if ([devToUpdate isEnabled] && devToUpdate.autoStartSector.state == NSOnState)
                {
                    [devToUpdate setAutoStartSectorValue:secStart];
                }
            }
        }
    }
}

- (BOOL) evaluate
{
    BOOL valid = YES;

    // Check for duplicate SCSI IDs
    std::vector<uint8_t> enabledID;

    // Check for overlapping SD sectors.
    std::vector<std::pair<uint32_t, uint64_t> > sdSectors;

    bool isTargetEnabled = false; // Need at least one enabled
    for (size_t i = 0; i < [deviceControllers count]; ++i)
    {
        DeviceController *target = [deviceControllers objectAtIndex: i]; //  getTargetConfig];
    
        // [target setAutoStartSectorValue: autoStartSector];
        valid = [target evaluate] && valid;
        if ([target isEnabled])
        {
            isTargetEnabled = true;
            uint8_t scsiID = [target getSCSIId];
            for (size_t j = 0; j < [deviceControllers count]; ++j)
            {
                DeviceController *t2 = [deviceControllers objectAtIndex: j];
                if (![t2 isEnabled] || t2 == target)
                    continue;
                
                uint8_t sid2 = [t2 getSCSIId];
                if(sid2 == scsiID)
                {
                    [target setDuplicateID:YES];
                    valid = false;
                }
                else
                {
                    [target setDuplicateID:NO];
                    valid = true;
                }
            }

            NSRange sdSectorRange = [target getSDSectorRange];
            for (size_t k = 0; k < [deviceControllers count]; ++k)
            {
                DeviceController *t3 = [deviceControllers objectAtIndex: k];
                if (![t3 isEnabled] || t3 == target)
                    continue;

                NSRange sdr = [t3 getSDSectorRange];
                if(RangesIntersect(sdSectorRange, sdr))
                {
                    valid = false;
                    [target setSDSectorOverlap: YES];
                }
                else
                {
                    valid = true;
                    [target setSDSectorOverlap: NO];
                }
            }
            // sdSectors.push_back(sdSectorRange);
            // autoStartSector = sdSectorRange.second;
        }
        else
        {
            [target setDuplicateID:NO];
            [target setSDSectorOverlap:NO];
        }
    }

    valid = valid && isTargetEnabled; // Need at least one.
    
    if(myHID)
    {
        self.saveMenu.enabled = valid && (myHID->getFirmwareVersion() >= MIN_FIRMWARE_VERSION);
        self.openMenu.enabled = valid && (myHID->getFirmwareVersion() >= MIN_FIRMWARE_VERSION);
    }
    
    return valid;
}
#pragma GCC diagnostic pop


- (NSInteger)numberOfItemsInComboBoxCell:(NSComboBox *)comboBox
{
    return 8;
}

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)comboBox
{
    return 8;
}

- (nullable id)comboBox:(NSComboBox *)comboBox objectValueForItemAtIndex:(NSInteger)index
{
    return [NSString stringWithFormat:@"%ld", (long)index];
}

- (nullable id)comboBoxCall:(NSComboBox *)comboBox objectValueForItemAtIndex:(NSInteger)index
{
    return [NSString stringWithFormat:@"%ld", (long)index];
}
@end
