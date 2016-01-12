//
//  main.cpp
//  DGIM
//
//  Created by rmk on 12/12/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include <iostream>
#include <random>
#include <functional>
#include <string>
#include <time.h>

#include "DGIM.hpp"

// 设置开始输出的位置和输出间隔
#define STARTINDEX (N)
#define INTERVAL (1000000/scale)

using namespace std;

clock_t elapse(clock_t nulltime){
    return clock()*1000/CLOCKS_PER_SEC - nulltime;
}

int main(int argc, const char * argv[]) {
    DGIM dgim;

    uint64_t counter = 0;
    uint64_t start = STARTINDEX;
    clock_t st;
    uniform_int_distribution<unsigned int> distribution(1, pow2(8));

    mt19937 engine;

    // 跳过前面STARTINDEX位
    printf("初始化输入流,当前时钟归零:\n");
    st = clock()*1000/CLOCKS_PER_SEC;
    srand((uint32_t)time(NULL));

    while (start--) {
        dgim.next(distribution(engine));
        counter ++;
        if(counter % INTERVAL == 0){
            printf("初始化(忽略前%d位)：当前输入为第%llu位(%.2f%%),初始化耗时%lds\n",STARTINDEX, counter,(float)counter*100/STARTINDEX, elapse(st)/1000);
        }
    }
    printf("初始化完成，每%d位输出前%d位结果\n",INTERVAL,k);
    st = clock()*1000/CLOCKS_PER_SEC;

    while (true) {
        dgim.next(distribution(engine));
        counter ++;
        if(counter % INTERVAL == 0){
            printf("当前处理第%llu位,前%d位和约为%llu，距上一次输出耗时%lds,\n",counter, k, dgim.calSum(k),elapse(st)/1000);
            if(counter % (10*INTERVAL) == 0){
                printf("是否继续(y/n):");
                char s;
                cin >> s;
                st = clock()*1000/CLOCKS_PER_SEC;
                if(s == 'n'){
                    break;
                }
            }
        }
    }
}
