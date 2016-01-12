//
//  Graph.cpp
//  IMM
//
//  Created by rmk on 12/16/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include "Graph.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <cstdlib>
#include <list>
#include <random>
#include "IMM.hpp"


Graph::Graph(){

}

// 适用于数据集2，不知道其他的怎样
void Graph::read(string folderPath,enum Model model, double thresold){
    FILE* fd;
    int64_t first;
    int64_t second;
    string line;
    double p;

    // 为了方便重复使用类，将所有变量在初始化时均删除
    sampled_graph.clear();
    reversesets.clear();
    forwardsets.clear();
    m = n = 0;
    if(model == Model::IC){
    // IC模型
        fd = fopen((folderPath+"/attribute.txt").c_str(),"r");
        // 读取第一行的n和m
        fscanf(fd,"%lld %lld\n",&n,&m);
        fclose(fd);
        this->sampled_graph.resize(n);
        fd = fopen((folderPath+"/graph_ic.inf").c_str(), "r");
        srand((uint32_t)time(NULL));
        while (fscanf(fd, "%lld %lld %lf",&first, &second, &p)>0) {
            if(rand()%100 <= p*100){
                sampled_graph[second].push_back(first);
            }
        }
        // generate RRSet for each point
        this->reversesets.resize(n);
        this->forwardsets.resize(n);

        for (int i = 0; i< sampled_graph.size(); i++) {
            vector<bool> isvisit;
            isvisit.resize(n);
            list<int64_t> queue;
            queue.push_back(i);
            while (!queue.empty()) {
                int64_t cur = queue.front();
                queue.pop_front();
                isvisit[cur] = true;
                reversesets[i].insert(cur);
                forwardsets[cur].insert(i);
                for(int j = 0; j < sampled_graph[cur].size(); j++){
                    if(!isvisit[sampled_graph[cur][j]]){
                        queue.push_back(sampled_graph[cur][j]);
                    }
                }
            }
        }
    }else if(model == Model::LT){
        fd = fopen((folderPath+"/attribute.txt").c_str(),"r");
        vector<vector<double>> prob;
        // 读取第一行的n和m
        fscanf(fd,"%lld %lld\n",&n,&m);
        fclose(fd);
        this->sampled_graph.resize(n);
        prob.resize(n);
        fd = fopen((folderPath+"/graph_lt.inf").c_str(), "r");
        srand((uint32_t)time(NULL));
        while (fscanf(fd, "%lld %lld %lf",&first, &second, &p)>0) {
            sampled_graph[second].push_back(first);
            prob[second].push_back(p);
        }
        for(int i=0; i < n; i++){
            srand((uint32_t)time(0));
            int64_t u = rand()%100;
            int64_t j;
            for(j = 0; u >0 && j < prob[i].size(); j++){
                u -= (prob[i][j]*100);
            }
            if(u > 0){
                // empty set
                sampled_graph[i].clear();
            }else{
                int64_t node =sampled_graph[i][j-1];
                sampled_graph[i].clear();
                sampled_graph[i].push_back(node);
            }
        }

        // generate RRSet for each point
        this->reversesets.resize(n);
        this->forwardsets.resize(n);

        for (int i = 0; i< sampled_graph.size(); i++) {
            vector<bool> isvisit;
            isvisit.resize(n);
            list<int64_t> queue;
            queue.push_back(i);
            while (!queue.empty()) {
                int64_t cur = queue.front();
                queue.pop_front();
                isvisit[cur] = true;
                reversesets[i].insert(cur);
                forwardsets[cur].insert(i);
                for(int j = 0; j < sampled_graph[cur].size(); j++){
                    if(!isvisit[sampled_graph[cur][j]]){
                        queue.push_back(sampled_graph[cur][j]);
                    }
                }
            }
        }
    }else if(model == Model::CTIC){
        // CTIC model
        double scale;
        double shape;
        vector<vector<double>> prob;
        fd = fopen((folderPath+"/attribute.txt").c_str(),"r");
        // 读取第一行的n和m
        fscanf(fd,"%lld %lld\n",&n,&m);
        fclose(fd);
        this->sampled_graph.resize(n);
        prob.resize(n);
        fd = fopen((folderPath+"/graph_cont.inf").c_str(), "r");
        std::default_random_engine generator;

        while (fscanf(fd, "%lld %lld %lf %lf",&first, &second, &shape, &scale)>0) {
            std::weibull_distribution<double> distribution(shape,scale);
            p = distribution(generator);
            sampled_graph[second].push_back(first);
            prob[second].push_back(p);
        }

        // generate RRSet for each point
        this->reversesets.resize(n);
        this->forwardsets.resize(n);

        for (int i = 0; i< sampled_graph.size(); i++) {
            vector<bool> isvisit;
            vector<double> cost;
            
            isvisit.resize(n);
            cost.resize(n);

            list<int64_t> queue;
            queue.push_back(i);
            while (!queue.empty()) {
                int64_t cur = queue.front();
                queue.pop_front();
                if(cost[cur] > thresold){
                    continue;
                }
                isvisit[cur] = true;
                reversesets[i].insert(cur);
                forwardsets[cur].insert(i);
                for(int j = 0; j < sampled_graph[cur].size(); j++){
                    if(!isvisit[sampled_graph[cur][j]]){
                        queue.push_back(sampled_graph[cur][j]);
                        cost[sampled_graph[cur][j]] += cost[cur];
                        cost[sampled_graph[cur][j]] += prob[cur][j];
                    }
                }
            }
        }
    }
}