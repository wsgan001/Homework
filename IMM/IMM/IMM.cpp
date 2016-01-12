//
//  IMM.cpp
//  IMM
//
//  Created by rmk on 12/16/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include "IMM.hpp"
#include <cmath>
#include <random>
#include <iostream>
// combinatorics 组合数
double logcnk(int64_t n, int64_t k) {
    double result = 0;
    for (int64_t i = n - k + 1; i <= n; i++){
        result += log(i);
    }
    for (int64_t i = 1; i <= k; i++){
        result -= log(i);
    }
    return result;
}

IMM::IMM(){

}

vector<int64_t> IMM::Sampling(Graph& G, double k, double epsoid, double l){
    vector<int64_t> r;

    int64_t LB = 1;

    double epsoidpi = sqrt(2)*epsoid;

    for(int64_t i = 0; i <= log2(G.n)-1; i++){
        double x = G.n/pow((double)2, (double)i);
        double lambdapi = ((2+(double)2.0/3.0*epsoidpi)*(logcnk(G.n, k)+l*log(G.n)+log(log2(G.n))))/pow(epsoidpi, 2);
        double thetapi = lambdapi/x;
        srand((uint32_t)time(NULL));

        while (r.size() <= thetapi) {
            int64_t v = rand()%(G.n);
            r.push_back(v);
        }
        set<int64_t>  Si= NodeSelection(r, k, G);
        if (G.n*F(r,Si,G) >= (1+epsoidpi)*x) {
            LB = G.n*F(r,Si,G)/(1+epsoidpi);
            break;
        }
    }

    double alpha = sqrt(l*log(G.n)+log(2));// equ.(5)
    double beta = sqrt((1-(double)1/E)*logcnk(G.n, k)+l*log(G.n)+log(2));//(5)
    double lambdaaster = 2*G.n*pow(((1-(double)1/E)*alpha+beta),2)*pow(epsoid, -2);//(6)
    double theta = lambdaaster/LB;
    srand((uint32_t)time(NULL));
    while (r.size() <= theta) {
        int64_t v = rand()%(G.n);
        r.push_back(v);
    }
    return r;
}

set<int64_t> IMM::NodeSelection(vector<int64_t> r, double k, Graph& G){
    set<int64_t> Sk;
    if (r.size() <=0) {
        return Sk;
    }
    // 记录当前入度
    vector<int64_t> local_degree;
    vector<bool>    isvisit_local;
    isvisit_local.resize(G.n);
    local_degree.resize(G.n);
    for(auto rr:r){// RR set的内容
        for(auto rrnode:G.reversesets[rr]){
            local_degree[rrnode]++;
        }
    }
    for (int i = 0; i < k && i < r.size(); i++) {
        int64_t maxpoint = -1;
        double maxdegree = 0;
        for(int j =0; j < G.n; j++){
            if(local_degree[j] > maxdegree){
                maxpoint = j;
                maxdegree = local_degree[j];
            }
        }
        if(maxpoint != -1){
            Sk.insert(maxpoint);
            // 减少其他的以maxpoint及其前向点作为前向点的出度数
            for(auto outpoint: G.forwardsets[maxpoint]){
                for(auto outpoint_r:G.reversesets[outpoint])
                    local_degree[outpoint_r]--;
            }
        }else{
            cerr<<"Error NodeSelection" <<endl;
        }
    }
    return Sk;
}


double IMM::imm(Graph& G, double k, double epsoid, double l){
    l = l *(1+(log(2)/(log(G.n))));
    vector<int64_t> r = Sampling(G, k, epsoid, l);
    set<int64_t> Sk = NodeSelection(r, k, G);
    return F(r,Sk,G)*r.size();
}

double IMM::F(vector<int64_t> R, const set<int64_t>& Si,Graph& G){
    double matchedcount = 0;
    for(auto r:R){
        for(auto si:Si)
        if (G.reversesets[r].find(si)!= G.reversesets[r].end()) {
            matchedcount ++;
        }
    }
    return matchedcount/R.size();
}
