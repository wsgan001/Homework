//
//  IMM.hpp
//  IMM
//
//  Created by rmk on 12/16/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#ifndef IMM_hpp
#define IMM_hpp

#include <stdio.h>
#include <stdint.h>
#include <list>
#include <map>
#include <set>
#include "Graph.hpp"

#define E 2.71828

double logcnk(int64_t n, int64_t k);


class IMM{
public:
    IMM();
    set<int64_t> NodeSelection(vector<int64_t> rrsets, double k, Graph& G);
    vector<int64_t> Sampling(Graph& G, double k, double epsoid, double l);
    // IMM算法主流程,Algorithm3
    double imm(Graph& G, double k, double epsoid, double l);
    // 计算Fr(Si)
    double F(vector<int64_t> R, const set<int64_t>& Si,Graph& G);
};
#endif /* IMM_hpp */
