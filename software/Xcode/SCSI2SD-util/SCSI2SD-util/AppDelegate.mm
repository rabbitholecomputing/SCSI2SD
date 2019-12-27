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

// #include "z.h"
// #include "ConfigUtil.hh"

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

void dumpSCSICommand(std::vector<uint8_t> buf)
{
    /*
    std::stringstream msg;
    msg << std::hex;
    for (size_t i = 0; i < 32 && i < buf.size(); ++i)
    {
        msg << std::setfill('0') << std::setw(2) <<
        static_cast<int>(buf[i]) << ' ';
    }
    LogMessage(this, msg.str().c_str());
     */
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

@property (nonatomic) IBOutlet SettingsController *settings;
@end

@implementation AppDelegate

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"

- (void) logStringToPanel: (NSString *)logString
{
    NSString *string = [self.logTextView string];
    string = [string stringByAppendingString: logString];
    [self.logTextView setString: string];
}

- (void) startTimer
{
    pollDeviceTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)0.5
                                                       target:self
                                                     selector:@selector(doTimer)
                                                     userInfo:nil
                                                      repeats:YES];
}

- (void) stopTimer
{
    [pollDeviceTimer invalidate];
}

- (void) reset_hid
{
    try
    {
        myHID = SCSI2SD::HID::Open();
    }
    catch (std::exception& e)
    {
        NSLog(@"Exception caught : %s\n", e.what());
    }
}

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
        NSLog(@"Exception caught : %s\n", e.what());
    }
    
    deviceControllers = [[NSMutableArray alloc] initWithCapacity: 7];
    [deviceControllers addObject: _device1];
    [deviceControllers addObject: _device2];
    [deviceControllers addObject: _device3];
    [deviceControllers addObject: _device4];
    [deviceControllers addObject: _device5];
    [deviceControllers addObject: _device6];
    [deviceControllers addObject: _device7];
    
    [self.tabView selectTabViewItemAtIndex:0];
    
    [self startTimer];
    [self loadDefaults: nil];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    // Insert code here to tear down your application
    [pollDeviceTimer invalidate];
    [deviceControllers removeAllObjects];
}

- (void) doTimer
{
    // logSCSI();
    [self logScsiData:self];
    time_t now = time(NULL);
    if (now == myLastPollTime) return;
    myLastPollTime = now;

    // Check if we are connected to the HID device.
    // AND/or bootloader device.
    try
    {
        if (myBootloader)
        {
            // Verify the USB HID connection is valid
            if (!myBootloader->ping())
            {
                myBootloader = SCSI2SD::Bootloader::Open();
               //  myBootloader.reset();
            }
        }

        if (!myBootloader)
        {
            myBootloader = SCSI2SD::Bootloader::Open();

            if (myBootloader)
            {
                [self logStringToPanel:@"SCSI2SD Bootloader Ready"];
            }
        }

        int supressLog = 0;
        if (myHID && myHID->getFirmwareVersion() < MIN_FIRMWARE_VERSION)
        {
            // No method to check connection is still valid.
            // So assume it isn't.
            // myHID.reset();
            [self reset_hid];
            supressLog = 1;
        }
        else if (myHID && !myHID->ping())
        {
            // Verify the USB HID connection is valid
            // myHID.reset();
            [self reset_hid];
        }

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
                        /*
                        std::stringstream msg;
                        msg << "Firmware update required. Version " <<
                            myHID->getFirmwareVersionStr();
                        mmLogStatus(msg.str());*/
                        NSString *log = [NSString stringWithFormat: @"Firmware update required.  Version %s",myHID->getFirmwareVersionStr().c_str()];
                        [self logStringToPanel: log];
                    }
                }
                else
                {
                    NSString *msg = [NSString stringWithFormat: @"SCSI2SD Ready, firmware version %s",myHID->getFirmwareVersionStr().c_str()];
                    [self logStringToPanel:msg];

                    std::vector<uint8_t> csd(myHID->getSD_CSD());
                    std::vector<uint8_t> cid(myHID->getSD_CID());
                    // std::stringstream sdinfo;
                    msg = [NSString stringWithFormat: @"SD Capacity (512-byte sectors): %d",
                        myHID->getSDCapacity()];
                    [self logStringToPanel:msg];

                    // sdinfo << "SD CSD Register: ";
                    msg = [NSString stringWithFormat: @"SD CSD Register: "];
                    [self logStringToPanel:msg];
                    if (sdCrc7(&csd[0], 15, 0) != (csd[15] >> 1))
                    {
                        msg = [NSString stringWithFormat: @"BADCRC"];
                        [self logStringToPanel:msg];
                    }
                    for (size_t i = 0; i < csd.size(); ++i)
                    {
                        /*
                        sdinfo <<
                            std::hex << std::setfill('0') << std::setw(2) <<
                            static_cast<int>(csd[i]); */
                    }
                    //sdinfo << std::endl;
                    //sdinfo << "SD CID Register: ";
                    if (sdCrc7(&cid[0], 15, 0) != (cid[15] >> 1))
                    {
                        // sdinfo << "BADCRC ";
                    }
                    for (size_t i = 0; i < cid.size(); ++i)
                    {
                        /*
                        sdinfo <<
                            std::hex << std::setfill('0') << std::setw(2) <<
                            static_cast<int>(cid[i]); */
                    }

                    //NSLog(@" %@, %s", self, sdinfo.str());

                    /*
                    if (mySelfTestChk->IsChecked())
                    {
                        std::stringstream scsiInfo;
                        scsiInfo << "SCSI Self-Test: " <<
                            (myHID->scsiSelfTest() ? "Passed" : "FAIL");
                        LogMessage(this, "%s", scsiInfo.str());
                    }*/

                    if (!myInitialConfig)
                    {
                    }

                }
            }
            else
            {
                char ticks[] = {'/', '-', '\\', '|'};
                myTickCounter++;
                NSString *ss = [NSString stringWithFormat:@"Searching for SCSI2SD device %c", ticks[myTickCounter % sizeof(ticks)]];
                [self.infoLabel setStringValue: ss];
            }
        }
    }
    catch (std::runtime_error& e)
    {
        // std::cerr << e.what() << std::endl;
        // mmLogStatus(e.what());
        NSLog(@"%s", e.what());
    }

    [self evaluate];
}

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
     [outputString writeToFile:filename atomically:YES encoding:NSUTF8StringEncoding error:NULL];
}

