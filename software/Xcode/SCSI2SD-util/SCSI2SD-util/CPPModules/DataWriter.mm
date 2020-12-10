//
//  DataWriter.cpp
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/9/20.
//  Copyright © 2020 Rabbit Hole Computing LLC. All rights reserved.
//


#include "DataWriter.h"

DataWriter::~DataWriter()
{
    data = nil;
}

DataWriter::DataWriter()
{
    data = [[NSMutableData alloc] init];
}

/// Returns the size of the written data.
uint64_t DataWriter::getSize() const
{
    if (data == nil)
    {
        return 0;
    }
    return [data length];
}

void DataWriter::writeData(uint64_t offset, uint64_t bytes, const uint8_t* d)
{
    NSLog(@"%llu %llu", offset, bytes);
    [data appendBytes: (const void *)d
               length: bytes];
}

NSData *DataWriter::getData()
{
    return data;
}
