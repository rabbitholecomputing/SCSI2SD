//
//  Functions.hpp
//  SCSI2SD-util
//
//  Created by Gregory Casamento on 12/9/19.
//  Copyright © 2019 RabbitHole Computing, LLC. All rights reserved.
//

#ifndef Functions_hpp
#define Functions_hpp

#include <stdio.h>
#include "scsi2sd.h"

#if __cplusplus >= 201103L
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <utility>
// #include "scsi2sd.h"

static uint8_t sdCrc7(uint8_t* chr, uint8_t cnt, uint8_t crc);

#endif /* Functions_hpp */
