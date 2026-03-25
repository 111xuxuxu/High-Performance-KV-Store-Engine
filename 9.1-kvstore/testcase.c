#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "kv_store.h"
#include <sys/time.h>
#define MAX_MSG_LENGTH 1024
#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

int send_msg(int connd,char *msg,int length){
    int res=send(connd,msg,length,0);
    if(res<0){
        perror("send error");
        return -1;
    }
    return res;
}
int recv_msg(int connd,char *buffer,int size){
    int res=recv(connd,buffer,size,0);
    if(res<0){
        perror("recv error");
        return -1;
    }
    return res;
}
void testcase(int connd,char *msg,char *pattern,char *casename){
    if(!msg||!pattern||!casename){
        //printf("invalid testcase %s\n",casename);
        return;
    }
    send_msg(connd,msg,strlen(msg));
    char result[MAX_MSG_LENGTH]={0};
    recv_msg(connd,result,MAX_MSG_LENGTH);
    if(strcmp(result,pattern)==0){
        //printf("testcase %s success\n",casename);
    }else{
        //printf("testcase %s failed,expect %s but get %s\n",casename,pattern,result);
    }
}

int connect_tcpserver(const char* ip,unsigned short port){
    int connd=socket(AF_INET,SOCK_STREAM,0);
    if(connd<0){
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip);
    if(connect(connd,(struct sockaddr*)&server_addr,sizeof(server_addr))!=0){
        close(connd);
        perror("connect error");
        return -1;
    }
    return connd;
}
void array_testcase_10W(int connd){

    int count=10000;
    struct timeval start,end;
    gettimeofday(&start,NULL);
    while(count){
            //printf("--- 开始 KV Store 逻辑测试 ---\n");

            // 1. 测试 SET 功能 (预期：SUCCESS\r\n)
            testcase(connd, "SET teacher king\r\n", "SUCCESS\r\n", "SET-NEW-KEY");

            // 2. 测试 GET 功能 (预期：king\r\n)
            testcase(connd, "GET teacher\r\n", "king\r\n", "GET-EXISTS-KEY");

            // 3. 测试 SET 重复 Key (预期：EXIST\n) 
            testcase(connd, "SET teacher sparking\r\n", "EXIST\r\n", "SET-DUPLICATE-KEY");

            // 4. 测试 MOD 修改功能 (预期：SUCCESS\r\n)
            testcase(connd, "MOD teacher queen\r\n", "SUCCESS\r\n", "MOD-EXISTS-KEY");

            // 5. 再次 GET 确认修改成功 (预期：queen\r\n)
            testcase(connd, "GET teacher\r\n", "queen\r\n", "GET-AFTER-MOD");

            // 6. 测试 EXIST 功能 (预期：EXIST\r\n)
            testcase(connd, "EXIST teacher\r\n", "EXIST\r\n", "EXIST-QUERY-YES");

            // 7. 测试 DEL 功能 (预期：SUCCESS\r\n)
            testcase(connd, "DEL teacher\r\n", "SUCCESS\r\n", "DEL-EXISTS-KEY");

            // 8. 删除后再次 EXIST (预期：NO EXIST\n)
            testcase(connd, "EXIST teacher\r\n", "NO EXIST\r\n", "EXIST-QUERY-NO");

            // 9. 测试获取不存在的 Key (预期：NO EXIST\r\n)
            // 注意：GET 命令失败返回的是 "NO EXIST\r\n"
            testcase(connd, "GET teacher\r\n", "NO EXIST\r\n", "GET-AFTER-DEL");

            // 10. 测试协议健壮性：多空格处理
            //testcase(connd, "SET    student     tom\r\n", "SUCCESS\r\n", "SET-WITH-SPACES");

            //printf("--- 测试结束 ---\n");
            count--;
    }
    gettimeofday(&end,NULL);
    int time_used=TIME_SUB_MS(end,start);
    printf("ARRAYTotal time for 100,000 operations: %d ms, qps:%d\n", time_used, 90000*1000/time_used);

}
void rbtree_testcase_10W(int connd){
    int count=100000;
    struct timeval start,end;
    gettimeofday(&start,NULL);
    while(count){
            //printf("--- 开始 KV Store 逻辑测试 ---\n");

            // 1. 测试 SET 功能 (预期：SUCCESS\r\n)
            testcase(connd, "RSET teacher king\r\n", "SUCCESS\r\n", "RSET-NEW-KEY");

            // 2. 测试 GET 功能 (预期：king\r\n)
            testcase(connd, "RGET teacher\r\n", "king\r\n", "RGET-EXISTS-KEY");

            // 3. 测试 SET 重复 Key (预期：EXIST\n) 
            testcase(connd, "RSET teacher sparking\r\n", "EXIST\r\n", "RSET-DUPLICATE-KEY");

            // 4. 测试 MOD 修改功能 (预期：SUCCESS\r\n)
            testcase(connd, "RMOD teacher queen\r\n", "SUCCESS\r\n", "RMOD-EXISTS-KEY");

            // 5. 再次 GET 确认修改成功 (预期：queen\r\n)
            testcase(connd, "RGET teacher\r\n", "queen\r\n", "RGET-AFTER-MOD");

            // 6. 测试 EXIST 功能 (预期：EXIST\r\n)
            testcase(connd, "REXIST teacher\r\n", "EXIST\r\n", "REXIST-QUERY-YES");

            // 7. 测试 DEL 功能 (预期：SUCCESS\r\n)
            testcase(connd, "RDEL teacher\r\n", "SUCCESS\r\n", "RDEL-EXISTS-KEY");

            // 8. 删除后再次 EXIST (预期：NO EXIST\n)
            testcase(connd, "REXIST teacher\r\n", "NO EXIST\r\n", "REXIST-QUERY-NO");

            // 9. 测试获取不存在的 Key (预期：NO EXIST\r\n)
            // 注意：GET 命令失败返回的是 "NO EXIST\r\n"
            testcase(connd, "RGET teacher\r\n", "NO EXIST\r\n", "RGET-AFTER-DEL");
             // 10. 测试协议健壮性：多空格处理
            //testcase(connd, "SET    student     tom\r\n", "SUCCESS\r\n", "SET-WITH-SPACES");

            //printf("--- 测试结束 ---\n");
            count--;
        
    }
    gettimeofday(&end,NULL);
    int time_used=TIME_SUB_MS(end,start);
    printf("RBTREETotal time for 100,000 operations: %d ms, qps:%d\n", time_used, 900000*1000/time_used);
}
void rbtree_testcase_1W_0(int connd){
    int count=10000;
    struct timeval start,end;
    gettimeofday(&start,NULL);

    for(int i=0;i<count;i++){
        char cmd[128]={0};
        sprintf(cmd,"RSET teacher%d king%d\r\n",i,i);
        testcase(connd, cmd, "SUCCESS\r\n", "RSET-NEW-KEY");
        
    }
    for(int i=0;i<count;i++){
        char cmd[128]={0};
        sprintf(cmd,"RGET teacher%d\r\n",i);
        testcase(connd, cmd, "king%d\r\n", "RGET-EXISTS-KEY");
    }
    for(int i=0;i<count;i++){
        char cmd[128]={0};
        sprintf(cmd,"RMOD teacher%d queen%d\r\n",i,i);
        testcase(connd, cmd, "SUCCESS\r\n", "RMOD-EXISTS-KEY");
    }
    for(int i=0;i<count;i++){
        char cmd[128]={0};
        sprintf(cmd,"RDEL teacher%d\r\n",i);
        testcase(connd, cmd, "SUCCESS\r\n", "RDEL-EXISTS-KEY");
    }
     

    gettimeofday(&end,NULL);
    int time_used=TIME_SUB_MS(end,start);
    printf("RBTREETotal time for 100,000 operations: %d ms, qps:%d\n", time_used, count*4*1000/time_used);
}
void hash_testcase_100W(int connd) {
    int count = 1000000; // 100万次
    char **cmds = malloc(count * sizeof(char*));
    struct timeval start, end;
    char buf[1024];

    printf("--- 准备阶段：预生成 100 万条 HSET 指令 ---\n");
    for (int i = 0; i < count; i++) {
        cmds[i] = malloc(64);
        // 预先拼好指令，避开计时区域内的 sprintf 损耗
        snprintf(cmds[i], 64, "HSET user%d val%d\r\n", i, i);
    }

    printf("--- 执行阶段：开始 100 万次 HSET 压测 ---\n");
    gettimeofday(&start, NULL);

    for (int i = 0; i < count; i++) {
        // 1. 发送预生成的指令
        send(connd, cmds[i], strlen(cmds[i]), 0);
        
        // 2. 接收响应（静默处理，不打印结果）
        int n = recv(connd, buf, sizeof(buf), 0);
        if (n <= 0) {
            printf("连接中断在第 %d 次操作\n", i);
            break;
        }

        // 每 10 万次打个点，让你知道程序没死
        if (i % 100000 == 0) {
            printf("进度: %d / %d ...\n", i, count);
        }
    }

    gettimeofday(&end, NULL);

    // 计算总耗时 (ms)
    long time_used = TIME_SUB_MS(end, start);
    
    printf("\n========================================\n");
    printf("测试结果 (Hash Table):\n");
    printf("总操作数: %d 次\n", count);
    printf("总耗时: %ld ms\n", time_used);
    // QPS 计算：次数 / (毫秒 / 1000)
    printf("平均 QPS: %.2f\n", (double)count * 1000.0 / time_used);
    printf("========================================\n");

    // 清理内存
    for (int i = 0; i < count; i++) free(cmds[i]);
    free(cmds);
}
//testcase 192.168.213.134 2000

int main(int argc,char *argv[]){
    if(argc!=4){
        printf("arg error\n");
        return -1;
    }
    char *ip=argv[1];
    unsigned short port=atoi(argv[2]);
    int mode=atoi(argv[3]);
    int connd=connect_tcpserver(ip,port);
    if(mode==0){
        array_testcase_10W(connd);
    }else if(mode==1){
        //rbtree_testcase_10W(connd);
        rbtree_testcase_1W_0(connd);
    }else if(mode==2){
        hash_testcase_100W(connd);
    }else{
        printf("mode error\n");
    }
    close(connd);
    return 0;
}

