//
//  main.cpp
//  IMM
//
//  Created by rmk on 12/16/15.
//  Copyright © 2015 rmk. All rights reserved.
//

#include <iostream>
#include "Graph.hpp"
#include "IMM.hpp"
#include <stdlib.h>
clock_t nulltime;
// 测量时间距离nulltime的毫秒数
clock_t elapse(){
    return clock()*1000/CLOCKS_PER_SEC - nulltime;
}
void resetclock(){
    nulltime = clock()*1000/CLOCKS_PER_SEC;
}


int main(int argc, const char * argv[]) {

    Graph g;
    double k = 50;
    double epsoid = 0.5;
    double l = 1.0;
    IMM imm;
    string datafolder = "/Users/rmk/Documents/workspace/IMM/IMM/nethept";


    // 其他的都不要动，就改这下面的三种read就可以调用模型，你们写这个main函数的交互吧
    //==========================================================================
    //IC 模型
    //    g.read("/Users/rmk/Documents/workspace/IMM/IMM/nethept", Model::IC, 0);
    //    double EI = imm.imm(g,k, epsoid, l);
    //    cout << EI;
    //LT模型
    //g.read("/Users/rmk/Documents/workspace/IMM/IMM/nethept", Model::LT, 0);

    //CTIC 模型
    //g.read(datafolder, Model::CTIC, 10);
    //double EI = imm.imm(g,k, epsoid, l);
    //cout << EI <<endl;

    //==========================================================================





    double kset[] = {1,10,20,30,40,50};
    double testround = 3;//重复测试次数
    //==============================实验一======================================
    //IC模型，固定epsoid = 0.5, l=1, k取1，10，20，30，40，50.测量运行时间和最后的影响因子
    epsoid = 0.5;
    l = 1.0;
    for(auto kvalue:kset){
        double EIsum = 0;
        double timeSum = 0;
        printf("=======================k=%.0lf==================================\n",kvalue);
        for(int i = 0; i < testround; i++){
            sleep(1);//暂停一秒
            printf("第%d轮：测试数据集:nethept, 抽样模型：independent cascade(IC)\n",i+1);
            printf("参数:epsilon=%.2lf, l=%.2lf, k=%.0lf:\n", epsoid, l,kvalue);
            resetclock();
            g.read(datafolder, Model::IC, 0);
            double EI = imm.imm(g,kvalue, epsoid, l);
            EIsum += EI;
            clock_t t = elapse();
            timeSum += t;
            printf("影响因子：%.2lf, 测试用时:%ldms\n\n", EI, t);
        }

        printf("%1.0lf轮测试平均影响因子%1.0lf,平均测试用时:%.2lfms\n\n",testround, EIsum/testround, timeSum/testround);
    }
    //==============================实验二======================================
    //LT模型，固定epsoid = 0.5, l=1, k取1，10，20，30，40，50.测量运行时间和最后的影响因子
    epsoid = 0.5;
    l = 1.0;
    for(auto kvalue:kset){
        double EIsum = 0;
        double timeSum = 0;
        printf("=======================k=%.0lf==================================\n",kvalue);
        for(int i = 0; i < testround; i++){
            sleep(1);//暂停一秒
            printf("第%d轮：测试数据集:nethept, 抽样模型：(LT)\n",i+1);
            printf("参数:epsilon=%.2lf, l=%.2lf, k=%.0lf:\n", epsoid, l,kvalue);
            resetclock();
            g.read(datafolder, Model::LT, 0);
            double EI = imm.imm(g,kvalue, epsoid, l);
            EIsum += EI;
            clock_t t = elapse();
            timeSum += t;
            printf("影响因子：%.2lf, 测试用时:%ldms\n\n", EI, t);
        }

        printf("%1.0lf轮测试平均影响因子%1.0lf,平均测试用时:%.2lfms\n\n",testround, EIsum/testround, timeSum/testround);
    }
    //==============================实验三======================================
    //CTIC模型，固定epsoid = 0.5, l=1, k取1，2，4，6测量运行时间和最后的影响因子
    double cont_kset[]={1,2,4,6};
    double thresold = 10;
    epsoid = 0.5;
    l = 1.0;
    for(auto kvalue:cont_kset){
        double EIsum = 0;
        double timeSum = 0;
        printf("=======================k=%.0lf==================================\n",kvalue);
        for(int i = 0; i < testround; i++){
            sleep(1);//暂停一秒
            printf("第%d轮：测试数据集:nethept, 抽样模型：(CTIC)\n",i+1);
            printf("参数:epsilon=%.2lf, l=%.2lf, k=%.0lf:,t=%.0lf:\n", epsoid, l,kvalue,thresold);
            resetclock();
            g.read(datafolder, Model::CTIC, thresold);
            double EI = imm.imm(g,kvalue, epsoid, l);
            EIsum += EI;
            clock_t t = elapse();
            timeSum += t;
            printf("影响因子：%.2lf, 测试用时:%ldms\n\n", EI, t);
        }

        printf("%1.0lf轮测试平均影响因子%1.0lf,平均测试用时:%.2lfms\n\n",testround, EIsum/testround, timeSum/testround);
    }
}
