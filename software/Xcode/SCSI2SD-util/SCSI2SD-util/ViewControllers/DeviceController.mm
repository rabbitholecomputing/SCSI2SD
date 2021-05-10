//
//  DeviceController.m
//  scsi2sd
//
//  Created by Gregory Casamento on 12/3/18.
//  Copyright © 2018 Open Logic. All rights reserved.
//

#import "DeviceController.hh"
#import "NSString+Extensions.h"
#import "AppDelegate.hh"

#include "ConfigUtil.hh"

@interface DeviceController ()

@property (readwrite) IBOutlet NSButton *enableSCSITarget;
@property (readwrite) IBOutlet NSComboBox *SCSIID;
@property (readwrite) IBOutlet NSPopUpButton *deviceType;
@property (readwrite) IBOutlet NSTextField *sdCardStartSector;
@property (readwrite) IBOutlet NSTextField *sectorSize;
@property (readwrite) IBOutlet NSTextField *sectorCount;
@property (readwrite) IBOutlet NSTextField *deviceSize;
@property (readwrite) IBOutlet NSPopUpButton *deviceUnit;
@property (readwrite) IBOutlet NSTextField *vendor;
@property (readwrite) IBOutlet NSTextField *productId;
@property (readwrite) IBOutlet NSTextField *revsion;
@property (readwrite) IBOutlet NSTextField *serialNumber;
@property (readwrite) IBOutlet NSButton *autoStartSector;
@property (readwrite) IBOutlet NSTextField *sectorsPerTrack;
@property (readwrite) IBOutlet NSTextField *headsPerCylinder;

@property (readwrite) IBOutlet NSTextField *autoErrorText;
@property (readwrite) IBOutlet NSTextField *scsiIdErrorText;

@end

@implementation DeviceController

- (IBAction) enableSCSITarget: (id)sender
{
    NSButton *btn = (NSButton *)sender;
    self.autoStartSector.enabled = (btn.state == NSOnState);
}

- (void) awakeFromNib
{
    self.enableSCSITarget.toolTip = @"Enable this device";
    self.SCSIID.toolTip = @"Unique SCSI ID for target device";
    self.deviceType.toolTip = @"Dervice type: HD, Removable, etc";
    self.sdCardStartSector.toolTip = @"Supports multiple SCSI targets";
    self.sectorSize.toolTip = @"Between 64 and 8192. Default of 512 is suitable in most cases.";
    self.sectorCount.toolTip = @"Number of sectors (device size)";
    self.deviceSize.toolTip = @"Device size";
    self.deviceUnit.toolTip = @"Units for device: GB, MB, etc";
    self.vendor.toolTip = @"SCSI Vendor string. eg. ' codesrc'";
    self.productId.toolTip = @"SCSI Product ID string. eg. 'SCSI2SD";
    self.revsion.toolTip = @"SCSI device revision string. eg. '3.5a'";
    self.serialNumber.toolTip = @"SCSI serial number. eg. '13eab5632a'";
    self.autoStartSector.toolTip = @"Auto start sector based on other targets";
    self.sectorsPerTrack.toolTip = @"Number of sectors in each track";
    self.headsPerCylinder.toolTip = @"Number of heads in cylinder";
    
    // Initial values
    self.autoErrorText.stringValue = @"";
    self.scsiIdErrorText.stringValue = @"";
    
    self.sdCardStartSector.formatter = nil;
    
    // Set delegate..
    self.sectorCount.delegate = self;
    self.sdCardStartSector.delegate = self;
    self.deviceSize.delegate = self;
    
    // Set target action for some controls...
    [self.deviceUnit setTarget: self];
    [self.deviceUnit setAction: @selector(selectDeviceUnit:)];
    self.sdCardStartSector.delegate = self;
    self.sectorSize.delegate = self;
    self.sectorCount.delegate = self;
    self.deviceSize.delegate = self;
    
    self.SCSIID.dataSource = self;
    self.SCSIID.usesDataSource = YES;
    // numDevs = 10;
    [self.SCSIID reloadData];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(controlTextDidChange:) name:NSControlTextDidChangeNotification object:nil];
    
    [self evaluate];
}

// Data source for combobox
- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)comboBox
{
    return numDevs;
}

- (nullable id)comboBox:(NSComboBox *)comboBox objectValueForItemAtIndex:(NSInteger)index
{
    return [NSNumber numberWithLong:index];
}
// end data source

- (void) updateSCSIIDsTo: (NSNumber *)num
{
    NSUInteger n = [num longValue];
    numDevs = n;
    [self.SCSIID setNumberOfVisibleItems: numDevs];
    [self.SCSIID reloadData];
}

- (IBAction) selectDeviceUnit: (id)sender
{
    AppDelegate *delegate = (AppDelegate *)[NSApp delegate];
    NSUInteger num_sectors = [self convertUnitsToSectors];

    self.sectorCount.stringValue = [NSString stringWithFormat:@"%lld",(long long)num_sectors];

    [delegate evaluate];
}

