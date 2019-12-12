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
//#include "ConfigUtil.hh"

#define MIN_FIRMWARE_VERSION 0x0400
#define MIN_FIRMWARE_VERSION 0x0400

enum
{
    ID_ConfigDefaults = 0, // wxID_HIGHEST + 1,
    ID_Firmware,
    ID_Bootloader,
    ID_Timer,
    ID_Notebook,
    ID_BtnLoad,
    ID_BtnSave,
    ID_LogWindow,
    ID_SCSILog,
    ID_SelfTest,
    ID_SaveFile,
    ID_OpenFile
};

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

@property (weak) IBOutlet NSWindow *window;
@property (weak, nonatomic) IBOutlet NSWindow *mainWindow;
@property (weak, nonatomic) IBOutlet NSTextField *infoLabel;
@property (weak, nonatomic) IBOutlet NSPanel *logPanel;
@property (weak, nonatomic) IBOutlet NSTextView *logTextView;

@property (weak, nonatomic) IBOutlet DeviceController *device1;
@property (weak, nonatomic) IBOutlet DeviceController *device2;
@property (weak, nonatomic) IBOutlet DeviceController *device3;
@property (weak, nonatomic) IBOutlet DeviceController *device4;
@property (weak, nonatomic) IBOutlet DeviceController *device5;
@property (weak, nonatomic) IBOutlet DeviceController *device6;
@property (weak, nonatomic) IBOutlet DeviceController *device7;

@property (weak, nonatomic) IBOutlet SettingsController *settings;
@end

@implementation AppDelegate

- (void) logStringToPanel: (NSString *)logString
{
    NSString *string = [self.logTextView string];
    string = [string stringByAppendingString: logString];
    [self.logTextView setString: string];
}

- (void) startTimer
{
    pollDeviceTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)0.5
                                                      repeats:YES
                                                        block:^(NSTimer * _Nonnull timer) {
        [self doTimer];
    }];
}

