//
//  DataWriter.hpp
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/9/20.
//  Copyright © 2020 Rabbit Hole Computing LLC. All rights reserved.
//

#ifndef DataWriter_hpp
#define DataWriter_hpp

#import <Foundation/NSData.h>
#include <stdio.h>
#include "zipper.hh"

typedef uint64_t zsize_t;

class DataWriter : public zipper::Writer
{
private:
    NSMutableData *data;
    
public:
    virtual ~DataWriter();

    DataWriter();
    
    /// Returns the size of the written data.
    virtual uint64_t getSize() const;

    virtual void writeData(uint64_t offset, uint64_t bytes, const uint8_t* d);
    
    virtual NSData *getData();
};

#endif /* DataWriter_hpp */
