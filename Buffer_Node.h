//
// Created by alkaid on 2020/7/6.
//
#ifndef PINGCAP_HOMEWORK_BUFFER_NODE_H
#define PINGCAP_HOMEWORK_BUFFER_NODE_H
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <sys/types.h>
#include <string>

#define BUFFER_NODE_SIZE 1024
using namespace std;
class Buffer_Node{
public:
    int* data;
    uint32_t len;
    uint32_t used_len; //used len
    uint32_t travel_len;//Kmerge index
    string filename;
    ifstream inf; //ios::instream
    pthread_mutex_t qlock;
    pthread_cond_t n;
    Buffer_Node(uint32_t len_,string name);
    void read_file(int flag);

};
#endif //PINGCAP_HOMEWORK_BUFFER_NODE_H
