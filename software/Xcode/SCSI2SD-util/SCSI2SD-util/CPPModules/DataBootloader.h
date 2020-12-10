//
//  DataBootloader.h
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/9/20.
//  Copyright © 2020 Rabbit Hole Computing LLC. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "SCSI2SD_Bootloader.hh"

NS_ASSUME_NONNULL_BEGIN

class DataBootloader : SCSI2SD::Bootloader
{
    void load(NSData* data, void (*progress)(uint8_t, uint16_t));
};

NS_ASSUME_NONNULL_END
