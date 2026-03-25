#include "kv_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

kvs_hash_t global_hash={0};
// DJB2 哈希算法：运算极快且冲突率极低，适合 KV 存储
static unsigned int _hash_function(char *str, int slots) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % slots;
}

// 初始化哈希表
int kvs_hash_create(kvs_hash_t *inst) {
    if (!inst) return -1;

    // 申请桶数组内存
    inst->nodes = (kvs_hash_node_t **)kvs_malloc(sizeof(kvs_hash_node_t *) * kvs_hash_size);
    if (!inst->nodes) return -1;

    memset(inst->nodes, 0, sizeof(kvs_hash_node_t *) * kvs_hash_size);
    inst->max_slots = kvs_hash_size;
    inst->count = 0;
    return 0;
}

// 销毁哈希表（必须递归释放链表）
int kvs_hash_destory(kvs_hash_t *inst) {
    if (!inst || !inst->nodes) return -1;

    for (int i = 0; i < inst->max_slots; i++) {
        kvs_hash_node_t *node = inst->nodes[i];
        while (node) {
            kvs_hash_node_t *tmp = node;
            node = node->next;
            kvs_free(tmp->key);
            kvs_free(tmp->value);
            kvs_free(tmp);
        }
    }
    kvs_free(inst->nodes);
    return 0;
}

// SET 操作：采用“头插法”，复杂度 O(1)
//return : <0,error; =0,sucess; >0,exist;
int kvs_hash_set(kvs_hash_t *inst, char *key, char *value) {
    if (!inst || !key || !value) return -1;

    unsigned int idx = _hash_function(key, inst->max_slots);
    kvs_hash_node_t *node = inst->nodes[idx];

    // 1. 检查是否存在（防止重复插入）
    while (node) {
        if (strcmp(node->key, key) == 0) return 1; 
        node = node->next;
    }

    // 2. 创建新节点
    kvs_hash_node_t *new_node = (kvs_hash_node_t *)kvs_malloc(sizeof(kvs_hash_node_t));
    if (!new_node) return -2;

    new_node->key = kvs_malloc(strlen(key) + 1);
    strcpy(new_node->key, key);
    new_node->value = kvs_malloc(strlen(value) + 1);
    strcpy(new_node->value, value);

    // 3. 插入链表头部
    new_node->next = inst->nodes[idx];
    inst->nodes[idx] = new_node;
    inst->count++;

    return 0;
}

// GET 操作：平均 O(1)
//return : NULL,error; char*  sucess;
char* kvs_hash_get(kvs_hash_t *inst, char *key) {
    if (!inst || !key) return NULL;

    unsigned int idx = _hash_function(key, inst->max_slots);
    kvs_hash_node_t *node = inst->nodes[idx];

    while (node) {
        if (strcmp(node->key, key) == 0) return node->value;
        node = node->next;
    }
    return NULL;
}

// DEL 操作
//return : <0,error; =0,sucess; >0,no exist;
int kvs_hash_del(kvs_hash_t *inst, char *key) {
    if (!inst || !key) return -1;

    unsigned int idx = _hash_function(key, inst->max_slots);
    kvs_hash_node_t *node = inst->nodes[idx];
    kvs_hash_node_t *prev = NULL;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev) prev->next = node->next;
            else inst->nodes[idx] = node->next;

            kvs_free(node->key);
            kvs_free(node->value);
            kvs_free(node);
            inst->count--;
            return 0;
        }
        prev = node;
        node = node->next;
    }
    return 1;
}

//return : <0,error; =0,sucess; >0,no exist;
int kvs_hash_mod(kvs_hash_t *inst, char *key, char *value) {
	if (!inst || !key || !value) return -1;

	unsigned int idx = _hash_function(key, inst->max_slots);
	kvs_hash_node_t *node = inst->nodes[idx];

	while (node) {
		if (strcmp(node->key, key) == 0) {
			kvs_free(node->value);
			node->value = kvs_malloc(strlen(value) + 1);
			strcpy(node->value, value);
			return 0;
		}
		node = node->next;
	}
	return 1;
}
//return : <0,error; =0,sucess; >0,no exist;
int kvs_hash_exist(kvs_hash_t *inst, char *key) {
	if (!inst || !key) return -1;

	unsigned int idx = _hash_function(key, inst->max_slots);
	kvs_hash_node_t *node = inst->nodes[idx];

	while (node) {
		if (strcmp(node->key, key) == 0) return 0;
		node = node->next;
	}
	return 1;
}