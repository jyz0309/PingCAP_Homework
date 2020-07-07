//
// Created by alkaid on 2020/7/7.
//
#include "Buffer_Node.h"

Buffer_Node::Buffer_Node(uint32_t len_,string name):
len(len_),used_len(0),travel_len(0),filename(name){
    data = new int[len];
    inf.open(filename,ios::in);
    pthread_mutex_init(&qlock,NULL);
    pthread_cond_init(&n, NULL);
    if(!data){
        fprintf(stderr,"The space not enough to allocate");
        exit(1);
    }
}

void Buffer_Node::read_file(int flag){
    pthread_mutex_lock(&qlock);
    int num;
    while(inf>>num){

        this->data[this->used_len++] = num;
        if(flag == 0){
            //flag=0代表这是因为缓冲区已被读取完导致的文件读取
            pthread_cond_signal(&n);
            flag = 1;
        }
        if(this->used_len>=BUFFER_NODE_SIZE){
            //cout<<"the Buffer full, wait for customer"<<endl;
            break;
        }
        //pthread_mutex_unlock(&qlock);
    }

    pthread_mutex_unlock(&qlock);
}