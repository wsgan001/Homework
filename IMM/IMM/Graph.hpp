//
//  Graph.hpp
//  IMM
//
//  Created by rmk on 12/16/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <stdint.h>
#include <string>
#include <vector>
#include <set>
using namespace std;


enum Model{IC,LT,CTIC};
//链表法代表的一个图
class Graph{
public:

    // m: 边数， n:点数
    int64_t m;
    int64_t n;

    // 程序需要，sampled_graph 存的是反向边
    vector<vector<int64_t>> sampled_graph;
    // reversesets[i]存的是所有能到达的i的点
    vector<set<int64_t>> reversesets;
    // forwardsets[i]存的是所有从i出发能到达的的点
    vector<set<int64_t>> forwardsets;
    Graph();
    // 读取并生成影响子图
    void read(string folderPath, enum Model model, double thresold);
};

#endif /* Graph_hpp */
