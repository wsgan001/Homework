//
//  DGIM.cpp
//  DGIM
//
//  Created by rmk on 12/12/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include "DGIM.hpp"



bool operator == (const Bucket& first, const Bucket& other){
    return first.timestamp == other.timestamp && first.count == other.count;
}

uint32_t pow2(uint32_t n){
    uint32_t a = 1;
    return a << n;
}

Bucket::Bucket(){
    this->timestamp = 0;
    this->count = 0;
}

Bucket::Bucket(uint32_t timestamp, uint32_t count){
    this->timestamp = timestamp;
    this->count = count;
}


DGIM::DGIM(){
}

void DGIM::next(uint32_t data){

    // 首先，拆成m位位流来进行处理
    for (int i = 0; i < m; i++) {
        for(list<Bucket>::iterator itr = this->buckets[i].begin(); itr !=this->buckets[i].end(); itr++){
            (*itr).timestamp++;
        }
        while(buckets[i].back().timestamp >= N){
            buckets[i].pop_back();
        }
        if( (1 << i & data) != 0){
            // the i-th bit is 1
            Bucket newBucket(0,1);
            buckets[i].push_front(newBucket);
            this->shuffleBucket(buckets[i],buckets[i].begin());
        }else{
            // the i-th bit is 0
            // do nothing
        }
    }
}
// 处理加入新桶后面的分裂问题
void DGIM::shuffleBucket(list<Bucket>& l, list<Bucket>::iterator itr){
    Bucket first  = *itr;
    Bucket second = *(++itr);
    Bucket& third =  *(++itr);
    if (first.count == second.count && first.count == third.count) {
        l.remove(second);
        third.count+= second.count;
        third.timestamp = second.timestamp;
        this->shuffleBucket(l, itr);
    }
}

const int64_t DGIM::calSum(uint32_t topK){
    uint64_t sum = 0;
    for(int i = 0; i < m; i++){
        uint32_t a = 0;
        list<Bucket>::iterator itr;
        Bucket last;
        for(itr = this->buckets[i].begin(); itr!= buckets[i].end() && (*itr).timestamp < topK; itr++){
            a += (*itr).count;
            last = *itr;
        }
        if(last.timestamp < topK){
            a -= last.count/2;
        }
        sum += a*pow2(i);
    }
    return sum;
}


