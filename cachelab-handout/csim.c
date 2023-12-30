#include "cachelab.h"
#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"
#include "string.h"
typedef struct{
    unsigned char valid_bit;
    int tag_bit;
    int time;
}cache_line,*cache;
cache _cache=NULL;
typedef struct{
    int s;
    int E;
    int b;
    int m;
}cache_struct;
cache_struct _cache_struct;
int hit_count,miss_count,eviction_count;
char file_name[100];
void Initcache(){
    int S=1<<_cache_struct.s;
    _cache=(cache)malloc(sizeof(cache_line)*S*_cache_struct.E);
    for(int i=0;i<S*_cache_struct.E;i++){
        _cache[i].valid_bit=0;
        _cache[i].tag_bit=-1;
        _cache[i].time=-1;
    }
}
void update(int addr){
    int S_bit=((1<<_cache_struct.s)-1);
    int addr_S=(addr>>_cache_struct.b)&S_bit;
    int addr_T=addr>>(_cache_struct.b+_cache_struct.s);
    for(int i=addr_S*_cache_struct.E;i<(addr_S+1)*_cache_struct.E;i++){
        if(addr_T==_cache[i].tag_bit){
            _cache[i].time=0;
            hit_count++;
            return;
        }
    }
    for(int i=addr_S*_cache_struct.E;i<(addr_S+1)*_cache_struct.E;i++){
        if(_cache[i].valid_bit==0){
            _cache[i].valid_bit=1;
            _cache[i].tag_bit=addr_T;
            _cache[i].time=0;
            miss_count++;
            return;
        }
    }
    miss_count++;
    eviction_count++;
    int max_time=-1;
    int record_i;
    for(int i=addr_S*_cache_struct.E;i<(addr_S+1)*_cache_struct.E;i++){
        if(_cache[i].time>max_time){
            max_time=_cache[i].time;
            record_i=i;
        }
    }
    _cache[record_i].tag_bit=addr_T;
    _cache[record_i].time=0;
    return;
}
void update_time(){
    int S=1<<_cache_struct.s;
    for(int i=0;i<S*_cache_struct.E;i++)
        _cache[i].time++;
}
int main(int argc,char*argv[])
{
    hit_count=0;miss_count=0;eviction_count=0;
    int ch;
    while((ch=getopt(argc,argv,"s:E:b:t:"))!=EOF){
        switch (ch)
        {
        case 's':
            _cache_struct.s=atoi(optarg);
            break;
        case 'E':_cache_struct.b=atoi(optarg);
            _cache_struct.E=atoi(optarg);
            break;
        case 'b':
            _cache_struct.b=atoi(optarg);
            break;
        case 't':
            strcpy(file_name,optarg);
            break;
        default:
            break;
        }
    }
    Initcache();
    FILE *fp;
    if((fp=fopen(file_name,"r"))==NULL){
        printf("cannot open the file");
        exit(-1);
    }
    char op;
    unsigned int addr;
    int size;
    while(!feof(fp)){
        fscanf(fp," %c %x,%d\n",&op,&addr,&size);
        switch (op)
        {
        case 'L':
            update(addr);
            break;
        case 'M':
            update(addr);
            update(addr);
            break;
        case 'S':
            update(addr);
            break;
        }
        update_time();
    }
    fclose(fp);
    free(_cache);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
