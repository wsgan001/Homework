//
//  main.cpp
//  bitmap
//
//  Created by rmk on 11/17/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include "Bitmap.h"

using namespace std;
#define HUNDRED_MILLION 100000000

int main(int argc, const char * argv[]) {
    Bitmap bitmap(HUNDRED_MILLION);
    long st, et;

    st = clock()*1000/CLOCKS_PER_SEC;


    uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);
    mt19937 engine;
    for(uint32_t i=0; i < HUNDRED_MILLION; i++)
    {
        bitmap.set(distribution(engine));
    }
//    //Start testing
//    for(int i=0; i < 100000; i++){
//        bitmap.set(i);
//    }
//
//    for(int j=100001; j < HUNDRED_MILLION; j++){
//        bitmap.set(j);
//    }
//    //end Testing

    et = clock()*1000/CLOCKS_PER_SEC;
    cout<<"The First Missing number is: "<<bitmap.firstMissing()<<endl;


    cout << "Find first missing number in "<< HUNDRED_MILLION<<" numbers using "<< et-st<<" ms."<<endl;
    return 0;
}
