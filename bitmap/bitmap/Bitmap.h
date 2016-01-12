//
//  Bitmap.hpp
//  bitmap
//
//  Created by rmk on 11/17/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#ifndef Bitmap_h
#define Bitmap_h

#include <stdio.h>
#include <stdint.h>
#endif /* Bitmap_h */

#define BITNUM (UINT32_MAX/8+1)
class Bitmap {
    static char map[BITNUM];
    uint32_t size;
    uint32_t charsize;
public:
    Bitmap(uint32_t size);
    int get(uint32_t position);
    bool set(uint32_t position);
    bool unset(uint32_t position);
    uint32_t firstMissing();
};