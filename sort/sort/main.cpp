//
//  main.cpp
//  sort
//
//  Created by rmk on 10/15/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include "sort.h"

int main(int argc, char *argv[]){
    vector<unsigned int> v;
    const unsigned int sRange = 0;
    const unsigned int eRange = pow(2,32)-1;
    unsigned int pbset[]={
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        200000000,
        //1000000000,
    };

    Sort sort;
    clock_t st;
    clock_t et;
    for(int i = 0; i < sizeof(pbset)/sizeof(pbset[0]); i++){
        v.resize(pbset[i]);
        sort.genRandom(v, sRange, eRange, 0, pbset[i]-1);
        st = clock()*1000/CLOCKS_PER_SEC;
        sort.radixSort(v, 0, pbset[i]-1);
        et = clock()*1000/CLOCKS_PER_SEC;
        printf("Radixsort:\tpbsize:%d, time in ms:%ld\n", pbset[i], et-st);

    }
    for(int i = 0; i < sizeof(pbset)/sizeof(pbset[0]); i++){
        v.resize(pbset[i]);
        sort.genRandom(v, sRange, eRange, 0, pbset[i]-1);
        st = clock()*1000/CLOCKS_PER_SEC;
        sort.quickSort(v, 0, pbset[i]-1);
        et = clock()*1000/CLOCKS_PER_SEC;
        printf("QuickSort:\tpbsize:%d, time in ms:%ld\n", pbset[i], et-st);
    }

    for(int i = 0; i < sizeof(pbset)/sizeof(pbset[0]); i++){
        v.resize(pbset[i]);
        sort.genRandom(v, sRange, eRange, 0, pbset[i]-1);
        st = clock()*1000/CLOCKS_PER_SEC;
        sort.mergeSort(v, 0, pbset[i]-1);
        et = clock()*1000/CLOCKS_PER_SEC;
        printf("MergeSort:\tpbsize:%d, time in ms:%ld\n", pbset[i], et-st);
        
    }

    for(int i = 0; i < sizeof(pbset)/sizeof(pbset[0]); i++){
        v.resize(pbset[i]);
        sort.genRandom(v, sRange, eRange, 0, pbset[i]-1);
        st = clock()*1000/CLOCKS_PER_SEC;
        sort.insertionSort(v, 0, pbset[i]-1);
        et = clock()*1000/CLOCKS_PER_SEC;
        printf("InsertionSort:\tpbsize:%d, time in ms:%ld\n", pbset[i], et-st);
    }
    return 0;
}