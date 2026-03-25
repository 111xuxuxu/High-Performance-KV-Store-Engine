#ifndef _KV_STORE_H_
#define _KV_STORE_H_

#include <stddef.h>


#define NETWORK_REACTOR 0
#define NETWORK_IO_URING 1
#define NETWORK_NTYCO 2
#define NETWORK_SELECT  NETWORK_NTYCO
#define KVS_MAX_TOKENS 128
#define ENABLE_ARRAY 1
#define ENABLE_RBTREE 1
#define ENABLE_HASH 1
typedef int (*msg_handler)(char *msg,int length,char *response);

extern int reactor_start(unsigned short port,msg_handler handler);
extern int ntyco_start(unsigned short port,msg_handler handler);
extern int io_uring_start(unsigned short port,msg_handler handler);



#if ENABLE_ARRAY
typedef struct kvs_array_item_s{
    char *key;
    char *value;
}kvs_array_item_t;
#define kvs_array_size 1024
typedef struct kvs_array_s{
    kvs_array_item_t *table;
    int idx;
    int total;
}kvs_array_t;

int kvs_array_create(kvs_array_t *inst);
int kvs_array_destory(kvs_array_t *inst);
int kvs_array_set(kvs_array_t *inst,char *key,char *value);
char* kvs_array_get(kvs_array_t *inst,char *key);
int kvs_array_del(kvs_array_t *inst,char *key);
int kvs_array_mod(kvs_array_t *inst,char *key,char *value);
int kvs_array_exist(kvs_array_t *inst,char *key);

#endif

#if ENABLE_RBTREE

#define RED				1
#define BLACK 			2
#define ENABLE_KEY_CHAR 1
#if ENABLE_KEY_CHAR
typedef char* KEY_TYPE;
#else
typedef int KEY_TYPE;
#endif

typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;

	KEY_TYPE key;
	void *value;
} rbtree_node;

typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
} rbtree;


typedef struct _rbtree kvs_rbtree_t;

int kvs_rbtree_create(kvs_rbtree_t *inst);
int kvs_rbtree_destory(kvs_rbtree_t *inst);
int kvs_rbtree_set(kvs_rbtree_t *inst, char *key, char *value);
char* kvs_rbtree_get(kvs_rbtree_t *inst, char *key);
int kvs_rbtree_del(kvs_rbtree_t *inst, char *key);
int kvs_rbtree_mod(kvs_rbtree_t *inst,char *key,char *value);
int kvs_rbtree_exist(kvs_rbtree_t *inst,char *key);
#endif

void *kvs_malloc(size_t size);
void kvs_free(void *ptr);


#if ENABLE_HASH
// 1. 定义哈希表的最大桶数（建议设置大一些以减少冲突，如 102400）
#define kvs_hash_size 102400

// 2. 定义哈希表的单节点结构（链表节点）
typedef struct kvs_hash_node_s {
    char *key;                 // 动态申请的键
    char *value;               // 动态申请的值
    struct kvs_hash_node_s *next; // 指向下一个哈希冲突节点的指针
} kvs_hash_node_t;

// 3. 定义哈希表管理结构体
typedef struct kvs_hash_s {
    kvs_hash_node_t **nodes;   // 指向桶数组的二级指针（数组的每个元素都是一个链表头）
    int max_slots;             // 最大桶数
    int count;                 // 当前哈希表中存储的 KV 对总数
} kvs_hash_t;

int kvs_hash_create(kvs_hash_t *inst);
int kvs_hash_destory(kvs_hash_t *inst);
int kvs_hash_set(kvs_hash_t *inst, char *key, char *value);
char* kvs_hash_get(kvs_hash_t *inst, char *key);
int kvs_hash_del(kvs_hash_t *inst, char *key);
int kvs_hash_mod(kvs_hash_t *inst, char *key, char *value   );
int kvs_hash_exist(kvs_hash_t *inst, char *key);
#endif
#endif