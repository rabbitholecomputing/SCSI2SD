//
//  AppDelegate.m
//  scsi2sd
//
//  Created by Gregory Casamento on 7/23/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "AppDelegate.hh"
#import "DeviceController.h"
#import "SettingsController.h"

#include <vector>
#include <string>
#include "ConfigUtil.hh"

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
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
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
                    [devCon setConfig: configs.second[i]];
                }

                for (; i < [self->deviceControllers count]; ++i)
                {
                    DeviceController *devCon = [self->deviceControllers objectAtIndex:i];
                    [devCon setConfig: configs.second[i]];
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
        [devCon setConfig: SCSI2SD::ConfigUtil::Default(i)];
    }
}

- (IBAction)saveToDevice:(id)sender
{
    
}

- (IBAction)loadFromDevice:(id)sender
{
    
}

- (IBAction)upgradeFirmware:(id)sender
{
    
}

- (IBAction)scsiSelfTest:(id)sender
{
    
}

- (IBAction)logScsiData:(id)sender
{
    
}

- (void) evaluate
{
    bool valid = true;

    // Check for duplicate SCSI IDs
    std::vector<uint8_t> enabledID;

    // Check for overlapping SD sectors.
    std::vector<std::pair<uint32_t, uint64_t> > sdSectors;

    bool isTargetEnabled = false; // Need at least one enabled
    uint32_t autoStartSector = 0;
    for (size_t i = 0; i < myTargets.size(); ++i)
    {
        myTargets[i]->setAutoStartSector(autoStartSector);
        valid = myTargets[i]->evaluate() && valid;

        if (myTargets[i]->isEnabled())
        {
            isTargetEnabled = true;
            uint8_t scsiID = myTargets[i]->getSCSIId();
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
