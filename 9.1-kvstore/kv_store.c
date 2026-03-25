#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kv_store.h"
#include <assert.h>

#if ENABLE_ARRAY
extern kvs_array_t global_array;
#endif

#if ENABLE_RBTREE
extern kvs_rbtree_t global_rbtree;
#endif

#if ENABLE_HASH
extern kvs_hash_t global_hash;
#endif
void *kvs_malloc(size_t size){
    return malloc(size);
}
void kvs_free(void *ptr){
    return free(ptr);
}
const char *command[]={

    "SET","GET","DEL","MOD","EXIST",
    "RSET","RGET","RDEL","RMOD","REXIST",
    "HSET","HGET","HDEL","HMOD","HEXIST",
};
enum{
    KVS_CMD_START=0,
    //array
#if ENABLE_ARRAY
    KVS_CMD_SET=KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_EXIST,
#endif
    //rbtree
#if ENABLE_RBTREE
    KVS_CMD_RSET,
    KVS_CMD_RGET,
    KVS_CMD_RDEL,
    KVS_CMD_RMOD,
    KVS_CMD_REXIST,
#endif
    //hash
#if ENABLE_HASH
    KVS_CMD_HSET,
    KVS_CMD_HGET,
    KVS_CMD_HDEL,
    KVS_CMD_HMOD,
    KVS_CMD_HEXIST,
    KVS_CMD_COUNT,
#endif
};
const char *response[]={

};
/*
    msg:request message
    length: request message length
    response: need to send
    @return: length of response
*/
int kvs_split_token(char *msg,char *tokens[]){
    if(msg==NULL||tokens==NULL){
        return -1;
    }
    int idx=0;
    char *token=strtok(msg," \r\n");
    while(token!=NULL){
        tokens[idx++]=token;
        token=strtok(NULL," \r\n");
    }
    return idx;
}
int kvs_protocol_filter(char **tokens,int count,char *response){
    if(tokens==NULL||count==0||response==NULL){
        return -1;
    }
    int cmd=KVS_CMD_START;
    for(cmd;cmd<KVS_CMD_COUNT;cmd++){
        if(strcmp(tokens[0],command[cmd])==0){
            break;
        }
    }
    int length=0;
    int ret=0;
    switch(cmd){
#if ENABLE_ARRAY
        //array
        case KVS_CMD_SET:
            ret=kvs_array_set(&global_array,tokens[1],tokens[2]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"EXIST\r\n");
            }
            break;
        case KVS_CMD_GET:
            char *value=kvs_array_get(&global_array,tokens[1]);
            if(value==NULL){
                length=sprintf(response,"NO EXIST\r\n");
            }else{
                length=sprintf(response,"%s\r\n",value);
            }
            
            break;
        case KVS_CMD_DEL:
            ret=kvs_array_del(&global_array,tokens[1]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            
            break;
        case KVS_CMD_MOD:
            ret=kvs_array_mod(&global_array,tokens[1],tokens[2]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
        case KVS_CMD_EXIST:
            ret=kvs_array_exist(&global_array,tokens[1]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"EXIST\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
#endif
#if ENABLE_RBTREE

        //rbtree
        case KVS_CMD_RSET:
            ret=kvs_rbtree_set(&global_rbtree,tokens[1],tokens[2]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"EXIST\r\n");
            }
            break;
        case KVS_CMD_RGET:
            char *rvalue=kvs_rbtree_get(&global_rbtree,tokens[1]);
            if(rvalue==NULL){
                length=sprintf(response,"NO EXIST\r\n");
            }else{
                length=sprintf(response,"%s\r\n",rvalue);
            }
            break;
        case KVS_CMD_RDEL:
            ret=kvs_rbtree_del(&global_rbtree,tokens[1]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
        case KVS_CMD_RMOD:
            ret=kvs_rbtree_mod(&global_rbtree,tokens[1],tokens[2]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
        case KVS_CMD_REXIST:
            ret=kvs_rbtree_exist(&global_rbtree,tokens[1]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"EXIST\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
#endif
#if ENABLE_HASH
        //hash
        case KVS_CMD_HSET:
            ret=kvs_hash_set(&global_hash,tokens[1],tokens[2]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");   
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"EXIST\r\n");
            }
            break;
        case KVS_CMD_HGET:
            char *hvalue=kvs_hash_get(&global_hash,tokens[1]);
            if(hvalue==NULL){
                length=sprintf(response,"NO EXIST\r\n");
            }else{
                length=sprintf(response,"%s\r\n",hvalue);
            }
            break;
        case KVS_CMD_HDEL:
            ret=kvs_hash_del(&global_hash,tokens[1]);       
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
        case KVS_CMD_HMOD:
            ret=kvs_hash_mod(&global_hash,tokens[1],tokens[2]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"SUCCESS\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
        case KVS_CMD_HEXIST:
            ret=kvs_hash_exist(&global_hash,tokens[1]);
            if(ret<0){
                length=sprintf(response,"ERROR\r\n");
            }else if(ret==0){
                length=sprintf(response,"EXIST\r\n");
            }else{
                length=sprintf(response,"NO EXIST\r\n");
            }
            break;
#endif
        default:
    printf("[Unknown Command] tokens[0]: %s\n", tokens[0]); // 打印出那个导致崩溃的命令名
    // assert(0); // 先注释掉，防止崩溃
    break;
    }
    return length;
    
}
int kvs_protocol(char *msg,int length,char *response){
    if(msg==NULL||length<=0||response==NULL){
        return -1;
    }
     //printf("recv %d: %s\n",length,msg);
     //memcpy(response,msg,length);
     char *tokens[KVS_MAX_TOKENS]={0};
     int count=kvs_split_token(msg,tokens);
     if(count==-1){
        return -1;
     }
     return kvs_protocol_filter(tokens,count,response);
    }

int init_kvengine(void){
#if ENABLE_ARRAY
    memset(&global_array,0,sizeof(kvs_array_t));
    kvs_array_create(&global_array);

#endif
#if ENABLE_RBTREE
    memset(&global_rbtree,0,sizeof(kvs_rbtree_t));
    kvs_rbtree_create(&global_rbtree);
#endif
#if ENABLE_HASH
    memset(&global_hash,0,sizeof(kvs_hash_t));
    kvs_hash_create(&global_hash);
#endif

    return 0;
}

int destory_kvengine(void){
#if ENABLE_ARRAY
    kvs_array_destory(&global_array);
#endif
#if ENABLE_RBTREE
    kvs_rbtree_destory(&global_rbtree);
#endif
#if ENABLE_HASH
    kvs_hash_destory(&global_hash);
#endif
}

int main(int argc,char *argv[]){
    if(argc!=2) return -1;
    int port=atoi(argv[1]);

    init_kvengine();
#if(NETWORK_SELECT==NETWORK_REACTOR)
    reactor_start(port,kvs_protocol);
#elif(NETWORK_SELECT==NETWORK_NTYCO)
    ntyco_start(port,kvs_protocol);
#elif(NETWORK_SELECT==NETWORK_IO_URING)
    io_uring_start(port,kvs_protocol);
#endif
    destory_kvengine();
}
