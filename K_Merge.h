#include "Buffer_Node.h"
#include "thread_pool.h"
#include <vector>
#include <limits.h>
#include <iostream>
#define DATA_NUM 4

extern int *LoserTree,*External;
extern vector<Buffer_Node*> node_list;
extern int result[BUFFER_NODE_SIZE];

bool check(vector<Buffer_Node*> &nodelist);
void Read_File(void *args);
void Adjust(int s);
void CreateLoserTree();
void K_Merge(threadpool_t* pool);
void Test_Merge();