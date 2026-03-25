#include "kv_store.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
kvs_array_t global_array={0};
int kvs_array_create(kvs_array_t *inst){
    if(!inst){
        return -1;
    }
    if(inst->table){
        printf("table has alloc\n");
        return -1;
    }
    inst->table=kvs_malloc(kvs_array_size*sizeof(kvs_array_item_t));
    if(!inst->table){
        return -1;
    }
 
    inst->total=0;

    return 0;
}
int kvs_array_destory(kvs_array_t *inst){
    if(!inst){
        return -1;
    }
    if(inst->table){
        kvs_free(inst->table);
    }
    inst->total=0;
    return 0;
}
//return : <0,error; =0,sucess; >0,exist;
int kvs_array_set(kvs_array_t *inst,char *key,char *value){
    if(inst==NULL||key==NULL||value==NULL) return -1;
    if(inst->total==kvs_array_size) return -1;

    char *ptr=kvs_array_get(inst,key);
    if(ptr){
        return 1;
    }

     // 2. 申请内存
    char *kcopy = kvs_malloc(strlen(key) + 1);
    if (kcopy == NULL) return -2;
    
    char *vcopy = kvs_malloc(strlen(value) + 1);
    if (vcopy == NULL) {
        kvs_free(kcopy); // 防止内存泄漏
        return -2;
    }

    // 3. 拷贝数据
    strcpy(kcopy, key);
    strcpy(vcopy, value);

    int i=0;
    for(i;i<inst->total;i++){
        if(inst->table[i].key==NULL){
            inst->table[i].key=kcopy;
            inst->table[i].value=vcopy;
            inst->total++;
            return 0;
        }
    }

    if(i==inst->total&&i<kvs_array_size){
        inst->table[i].key=kcopy;
        inst->table[i].value=vcopy;
        inst->total++;
       
    }
     return 0;
}
//return : NULL,error; char*  sucess;
char* kvs_array_get(kvs_array_t *inst,char *key){
    if(inst==NULL){
        return NULL;
    }
    int i=0;
    for(i;i<inst->total;i++){
        if(inst->table[i].key==NULL){
            continue;
        }
        if(strcmp(inst->table[i].key,key)==0){
            return inst->table[i].value;
        }
    }
    return NULL;

}
//return : <0,error; =0,sucess; >0,no exist;
int kvs_array_del(kvs_array_t *inst, char *key) {
    if (inst == NULL || key == NULL) return -1;
    for (int i = 0; i < inst->total; i++) {
        if (inst->table[i].key && strcmp(inst->table[i].key, key) == 0) {
            // 1. 释放当前内存
            kvs_free(inst->table[i].key);
            kvs_free(inst->table[i].value);

            // 2. 将最后一个元素移动到当前位置，填补空洞
            inst->table[i] = inst->table[inst->total - 1];

            // 3. 清空原最后一个元素的位置并减少总数
            inst->table[inst->total - 1].key = NULL;
            inst->table[inst->total - 1].value = NULL;
            inst->total--; 

            return 0;
        }
    }
    return 1; // 不存在
}
//return : <0,error; =0,sucess; >0,no exist;
int kvs_array_mod(kvs_array_t *inst,char *key,char *value){
    if(inst==NULL||key==NULL||value==NULL) return -1;
    int i=0;
    for(i;i<inst->total;i++){
        if(inst->table[i].key==NULL){
            continue;
        }
        if(strcmp(inst->table[i].key,key)==0){
            kvs_free(inst->table[i].value);
         
            
            char *kvalue=kvs_malloc(strlen(value)+1);
            if(kvalue==NULL){
                return -2;
            }
            memset(kvalue,0,strlen(value)+1);
            strncpy(kvalue,value,strlen(value)+1);
            inst->table[i].value=kvalue;
            return 0;
        }
    }
    return i;
}
//return : <0,error; =0,exist; >0,no exist;
int kvs_array_exist(kvs_array_t *inst,char *key){
    if(inst==NULL||key==NULL){
        return -1;
    }
    char *ptr=kvs_array_get(inst,key);
    if(!ptr){
        return 1;
    }
    return 0;
    
}