- (void) setTargetConfig: (TargetConfig)config
{
    NSInteger sectors = (NSInteger)(config.scsiSectors);
    NSInteger bytesPerSector = (NSInteger)(config.bytesPerSector);
    NSInteger deviceSize = (NSInteger)((sectors * bytesPerSector) / (1024 * 1024 * 1024));
    
    self.enableSCSITarget.state = (config.scsiId & 0x80) ? NSOnState : NSOffState;
    [self.SCSIID setStringValue:
     [NSString stringWithFormat: @"%d", (config.scsiId & 0x80) ?
      (config.scsiId - 0x80) : config.scsiId]];
    [self.deviceType selectItemAtIndex: config.deviceType];
    [self.sdCardStartSector setStringValue:[NSString stringWithFormat:@"%d", config.sdSectorStart]];
    [self.sectorSize setStringValue: [NSString stringWithFormat: @"%d", config.bytesPerSector]];
    [self.sectorCount setStringValue: [NSString stringWithFormat: @"%d", config.scsiSectors]];
    [self.deviceSize setStringValue: [NSString stringWithFormat: @"%lld", (long long)deviceSize]];

    // Sectors per track...
    [self.vendor setStringValue: [NSString stringWithCString:config.vendor length:8]];
    [self.productId setStringValue: [NSString stringWithCString:config.prodId length:16]];
    [self.revsion setStringValue: [NSString stringWithCString:config.revision length:4]];
    [self.serialNumber setStringValue: [NSString stringWithCString:config.serial length:16]];
    [self.sectorsPerTrack setStringValue: [NSString stringWithFormat: @"%d", config.sectorsPerTrack]];
    [self.headsPerCylinder setStringValue: [NSString stringWithFormat: @"%d", config.headsPerCylinder]];
    // [self.autoStartSector setState:]
    
    [self evaluateSize];
}

- (void) setTargetConfigData: (NSData *)data
{
    TargetConfig config;
    const void *bytes;
    bytes = [data bytes];
    memcpy(&config, bytes, sizeof(TargetConfig));
    [self setTargetConfig: config];
}

