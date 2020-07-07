#include "Buffer_Node.h"
#include "thread_pool.h"
#include "K_Merge.h"
#include<ctime>

using namespace std;


int *LoserTree,*External;
vector<Buffer_Node*> node_list;
int result[BUFFER_NODE_SIZE];

int main (){
    threadpool_t* pool = threadpool_create(4, 65535);
    LoserTree=(int *)malloc(sizeof(int)*DATA_NUM);
    External=(int *)malloc(sizeof(int)*(DATA_NUM+1));

    clock_t start_time = clock();
    for(int i=1;i<=DATA_NUM;i++){
        //init Buffer Node List
        Buffer_Node* cur = new Buffer_Node(BUFFER_NODE_SIZE,"data"+to_string(i)+".txt");
        node_list.push_back(cur);
        node_list[i-1]->read_file(1);
    }
    //Customer();
    K_Merge(pool);
    clock_t endTime = clock();//计时结束
    cout<<"runtime:"<<(double)(endTime-start_time)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
