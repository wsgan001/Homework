//
//  sort.cpp
//  sort
//
//  Created by rmk on 10/14/15.
//  Copyright © 2015 rmk. All rights reserved.
//
#include <math.h>
#include <random>
#include <functional>
#include "sort.h"
Sort::Sort(){


}
void Sort::insertionSort(vector<unsigned int>& data, int s, int e){
    for(int i = s; i <=e; i++){
        int tindex = i+1;
        unsigned int tdata = data[tindex];
        while(tindex > s && tdata < data[tindex-1] ){
            data[tindex] = data[tindex-1];
            tindex -=1;
        }
        data[tindex] = tdata;
    }

}
void Sort::quickSort(vector<unsigned int>& data, int s, int e){
    int pivot = s;
    int lastless = s;
    if(e <= s){
        return;
    }
    for ( int i = s+1; i <= e; i++) {
        if(data[i] < data[pivot]){
            this->swap(data[++lastless], data[i]);
        }
    }
    this->swap(data[pivot], data[lastless]);
    this->quickSort(data, s, lastless-1);
    this->quickSort(data, lastless+1, e);
}
void Sort::radixSort(vector<unsigned int>& data, int s, int e){
    const int maxDigitsNum = 10; //最多10位十进制数
    const int base = 10;
    bool shouldStop = false;
    for (int i = 0; i < maxDigitsNum && !shouldStop; i++)
    {
        vector<vector<unsigned int> > bucket(base);
        shouldStop = true;
        for(int j = s; j <= e; j++)
        {
            unsigned int temp = data[j]/pow(base,i);
            if(temp > 0 ){
                temp %= base;
                shouldStop = false;//这个位数还是有数字的
            }
            bucket[temp].push_back(data[j]);
        }
        for (int k = s, x = 0; x < base; x++)
        {
            for (int y = 0; y < bucket[x].size(); y++)
            {
                data[k++] = bucket[x][y];
            }
        }
    }

}
void Sort::mergeSort(vector<unsigned int>& data, int s, int e){
    if (e <= s) {
        return;
    }
    int m = (s+e)/2;
    this->mergeSort(data, s, m);
    this->mergeSort(data, m+1, e);
    vector<unsigned int> L;
    for (int i=s; i <=m; i++) {
        L.push_back(data[i]);
    }

    vector<unsigned int> R;
    for (int i=m+1; i <=e; i++) {
        R.push_back(data[i]);
    }
    int i=0, j = 0, k = s;
    while (true) {
        if (L[i] < R[j]) {
            data[k++] = L[i++];
            if (i >= L.size()) {
                //L结束
                for(int tj = j; tj < R.size(); tj++){
                    data[k++] = R[tj];
                }
                break;
            }
        }else{
            data[k++] = R[j++];
            if (j >= R.size()) {
                //R结束
                for(int ti = i; ti < R.size(); ti++){
                    data[k++] = L[ti];
                }
                break;
            }
        }
    }
    L.clear();
    R.clear();

}

void Sort::swap(unsigned int &a, unsigned int& b){
    unsigned int tmp;
    tmp = a;
    a = b;
    b = tmp;
}

unsigned int Sort::max(const unsigned int &a, const unsigned int &b){
    return a>b? a:b;
}

void Sort::genRandom(vector<unsigned int>& data, unsigned int sRange,
                     unsigned int eRange, int sIndex, int eIndex){
    uniform_int_distribution<unsigned int> distribution(sRange, eRange);
    mt19937 engine;
    int num = eIndex - sIndex +1;
    for(int i=0;i<num;i++)
    {
        data[i]=distribution(engine);
    }

}

unsigned int Sort::pow2(int power){
    return 1 >> power;
}