- (TargetConfig) getTargetConfig
{
    TargetConfig targetConfig;
    
    //NSLog(@"getTargetConfig");
    // targetConfig.scsiId = (self.SCSIID.indexOfSelectedItem) + self.enableSCSITarget.state == NSOnState ? 0x80:0x0;
    //NSLog(@"%d", self.SCSIID.intValue);
    targetConfig.scsiId = self.SCSIID.intValue + (self.enableSCSITarget.state == NSOnState ? 0x80 : 0x0);
    //NSLog(@"%d",  targetConfig.scsiId );
    targetConfig.deviceType = self.deviceType.indexOfSelectedItem;
    targetConfig.sdSectorStart = self.sdCardStartSector.intValue;
    targetConfig.bytesPerSector = self.sectorSize.intValue;
    targetConfig.scsiSectors = self.sectorCount.intValue;
    strncpy(targetConfig.vendor, [self.vendor.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 8);
    strncpy(targetConfig.prodId, [self.productId.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 16);
    strncpy(targetConfig.revision, [self.revsion.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 4);
    strncpy(targetConfig.serial, [self.serialNumber.stringValue cStringUsingEncoding:NSUTF8StringEncoding], 16);

    return targetConfig;
}

- (NSString *) toXml
{
    TargetConfig config = [self getTargetConfig];
    std::string str = SCSI2SD::ConfigUtil::toXML(config);
    NSString *result = [NSString stringWithCString:str.c_str() encoding:NSUTF8StringEncoding];
    return result;
}

- (BOOL) isEnabled
{
    return self.enableSCSITarget.state == NSOnState;
}

- (NSUInteger) getSCSIId
{
    return (NSUInteger)self.SCSIID.integerValue;
}

- (void) setDuplicateID: (BOOL)flag
{
    self.duplicateId = flag;
    if(flag)
        self.scsiIdErrorText.stringValue = @"Duplicate IDs.";
    else
        self.scsiIdErrorText.stringValue = @"";
}

- (void) setSDSectorOverlap: (BOOL)flag
{
    self.sectorOverlap = flag;
    if(flag)
        self.autoErrorText.stringValue = @"Sectors overlap.";
    else
        self.autoErrorText.stringValue = @"";
}

- (NSRange) getSDSectorRange
{
    return NSMakeRange(self.sdCardStartSector.integerValue,
                       self.sectorCount.integerValue);
}

- (void) setAutoStartSectorValue: (NSUInteger)sector
{
    self.sdCardStartSector.stringValue = [NSString stringWithFormat: @"%d", (unsigned int)sector];
}

- (void) _recalcOnTextChange: (NSControl *)control
{
    if (control == self.sectorSize || control == self.sectorCount)
    {
        [self recalculate];
        [self evaluateSize];
    }
    else if (control == self.deviceSize)
    {
        NSInteger sc = [self convertUnitsToSectors];
        self.sectorCount.stringValue = [NSString stringWithFormat:@"%lld", (long long)sc];
    }
}

- (void)controlTextDidChange:(NSNotification *)notification
{
    NSTextField *textfield = [notification object];
    NSCharacterSet *charSet = [NSCharacterSet characterSetWithCharactersInString:@"0123456789"];

    char *stringResult = (char *)malloc([textfield.stringValue length]);
    int cpt=0;
    for (int i = 0; i < [textfield.stringValue length]; i++) {
        unichar c = [textfield.stringValue characterAtIndex:i];
        if ([charSet characterIsMember:c]) {
            stringResult[cpt]=c;
            cpt++;
        }
        else
        {
            NSBeep();
        }
    }
    stringResult[cpt]='\0';
    textfield.stringValue = [NSString stringWithUTF8String:stringResult];
    free(stringResult);
    
    [self _recalcOnTextChange: textfield];
}

- (void) recalculate
{
    [self evaluate];
}

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor
{
    [self _recalcOnTextChange: control];
    return YES;
}

- (BOOL) evaluate
{
    BOOL valid = YES;
    BOOL enabled = self.enableSCSITarget.state == NSOnState;

    /*
    self.SCSIID.enabled = enabled;
    self.deviceType.enabled = enabled;
    self.sdCardStartSector.enabled = enabled;
    self.autoStartSector.enabled = enabled;
    self.sectorSize.enabled = enabled;
    self.sectorCount.enabled = enabled;
    self.deviceSize.enabled = enabled;
    self.deviceUnit.enabled = enabled;
    self.vendor.enabled = enabled;
    self.productId.enabled = enabled;
    self.revsion.enabled = enabled;
    self.serialNumber.enabled = enabled;*/

    switch (self.deviceType.indexOfSelectedItem)
    {
        case CONFIG_FLOPPY_14MB:
            self.sectorSize.stringValue = @"512";
            self.sectorSize.enabled = NO;
            self.sectorCount.stringValue = @"2880";
            self.sectorCount.enabled = NO;
            self.deviceUnit.enabled = NO;
            self.deviceSize.enabled = NO;

            [self evaluateSize];
            break;
    };

    NSUInteger sectorSize = self.sectorSize.integerValue;
    if (sectorSize < 64 || sectorSize > 8192)
    {
        // Set error (TBD)
        valid = NO;
    }
    else
    {
        // clear error (TBD)
    }
    
    NSUInteger numSectors = self.sectorCount.integerValue;
    if (numSectors == 0)
    {
        // myNumSectorMsg->SetLabelMarkup(wxT("<span foreground='red' weight='bold'>Invalid size</span>"));
        valid = NO;
    }
    else
    {
        // myNumSectorMsg->SetLabelMarkup("");
    }
    
    // [self evaluateSize];

    return valid || !enabled;
}

/*
void
TargetPanel::onSizeInput(wxCommandEvent& event)
{
    if (event.GetId() != ID_numSectorCtrl)
    {
        std::pair<uint32_t, bool> sec = convertUnitsToSectors();
        if (sec.second)
        {
            std::stringstream ss;
            ss << sec.first;
            myNumSectorCtrl->ChangeValue(ss.str());
        }
    }
    if (event.GetId() != ID_sizeCtrl)
    {
        evaluateSize();
    }
    onInput(event); // propagate
} */

- (void) evaluateSize
{
    NSInteger numSectors = self.sectorCount.integerValue + 1;

    if (numSectors > 0)
    {
        NSInteger size = 0;
        NSInteger bytes = numSectors * self.sectorSize.integerValue;
        if (bytes >= 1024 * 1024 * 1024)
        {
            size = (bytes / (1024 * 1024 * 1024));
            NSMenuItem *item = [self.deviceUnit itemAtIndex:0]; // GB
            [self.deviceUnit selectItem:item];
        }
        else if (bytes >= 1024 * 1024)
        {
            size = (bytes / (1024 * 1024));
            NSMenuItem *item = [self.deviceUnit itemAtIndex:1]; // MB
            [self.deviceUnit selectItem:item];
        }
        else
        {
            size = (bytes / (1024));
            NSMenuItem *item = [self.deviceUnit itemAtIndex:2]; // KB
            [self.deviceUnit selectItem:item];
        }
        
        self.deviceSize.stringValue = [NSString stringWithFormat:@"%lld",(long long)size];
    }
}

- (NSUInteger) convertUnitsToSectors
{
    NSUInteger index = [self.deviceUnit indexOfSelectedItem];
    NSUInteger gb_size = 1024 * 1024 * 1024,
    mb_size = 1024 * 1024,
    kb_size = 1024;
    NSUInteger size = (NSUInteger)[[self.deviceSize stringValue] integerValue];
    NSUInteger sectorSize = (NSUInteger)[[self.sectorSize stringValue] integerValue];
    NSUInteger num_sectors = 0;
    NSUInteger size_factor = 0;
    
    switch (index)
    {
        case 0: // GB
            size_factor = gb_size;
            break;
        case 1: // MB
            size_factor = mb_size;
            break;
        case 2: // KB
            size_factor = kb_size;
            break;
        default:
            [NSException raise:NSInternalInconsistencyException format:@"Unexpected size selection"];
            break;
    }
    
    num_sectors = ((size * size_factor) / sectorSize) - 1;
    return num_sectors;
}

@end
