//
//  sort.hpp
//  sort
//
//  Created by rmk on 10/14/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#ifndef sort_hpp
#define sort_hpp
#include <vector>
#include <stdio.h>
using namespace std;
class Sort{
public :
    Sort();
    void insertionSort(vector<unsigned int>& data, int s, int e);
    void quickSort(vector<unsigned int>& data, int s, int e);
    void radixSort(vector<unsigned int>& data, int s, int e);
    void mergeSort(vector<unsigned int>& data, int s, int e);
    //生成范围在sRange-eRange之间的eIndex - sIndex +1个随机数,写入datasIndex和eIndex中
    void genRandom(vector<unsigned int>& data, unsigned int sRange,
                   unsigned int eRange, int sIndex, int eIndex);
private:
    void swap(unsigned int &a, unsigned int& b);
    unsigned int max(const unsigned int &a, const unsigned int &b);
    unsigned int pow2(int power);

};
#endif /* sort_hpp */
