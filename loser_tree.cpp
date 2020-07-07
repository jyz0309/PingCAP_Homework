#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>
using namespace std;
void test(){
    srand(time(NULL));//先种种子
    vector<int> result;
    clock_t start,end;
    for(int i=0;i<4;i++){
        int index = 0;
        for(int j=0;j<2500000;j++){
            index = index+rand()%10;
            result.push_back(index);
        }
    }
    start = clock();
    sort(result.begin(),result.end());
    end = clock();
    cout<<"runtime:"<<(double)(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
}
int main(void)
{
    test();
    return 0;
    srand(time(NULL));//先种种子
    int i,j;
    FILE *fp = NULL;
    fp = fopen("data4.txt","a");//在指定目录下创建.txt文件
    int index = 0;
    for(i = 0; i < 2500000; i++) //产生10个100以内的随机数
    {
        j = rand()%10;
        index = index+j;
        fprintf(fp,"%d ",index);
    }
    return 0;
}