- (IBAction)saveFile:(id)sender
{
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel beginSheetForDirectory:NSHomeDirectory()
                             file:nil
                   modalForWindow:[self mainWindow]
                    modalDelegate:self
                   didEndSelector:@selector(saveFileEnd:)
                      contextInfo:nil];
}

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

        for (; i < [self->deviceControllers count]; ++i)
        {
            DeviceController *devCon = [self->deviceControllers objectAtIndex:i];
            [devCon setTargetConfig: configs.second[i]];
        }
    }
    catch (std::exception& e)
    {
        NSArray *paths = [panel filenames];
        NSString *path = [paths objectAtIndex: 0];
        char *sPath = (char *)[path cStringUsingEncoding:NSUTF8StringEncoding];
        NSLog(@
            "Cannot load settings from file '%s'.\n%s",
            sPath,
            e.what());
    }
}

- (IBAction)openFile:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles: YES];
    [panel setAllowedFileTypes:[NSArray arrayWithObject:@"xml"]];
    [panel beginSheetForDirectory:nil
                             file:nil
                            types:[NSArray arrayWithObject: @"xml"]
                   modalForWindow:[self mainWindow]
                    modalDelegate:self
                   didEndSelector:@selector(openFileEnd:)
                      contextInfo:NULL];
}

- (IBAction) loadDefaults: (id)sender
{
    // myBoardPanel->setConfig(ConfigUtil::DefaultBoardConfig());
    [self.settings setConfig: SCSI2SD::ConfigUtil::DefaultBoardConfig()];
    for (size_t i = 0; i < [deviceControllers count]; ++i)
    {
        // myTargets[i]->setConfig(ConfigUtil::Default(i));
        DeviceController *devCon = [self->deviceControllers objectAtIndex:i];
        [devCon setTargetConfig: SCSI2SD::ConfigUtil::Default(i)];
    }
}

