#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <cstring>
#include "common.h"
using namespace std;
typedef unsigned char uchar;
const int Collisionlen = 4;

int SM3Calc_RhoAttack()
{
    unsigned char m[Collisionlen + 1] = {};
    generateString(m, Collisionlen);

    uchar h1[SM3_HASH_SIZE];
    uchar h2[SM3_HASH_SIZE];
    SM3Calc(m, Collisionlen, h1, ivStd, true);
    SM3Calc(h1, Collisionlen, h2, ivStd, true);

    int circleSize = 0;
    while(true)
    {
        SM3Calc(h1, Collisionlen, h1, ivStd, true);
        // 对h2进行两次哈希计算，结果存储在h2中
        SM3Calc(h2,Collisionlen,h2, ivStd, true);
        SM3Calc(h2,Collisionlen,h2, ivStd, true);
        if(!memcmp(h1,h2,Collisionlen))
        {
            cout<<"find"<<Collisionlen * 8<<"bit collision"<<endl;
            cout<<"hash1:";
            print_Hexvalue(h1,SM3_HASH_SIZE);
            cout<<"hash2:";
            print_Hexvalue(h2,SM3_HASH_SIZE);
            printf("circle size:%d\n", circleSize);
            return 0;
        }
        circleSize++;
    }
    return 1;
}

int main()
{
    clock_t start,end;//定义clock_t变量
    start = clock();  //开始时间
    SM3Calc_RhoAttack();
    end = clock();   //结束时间
    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
    return 0;
}