- (void) stopTimer
{
    [pollDeviceTimer invalidate];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    myHID = SCSI2SD::HID::Open();
    myBootloader = SCSI2SD::Bootloader::Open();
    
    deviceControllers = [[NSMutableArray alloc] initWithCapacity: 7];
    [deviceControllers addObject: _device1];
    [deviceControllers addObject: _device2];
    [deviceControllers addObject: _device3];
    [deviceControllers addObject: _device4];
    [deviceControllers addObject: _device5];
    [deviceControllers addObject: _device6];
    [deviceControllers addObject: _device7];
    
    [self startTimer];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
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
            myHID = SCSI2SD::HID::Open();
            supressLog = 1;
        }
        else if (myHID && !myHID->ping())
        {
            // Verify the USB HID connection is valid
            // myHID.reset();
            myHID = SCSI2SD::HID::Open();
        }

        if (!myHID)
        {
            myHID = SCSI2SD::HID::Open();
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
                    /*
                    std::stringstream msg;
                    msg << "SCSI2SD Ready, firmware version " <<
                        myHID->getFirmwareVersionStr();
                    mmLogStatus(msg.str()); */
                    NSLog(@"SCSI2SD Ready, firmware version %s",myHID->getFirmwareVersionStr().c_str());

                    std::vector<uint8_t> csd(myHID->getSD_CSD());
                    std::vector<uint8_t> cid(myHID->getSD_CID());
                    // std::stringstream sdinfo;
                    NSLog(@"SD Capacity (512-byte sectors): %d",
                        myHID->getSDCapacity());

                    // sdinfo << "SD CSD Register: ";
                    NSLog(@"SD CSD Register: ");
                    if (sdCrc7(&csd[0], 15, 0) != (csd[15] >> 1))
                    {
                        // sdinfo << "BADCRC ";
                        NSLog(@"BADCRC");
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
/* This doesn't work properly, and causes crashes.
                        wxCommandEvent loadEvent(wxEVT_NULL, ID_BtnLoad);
                        GetEventHandler()->AddPendingEvent(loadEvent);
*/
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

- (IBAction)saveFile:(id)sender
{
    NSString *outputString = @"";
    outputString = [outputString stringByAppendingString: @"<SCSI2SD>\n"];

    outputString = [outputString stringByAppendingString: [_settings toXml]];
    DeviceController *dc = nil;
    NSEnumerator *en = [deviceControllers objectEnumerator];
    while((dc = [en nextObject]) != nil)
    {
        outputString = [outputString stringByAppendingString: [dc toXml]];
    }
    outputString = [outputString stringByAppendingString: @"</SCSI2SD>\n"];
}

- (IBAction)openFile:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles: YES];
    [panel setRequiredFileType:@"xml"];
    
    [panel beginSheet:[self mainWindow]
    completionHandler:^(NSModalResponse returnCode) {
        if(returnCode == NSModalResponseOK ||
           returnCode == NSModalResponseContinue)
        {
            try
            {
                NSArray *paths = [panel filenames];
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
    }];
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
    // TimerLock lock(myTimer);
    if (!myHID) return;

    NSLog(@"Saving configuration");
    /*
    wxWindowPtr<wxGenericProgressDialog> progress(
        new wxGenericProgressDialog(
            "Save config settings",
            "Saving config settings",
            100,
            this,
            wxPD_CAN_ABORT | wxPD_REMAINING_TIME)
            ); */


    int currentProgress = 0;
    int totalProgress = (int)[deviceControllers count] * SCSI_CONFIG_ROWS + 1;

    // Write board config first.
    int flashRow = SCSI_CONFIG_BOARD_ROW;
    {
        //std::stringstream ss;
        //ss << "Programming flash array " << SCSI_CONFIG_ARRAY <<
        //    " row " << flashRow;
        //mmLogStatus(ss.str());
        currentProgress += 1;

        /*
        if (!progress->Update(
                (100 * currentProgress) / totalProgress,
                ss.str()
                )
            )
        {
            goto abort;
        }*/

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
            //std::stringstream ss;
            //ss << "Programming flash array " << SCSI_CONFIG_ARRAY <<
            //    " row " << (flashRow + j);
            //mmLogStatus(ss.str());
            currentProgress += 1;

            if (currentProgress == totalProgress)
            {
                // ss.str("Save Complete.");
                NSLog(@"Save Complete.");
            }
            /*
            if (!progress->Update(
                    (100 * currentProgress) / totalProgress,
                    ss.str()
                    )
                )
            {
                goto abort;
            }
             */
            std::vector<uint8_t> flashData(SCSI_CONFIG_ROW_SIZE, 0);
            std::copy(
                &raw[j * SCSI_CONFIG_ROW_SIZE],
                &raw[(1+j) * SCSI_CONFIG_ROW_SIZE],
                flashData.begin());
            try
            {
                myHID->writeFlashRow(
                    SCSI_CONFIG_ARRAY, flashRow + j, flashData);
            }
            catch (std::runtime_error& e)
            {
                NSLog(@"%s",e.what());
                goto err;
            }
        }
    }

    myHID = SCSI2SD::HID::Open();

    goto out;

err:
    NSLog(@"Save failed");
    // progress->Update(100, "Save failed");
    goto out;

abort:
    NSLog(@"Save Aborted");

out:
    return;

}

- (IBAction)loadFromDevice:(id)sender
{
    //TimerLock lock(myTimer);
    if (!myHID) return;

    NSLog(@"Loading configuration");
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
    int totalProgress = [deviceControllers count] * SCSI_CONFIG_ROWS + 1;

    // Read board config first.
    std::vector<uint8_t> boardCfgFlashData;
    int flashRow = SCSI_CONFIG_BOARD_ROW;
    {
        //std::stringstream ss;
        //ss << "Reading flash array " << SCSI_CONFIG_ARRAY <<
         //   " row " << flashRow;
        //mmLogStatus(ss.str());
        currentProgress += 1;

        /*
        if (!progress->Update(
                (100 * currentProgress) / totalProgress,
                ss.str()
                )
            )
        {
            goto abort;
        }*/

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
            //std::stringstream ss;
            //ss << "Reading flash array " << SCSI_CONFIG_ARRAY <<
            //    " row " << (flashRow + j);
            //mmLogStatus(ss.str());
            currentProgress += 1;
            if (currentProgress == totalProgress)
            {
                NSLog(@"Load Complete.");
                // mmLogStatus("Load Complete.");
            }

            /*
            if (!progress->Update(
                    (100 * currentProgress) / totalProgress,
                    ss.str()
                    )
                )
            {
                goto abort;
            }
*/
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
        [[deviceControllers objectAtIndex: i] setTargetConfig: SCSI2SD::ConfigUtil::fromBytes(&raw[0])];
        // myTargets[i]->setConfig(ConfigUtil::fromBytes(&raw[0]));
    }

    // Support old boards without board config set
    if (memcmp(&boardCfgFlashData[0], "BCFG", 4)) {
        BoardConfig defCfg = SCSI2SD::ConfigUtil::DefaultBoardConfig();
        // defCfg.flags = myTargets[0]->getConfig().flagsDEPRECATED;
        defCfg.flags = [[deviceControllers objectAtIndex:0] getTargetConfig].flagsDEPRECATED;
        // myBoardPanel->setConfig(defCfg);
        [_settings setConfig:defCfg];
    }

    myInitialConfig = true;
    goto out;

err:
    NSLog(@"Load failed");
    // progress->Update(100, "Load failed");
    goto out;

abort:
    NSLog(@"Load Aborted");

out:
    return;
}

- (IBAction)upgradeFirmware:(id)sender
{
    while (true)
    {
        try
        {
            if (!myHID)
            {
                myHID = SCSI2SD::HID::Open();
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
            myHID = SCSI2SD::HID::Open(); // .reset()
            myBootloader = SCSI2SD::Bootloader::Open(); // .reset()
        }
        // wxMilliSleep(100);
        if (false) // !progress->Pulse())
        {
            return; // user cancelled.
        }
    }

    int totalFlashRows = 0;
    std::string tmpFile;
    try
    { /*
        zipper::ReaderPtr reader(new zipper::FileReader(filename));
        zipper::Decompressor decomp(reader);
        std::vector<zipper::CompressedFilePtr> files(decomp.getEntries());
        for (auto it(files.begin()); it != files.end(); it++)
        {
            if (myBootloader && myBootloader->isCorrectFirmware((*it)->getPath()))
            {
                //std::stringstream msg;
                //msg << "Found firmware entry " << (*it)->getPath() <<
                //    " within archive " << filename;
                //mmLogStatus(msg.str());
                tmpFile =
                    wxFileName::CreateTempFileName(
                        _("SCSI2SD_Firmware"), static_cast<wxFile*>(NULL)
                        );
                zipper::FileWriter out(tmpFile);
                (*it)->decompress(out);
                msg.clear();
                msg << "Firmware extracted to " << tmpFile;
                mmLogStatus(msg.str());
                break;
            }
        }

        if (tmpFile.empty())
        {
            // TODO allow "force" option
            wxMessageBox(
                "Wrong filename",
                "Wrong filename",
                wxOK | wxICON_ERROR);
            return;
        }

        Firmware firmware(tmpFile);
        totalFlashRows = firmware.totalFlashRows(); */
    }
    catch (std::exception& e)
    { /*
        mmLogStatus(e.what());
        std::stringstream msg;
        msg << "Could not open firmware file: " << e.what();
        wxMessageBox(
            msg.str(),
            "Bad file",
            wxOK | wxICON_ERROR);
        wxRemoveFile(tmpFile);
        return; */
    }

    { /*
        wxWindowPtr<wxGenericProgressDialog> progress(
            new wxGenericProgressDialog(
                "Loading firmware",
                "Loading firmware",
                totalFlashRows,
                this,
                wxPD_AUTO_HIDE | wxPD_REMAINING_TIME)
                );
        TheProgressWrapper.setProgressDialog(progress, totalFlashRows); */
    }

    //std::stringstream msg;
    //msg << "Upgrading firmware from file: " << tmpFile;
    //mmLogStatus(msg.str());

    try
    {
        //myBootloader->load(tmpFile, &ProgressUpdate);
        //TheProgressWrapper.clearProgressDialog();

        //wxMessageBox(
        //    "Firmware update successful",
        //    "Firmware OK",
        //    wxOK);
//mmLogStatus("Firmware update successful");


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


- (IBAction)bootloaderUpdate:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    [panel beginSheet:[self mainWindow] completionHandler:^(NSModalResponse returnCode) {
        if(returnCode == NSModalResponseOK ||
           returnCode == NSModalResponseContinue)
        {
            NSArray *paths = [panel filenames];
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
                if (!progress->Update(
                                      (100 * currentProgress) / totalProgress,
                                      ss.str()
                                      )
                    )
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
        }
        
    err:
        NSLog(@"Bootloader update failed");
        // progress->Update(100, "Bootloader update failed");
        goto out;

    abort:
        NSLog(@"Bootloader update aborted");

    out:
        return;
        
    }];
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
    [self logStringToPanel:@"Logging SCSI info \n"];
}

- (void) evaluate
{
    /*
    bool valid = true;

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
            uint8_t scsiID = [target getSCSIIdVal];
            //if (find(enabledID, scsiID) != enabledID.end())
            {
                myTargets[i]->setDuplicateID(true);
                valid = false;
            }
            //else
            {
                //enabledID.insert(scsiID);
                myTargets[i]->setDuplicateID(false);
            }

            auto sdSectorRange = myTargets[i]->getSDSectorRange();
            for (auto it(sdSectors.begin()); it != sdSectors.end(); ++it)
            {
                if (sdSectorRange.first < it->second &&
                    sdSectorRange.second > it->first)
                {
                    valid = false;
                    myTargets[i]->setSDSectorOverlap(true);
                }
                else
                {
                    myTargets[i]->setSDSectorOverlap(false);
                }
            }
            sdSectors.push_back(sdSectorRange);
            autoStartSector = sdSectorRange.second;
        }
        else
        {
            myTargets[i]->setDuplicateID(false);
            myTargets[i]->setSDSectorOverlap(false);
        }
    }

    valid = valid && isTargetEnabled; // Need at least one.
     */
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

@end
