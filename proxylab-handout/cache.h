#include "csapp.h"

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_BLOCK_NUM 10

typedef struct{
    char request[MAXLINE];
    char info[MAX_OBJECT_SIZE];
    int size;
    int time;
} cache_block;
typedef struct{
    int num;
    cache_block * cache_blocks[MAX_BLOCK_NUM];
} Cache;
sem_t mutex, w;

static int cur_time;
static int readcnt;

Cache cache;

void init_cache(){
    cur_time = 0;
    readcnt = 0;
    cache.num = 0;
    Sem_init(&mutex, 0, 1);
    Sem_init(&w, 0, 1);
    for(int i=0;i<MAX_BLOCK_NUM;++i){
        cache.cache_blocks[i] = (cache_block*)malloc(sizeof(cache_block));
        cache.cache_blocks[i]->time = 0;
        cache.cache_blocks[i]->size = 0;
    }
}

void free_cache(){
    for(int i=0;i<MAX_BLOCK_NUM;++i){
        free(cache.cache_blocks[i]);
    }
}

int reader(int clientfd, char * request){
    int flag = 0;
    P(&mutex);
    readcnt++;
    if(readcnt == 1){
        P(&w);
    }
    V(&mutex);
    for(int i=0;i<cache.num;++i){
        if(cache.cache_blocks[i]->time && !strcmp(cache.cache_blocks[i]->request, request)){
            flag = 1;
            Rio_writen(clientfd, cache.cache_blocks[i]->info, cache.cache_blocks[i]->size);
            break;
        }
    }
    P(&mutex);
    readcnt--;
    if(readcnt == 0){
        V(&w);
    }
    V(&mutex);
    return flag;
}

void writer(char * request, char * info, int size){
    P(&w);
    if(cache.num == MAX_BLOCK_NUM){
        int mintime = ~(1 << 31);
        int index = MAX_BLOCK_NUM;
        for(int i=0;i<MAX_BLOCK_NUM;++i){
            if(cache.cache_blocks[i]->time < mintime){
                mintime = cache.cache_blocks[i]->time;
                index = i;
            }
        }
        strcpy(cache.cache_blocks[index]->request, request);
        memcpy(cache.cache_blocks[index]->info, info, size);
        cache.cache_blocks[index]->size = size;
        cache.cache_blocks[index]->time = cur_time++;
    }
    else{
        strcpy(cache.cache_blocks[cache.num]->request, request);
        memcpy(cache.cache_blocks[cache.num]->info, info, size);
        cache.cache_blocks[cache.num]->size = size;
        cache.cache_blocks[cache.num++]->time = cur_time++;
    }
    V(&w);
}