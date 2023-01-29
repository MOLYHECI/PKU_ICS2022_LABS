// Zimo Meng 2100017702
#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int hit_count, miss_count, eviction_count;
int isvisible = 0;
long s,S,E,b,B;
char t[1000];
FILE * fp;
typedef struct{
    long vavid;
    long tag;
    long last_visit_time;
}Line,*Set,**Cache;
Cache cache;
void __init__(){
    cache = (Cache)malloc(sizeof(Set) * S);
    for(int i=0;i<S;++i){
        cache[i] = (Set)malloc(sizeof(Line) * E);
        for(int j=0;j<E;++j){
            cache[i][j].vavid = 0;
            cache[i][j].tag = -1;
            cache[i][j].last_visit_time = -1;
        }
    }
}
void cache_visit(char op,long address, long size, long time){
    long x = 1;
    long set_index = (address >> b) & ((x << s) - 1);
    long cur_tag = (address >> (s + b)) & ((x << (64-(s+b))) - 1);
    for(int i=0;i<E;++i){
        if(cache[set_index][i].vavid == 1 && cache[set_index][i].tag == cur_tag){
            ++hit_count;
            cache[set_index][i].last_visit_time = time;
            if(isvisible){
                printf("%c %lx,%ld hit\n",op,address,size);
            }
            return;
        }
    }
    ++miss_count;
    for(int i=0;i<E;++i){
        if(cache[set_index][i].vavid == 0){
            cache[set_index][i].vavid = 1;
            cache[set_index][i].tag = cur_tag;
            cache[set_index][i].last_visit_time = time;
            if(isvisible){
                printf("%c %lx,%ld miss\n",op,address,size);
            }
            return;
        }
    }
    ++eviction_count;
    long LRU_index;
    long LRU_time = time;
    for(int i=0;i<E;++i){
        if(cache[set_index][i].last_visit_time < LRU_time){
            LRU_time = cache[set_index][i].last_visit_time;
            LRU_index = i;
        }
    }
    cache[set_index][LRU_index].tag = cur_tag;
    cache[set_index][LRU_index].last_visit_time = time;
    if(isvisible){
        printf("%c %lx,%ld\n",op,address,size);
    }
}
void Read(){
    char op;
    long address;
    long size;
    long time = 0;
    while(fscanf(fp," %c %lx,%ld\n",&op,&address,&size)!=-1){
        switch(op){
            case 'I':
                continue;
            case 'L':
                cache_visit(op,address,size,time);
                break;
            case 'S':
                cache_visit(op,address,size,time);
                break;
            case 'M':
                cache_visit(op,address,size,time);
                cache_visit(op,address,size,time);
                break;
        }
        ++time;
    }
    for(int i=0;i<S;++i){
        free(cache[i]);
    }
    free(cache);
}
void printusage(){
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
            "Options:\n"
            "  -h         Print this help message.\n"
            "  -v         Optional verbose flag.\n"
            "  -s <num>   Number of set index bits.\n"
            "  -E <num>   Number of lines per set.\n"
            "  -b <num>   Number of block offset bits.\n"
            "  -t <file>  Trace file.\n\n"
            "Examples:\n"
            "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
            "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
int main(int argc,char *argv[]){
    int opt;
    hit_count = miss_count = eviction_count = 0;
    while((opt = getopt(argc,argv,"hvs:E:b:t:"))!=-1){
        switch(opt){
            case 'h':
                printusage();
                break;
            case 'v':
                isvisible = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(t,optarg);
                break;
            default:
                return -1;
        }
    }
    if(s<=0||E<=0||b<=0||t==NULL) return -1;
    S = 1 << s, B = 1 << b;
    __init__();
    fp = fopen(t,"r");
    if(fp == NULL) return -1;
    Read();
    fclose(fp);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}