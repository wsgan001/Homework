//
//  Bitmap.cpp
//  bitmap
//
//  Created by rmk on 11/17/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include "Bitmap.h"
#include <memory.h>
char Bitmap::map[BITNUM];

Bitmap::Bitmap(uint32_t size){
    
    this->size = size;

    if(this->size % 8 == 0){
        this->charsize = this->size/8;
    }else{
        this->charsize = this->size/8+1;
    }
    memset(this->map, 0, this->charsize);
}

/*
 *position是自然数，从0开始
 *0:unset, 1:set, -1 error
 */
int Bitmap::get(uint32_t position){

    if (position > this->size) {
        return -1;
    }

    // 在char map[]的下标位置
    uint32_t char_pos = (position)/8;
    // 在char中的位置
    uint32_t bit_pos = (position) % 8;

    if((this->map[char_pos] & 1 << (bit_pos) ) > 0){
        return 1;
    }else{
        return 0;
    }
}
/*
 true: 设置成功
 false：设置失败
 */
bool Bitmap::set(uint32_t position){

    if (position > size) {
        return false;
    }
    
    // 在char map[]的下标位置
    uint32_t char_pos = (position)/8;
    // 在char中的位置
    uint32_t bit_pos = (position) % 8;

    this->map[char_pos] |= ( 1 << (bit_pos) );

    return true;
}
/*
 true: 设置成功
 false：设置失败
 */
bool Bitmap::unset(uint32_t position){

    if (position > size) {
        return false;
    }
    // 在char map[]的下标位置
    uint32_t char_pos = (position)/8;
    // 在char中的位置
    uint32_t bit_pos = (position) % 8;
    this->map[char_pos] &= ( 0xFF -  ((char)1 << (bit_pos)));
    return true;
}
uint32_t Bitmap::firstMissing(){
    for(int i=0; i < this->charsize; i++){
        if (this->map[i] != (char)0xFF) {
            for(int j=0; j < 8; j++){
                if (
                    (this->map[i] & char( 1 << j) ) == 0
                    ) {
                    
                    return i*8+j;
                }
            }
        }
    }
    return -1;
}
