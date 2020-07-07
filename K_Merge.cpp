//
// Created by alkaid on 2020/7/7.
//

//
// Created by alkaid on 2020/7/7.
//
#include "K_Merge.h"

using namespace std;


extern int *LoserTree,*External;
extern vector<Buffer_Node*> node_list;
extern int result[BUFFER_NODE_SIZE];

/*
 * function: check the merge done
 */
bool check(vector<Buffer_Node*> &nodelist){
    for(int i=0;i<nodelist.size();i++){
        if(nodelist[i]->inf.is_open()) return false;
    }
    return true;
}

/*
 * function: Read File when the cache empty
 */
void Read_File(void *args){
    int *i = (int*) args;
    node_list[*i]->read_file(0);
}

/*
 * function: Adjust the loser tree
 */
void Adjust(int s)
{
    int t=(s+DATA_NUM)/2;
    //cout<<t<<endl;
    int temp;
    while(t>0)
    {
        if(External[s] > External[LoserTree[t]])
        {
            temp = s;
            s = LoserTree[t];
            LoserTree[t]=temp;
        }
        t=t/2;
    }
    LoserTree[0]=s;
}

/*
 * function:Create a Loser Tree
 */
void CreateLoserTree()
{
    External[DATA_NUM]=INT_MIN;
    int i;
    for(i=0;i<DATA_NUM;i++)LoserTree[i]=DATA_NUM;
    for(i=DATA_NUM-1;i>=0;i--)Adjust(i);
}

/*
 * function:K_Merge code
 */
void K_Merge(threadpool_t* pool)
{
    ofstream outfile("result.txt");

    int travel_index = 0;
    int i,p;
    for(i=0;i<DATA_NUM;i++)
    {
        p = node_list[i]->travel_len;
        External[i]=node_list[i]->data[p];
        //cout<<External[i]<<endl;
        node_list[i]->travel_len++;
    }
    CreateLoserTree();
    while(!check(node_list))
    {
        p=LoserTree[0];
        result[travel_index++] = External[p];
        if(node_list[p]->travel_len>=node_list[p]->used_len){
            if(node_list[p]->inf.eof()){
                //cout<<"close the fstream"<<endl;
                External[p]=INT_MAX;//代表读完了
                node_list[p]->inf.close();
            }
            else{
                node_list[p]->used_len = 0;
                node_list[p]->travel_len = 1;
                //pthread_t *add_thread = (pthread_t *)malloc(sizeof(pthread_t));
                //pthread_create(add_thread,NULL,Read_File,&p);
                threadpool_add(pool,Read_File,&p);
                pthread_mutex_lock(&node_list[p]->qlock);
                while(node_list[p]->used_len == 0){
                    pthread_cond_wait(&node_list[p]->n,&node_list[p]->qlock);
                }
                External[p] = node_list[p]->data[0];
                pthread_mutex_unlock(&node_list[p]->qlock);
            }
        }
        else
        {
            External[p]=node_list[p]->data[node_list[p]->travel_len];
            node_list[p]->travel_len++;
        }
        Adjust(p);
        if(travel_index >= BUFFER_NODE_SIZE){
            for(int index=0;index<BUFFER_NODE_SIZE;index++) outfile<<result[index]<<" ";
            travel_index = 0;
        }
    }
    if(travel_index!=0){
        for(int index=0;index<travel_index;index++) outfile<<result[index]<<" ";
    }
    outfile.close();
}

/*
 * function:Test
 */
void Test_Merge(){
    int min;
    int min_index;
    while(!check(node_list)){
        min = INT_MAX;
        min_index = -1;
        for(int i=0;i<node_list.size();i++){

            if(!node_list[i]->inf.is_open()){
                continue;
            }
            if(node_list[i]->travel_len==node_list[i]->used_len){
                node_list[i]->used_len = 0;
                node_list[i]->travel_len = 0;
                node_list[i]->read_file(0);
            }
            if(node_list[i]->data[node_list[i]->travel_len]<min){
                min = node_list[i]->data[node_list[i]->travel_len];
                min_index = i;
            }
        }
        node_list[min_index]->travel_len++;
        if(node_list[min_index]->inf.eof()&&node_list[min_index]->travel_len==node_list[min_index]->used_len){
            node_list[min_index]->inf.close();
        }
    }
}
