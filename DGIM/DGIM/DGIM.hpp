//
//  DGIM.hpp
//  DGIM
//
//  Created by rmk on 12/12/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#ifndef DGIM_hpp
#define DGIM_hpp

#include <stdio.h>
#include <list>
#include <vector>
#include <bitset>
#include <cstdint>


// 定义一些题目要求的量
#define scale (1)
#define N (100000000/scale)

// 1-2^8的范围需要9个位
#define m (9)
#define k (50000000/scale)

using namespace std;



class Bucket{
public:
    uint32_t timestamp;
    uint32_t count;
    Bucket();
    Bucket(uint32_t timestamp, uint32_t count);
};

bool operator == (const Bucket& first, const Bucket& other);
uint32_t pow2(uint32_t n);


class DGIM{
private:
    // 桶队列
    list<Bucket> buckets[m];

public:
    DGIM();
    // 加入一个新的数据
    void next(uint32_t data);
    /*
     
     */
    void shuffleBucket(list<Bucket>& l, list<Bucket>::iterator itr);
    const int64_t calSum(uint32_t topK);
};



#endif /* DGIM_hpp */