- (IBAction)saveToDevice:(id)sender
{
    [self stopTimer];
    if (!myHID) return;

    [self logStringToPanel: @"Saving configuration"];
    int currentProgress = 0;
    int totalProgress = (int)[deviceControllers count] * SCSI_CONFIG_ROWS + 1;

    // Write board config first.
    int flashRow = SCSI_CONFIG_BOARD_ROW;
    {
        NSString *ss = [NSString stringWithFormat:
                        @"Programming flash array %d row %d", SCSI_CONFIG_ARRAY, flashRow + 1];
        [self logStringToPanel:ss];
        currentProgress += 1;
        [self.progress setDoubleValue:(double)currentProgress];

        std::vector<uint8_t> flashData =
        SCSI2SD::ConfigUtil::boardConfigToBytes([self.settings getConfig]);
        try
        {
            myHID->writeFlashRow(
                SCSI_CONFIG_ARRAY, flashRow, flashData);
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

        TargetConfig config([[deviceControllers objectAtIndex:i] getTargetConfig]);//myTargets[i]->getConfig());
        std::vector<uint8_t> raw(SCSI2SD::ConfigUtil::toBytes(config));

        for (size_t j = 0; j < SCSI_CONFIG_ROWS; ++j)
        {
            NSString *ss = [NSString stringWithFormat:
                            @"Programming flash array %d row %d", SCSI_CONFIG_ARRAY, flashRow + 1];
            [self logStringToPanel:ss];

            currentProgress += 1;
            if (currentProgress == totalProgress)
            {
                [self logStringToPanel:@"Save Complete."];
            }
            [self.progress setDoubleValue:(double)currentProgress];
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
                [self logStringToPanel:ss];
                goto err;
            }
        }
    }

    [self reset_hid];
        //myHID = SCSI2SD::HID::Open();
    goto out;

err:
    [self.progress setDoubleValue: 100.0];
    [self.infoLabel setStringValue: @"Save Failed"];
    goto out;

out:
    [self startTimer];
    return;

}

- (IBAction)loadFromDevice:(id)sender
{
    [self stopTimer];
    if (!myHID) return;

    [self logStringToPanel: @"Loading configuration"];
/*
    wxWindowPtr<wxGenericProgressDialog> progress(
        new wxGenericProgressDialog(
            "Load config settings",
            "Loading config settings",
            100,
            this,
            wxPD_CAN_ABORT | wxPD_REMAINING_TIME)
            ); */

    int currentProgress = 0;
    int totalProgress = (int)([deviceControllers count] * (NSUInteger)SCSI_CONFIG_ROWS + (NSUInteger)1);

    // Read board config first.
    std::vector<uint8_t> boardCfgFlashData;
    int flashRow = SCSI_CONFIG_BOARD_ROW;
    {
        NSString *ss = [NSString stringWithFormat:
                        @"Reading flash array %d row %d", SCSI_CONFIG_ARRAY, flashRow + 1];
        [self logStringToPanel:ss];
        currentProgress += 1;
        try
        {
            myHID->readFlashRow(
                SCSI_CONFIG_ARRAY, flashRow, boardCfgFlashData);
            [_settings setConfig: SCSI2SD::ConfigUtil::boardConfigFromBytes(&boardCfgFlashData[0])];
        }
        catch (std::runtime_error& e)
        {
            NSLog(@"%s",e.what());
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
            NSString *ss = [NSString stringWithFormat:
                            @"Programming flash array %d row %d", SCSI_CONFIG_ARRAY, flashRow + 1];
            [self logStringToPanel:ss];
            currentProgress += 1;
            if (currentProgress == totalProgress)
            {
                [self logStringToPanel: @"Load Complete."];
            }
            [self.progress setDoubleValue:(double)currentProgress];
            
            std::vector<uint8_t> flashData;
            try
            {
                myHID->readFlashRow(
                    SCSI_CONFIG_ARRAY, flashRow + j, flashData);

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
        [[deviceControllers objectAtIndex: i] setTargetConfig: SCSI2SD::ConfigUtil::fromBytes(&raw[0])];
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
    NSLog(@"Load failed");
    [self.progress setDoubleValue: 100.0];
    [self.infoLabel setStringValue: @"Load Failed"];
    goto out;

out:
    [self startTimer];
    return;
}

- (IBAction)upgradeFirmware:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    int prog = 0;
    __block NSString *filename;
    
    [panel beginSheetModalForWindow:[self mainWindow]
                  completionHandler:^(NSModalResponse result) {
        filename = [panel filename];
    }];
    
    while (true)
    {
        try
        {
            if (!myHID)
            {
                [self reset_hid];
//                myHID = SCSI2SD::HID::Open();
            }
            if (myHID)
            {
                NSLog(@"Resetting SCSI2SD into bootloader");

                myHID->enterBootloader();
                // myHID->reset();
            }


            if (!myBootloader)
            {
                myBootloader = SCSI2SD::Bootloader::Open();
                if (myBootloader)
                {
                    NSLog(@"Bootloader found");
                    break;
                }
            }

            else if (myBootloader)
            {
                // Verify the USB HID connection is valid
                if (!myBootloader->ping())
                {
                    NSLog(@"Bootloader ping failed");
                    // myBootloader.reset();
                    myBootloader = SCSI2SD::Bootloader::Open();
                }
                else
                {
                    NSLog(@"Bootloader found");
                    break;
                }
            }
        }
        catch (std::exception& e)
        {
            NSLog(@"%s",e.what());
            [self reset_hid];
//            myHID = SCSI2SD::HID::Open(); // .reset()
            myBootloader = SCSI2SD::Bootloader::Open(); // .reset()
        }
        // wxMilliSleep(100);
        if (false) // !progress->Pulse())
        {
            return; // user cancelled.
        }
    }

    int totalFlashRows = 0;
    NSString *tmpFile = nil;
    try
    {
        zipper::ReaderPtr reader(new zipper::FileReader([filename cStringUsingEncoding:NSUTF8StringEncoding]));
        zipper::Decompressor decomp(reader);
        std::vector<zipper::CompressedFilePtr> files(decomp.getEntries());
        for (auto it(files.begin()); it != files.end(); it++)
        {
            if (myBootloader && myBootloader->isCorrectFirmware((*it)->getPath()))
            {
                NSString *ss = [NSString stringWithFormat: @"Found firmware entry %s within archive %@",
                                (*it)->getPath().c_str(), filename];
                [self logStringToPanel:ss];
                tmpFile = [NSTemporaryDirectory()
                           stringByAppendingPathComponent:
                           [NSString stringWithFormat:
                            @"SCSI2SD_Firmware-%f.scsi2sd",
                            [[NSDate date] timeIntervalSince1970]]];
                zipper::FileWriter out([tmpFile cStringUsingEncoding:NSUTF8StringEncoding]);
                (*it)->decompress(out);
                NSString *msg = [NSString stringWithFormat: @"Firmware extracted to %@",tmpFile];
                [self logStringToPanel:msg];
                break;
            }
        }

        if ([tmpFile isEqualToString:@""])
        {
            // TODO allow "force" option
            [self logStringToPanel:@"Wrong filename"];
            return;
        }

        SCSI2SD::Firmware firmware([tmpFile cStringUsingEncoding:NSUTF8StringEncoding]);
        totalFlashRows = firmware.totalFlashRows();
    }
    catch (std::exception& e)
    {
        NSString *msg = [NSString stringWithFormat:@"Could not open firmware file: %s",e.what()];
        [self logStringToPanel:msg];
        return;
    }

    {
        [self.progress setDoubleValue: (double)((double)prog / (double)totalFlashRows)];
    }

    NSString *msg2 = [NSString stringWithFormat:@"Upgrading firmware from file: %@", tmpFile];
    [self logStringToPanel: msg2];
    try
    {
        myBootloader->load([tmpFile cStringUsingEncoding:NSUTF8StringEncoding], NULL);
        //TheProgressWrapper.clearProgressDialog();
        [self logStringToPanel: @"Firmware update successful"];

    //        myHID = SCSI2SD::HID::Open();
        [self reset_hid];
        myBootloader = SCSI2SD::Bootloader::Open();
        //myHID.reset();
        //myBootloader.reset();
    }
    catch (std::exception& e)
    {
        //TheProgressWrapper.clearProgressDialog();
        //mmLogStatus(e.what());
        //myHID.reset();
        //myBootloader.reset();

        //wxMessageBox(
        //    "Firmware Update Failed",
        //    e.what(),
        //    wxOK | wxICON_ERROR);

        //wxRemoveFile(tmpFile);
    }
}

- (void) bootLoaderUpdateEnd: (NSOpenPanel *)panel
{
    NSArray *paths = [panel filenames];
    if([paths count] == 0)
        return;

    NSString *filename = [paths objectAtIndex: 0];
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
        NSLog(@"Not a valid boot loader file");
        return;
    }
    
    NSLog(@"Upgrading bootloader from file: %@", filename);
    
    int currentProgress = 0;
    int totalProgress = 36;
    
    for (size_t flashRow = 0; flashRow < 36; ++flashRow)
    {
        // std::stringstream ss;
        // ss << "Programming flash array 0 row " << (flashRow);
        // mmLogStatus(ss.str());
        currentProgress += 1;
        
        if (currentProgress == totalProgress)
        {
            // ss.str("Save Complete.");
            // mmLogStatus("Save Complete.");
        }
        
        /*
         if (!progress->Update((100 * currentProgress) / totalProgress,ss.str()))
         {
            goto abort;
         }*/
        
        uint8_t *rowData = data + (flashRow * 256);
        std::vector<uint8_t> flashData(rowData, rowData + 256);
        try
        {
            self->myHID->writeFlashRow(0, (int)flashRow, flashData);
        }
        catch (std::runtime_error& e)
        {
            NSLog(@"%s",e.what());
            goto err;
        }
    }
    
    goto out;
    
err:
    NSLog(@"Bootloader update failed");
    // progress->Update(100, "Bootloader update failed");
    goto out;
    
out:
    return;
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
    
}

- (IBAction)logScsiData:(id)sender
{
    /*
    if (!mySCSILogChk->IsChecked() ||
        !myHID)
    {
        return;
    }
    try
    {
        std::vector<uint8_t> info(HID::HID_PACKET_SIZE);
        if (myHID->readSCSIDebugInfo(info))
        {
            dumpSCSICommand(info);
        }
    }
    catch (std::exception& e)
    {
        LogWarning(this, e.what());
        myHID.reset();
    } */
    // [self logStringToPanel:@"Logging SCSI info \n"];
}

- (void) evaluate
{
    BOOL valid = YES;

    // Check for duplicate SCSI IDs
    std::vector<uint8_t> enabledID;

    // Check for overlapping SD sectors.
    std::vector<std::pair<uint32_t, uint64_t> > sdSectors;

    bool isTargetEnabled = false; // Need at least one enabled
    uint32_t autoStartSector = 0;
    for (size_t i = 0; i < [deviceControllers count]; ++i)
    {
        DeviceController *target = [deviceControllers objectAtIndex:i]; //  getTargetConfig];
    
        [target setAutoStartSectorValue: autoStartSector];
        valid = [target evaluate] && valid;
        if ([target isEnabled])
        {
            isTargetEnabled = true;
            // uint8_t scsiID = [target getSCSIId];
            /*
            if (find(enabledID, scsiID) != enabledID.end())
            {
                [target setDuplicateID: YES]; // true);
                valid = false;
            }
            else
            {
                [target setDuplicateID: NO]; // false);
            }

            //auto sdSectorRange = myTargets[i]->getSDSectorRange();
            for (auto it(sdSectors.begin()); it != sdSectors.end(); ++it)
            {
                if (sdSectorRange.first < it->second &&
                    sdSectorRange.second > it->first)
                {
                    valid = false;
                    [target setSDSectorOverlap: YES];
                }
                else
                {
                    [target setSDSectorOverlap: NO];
                    // myTargets[i]->setSDSectorOverlap(false);
                }
            }
            sdSectors.push_back(sdSectorRange);
            autoStartSector = sdSectorRange.second; */
        }
        else
        {
            [target setDuplicateID:NO];
            [target setSDSectorOverlap:NO];
           // myTargets[i]->setDuplicateID(false);
           // myTargets[i]->setSDSectorOverlap(false);
        }
    }

    valid = valid && isTargetEnabled; // Need at least one.
    
    self.saveMenu.enabled = valid && (myHID->getFirmwareVersion() >= MIN_FIRMWARE_VERSION);
    self.openMenu.enabled = valid && (myHID->getFirmwareVersion() >= MIN_FIRMWARE_VERSION);
    
/*
    mySaveButton->Enable(
        valid &&
        myHID &&
        (myHID->getFirmwareVersion() >= MIN_FIRMWARE_VERSION));

    myLoadButton->Enable(
        myHID &&
        (myHID->getFirmwareVersion() >= MIN_FIRMWARE_VERSION));
 */
    
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
