#include <stdio.h>
#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define SUBFSIZE 16
#define NTHREADS 4

/* You won't lose style points for including this long line in your code */

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *conn_hdr="Connection: close\r\n";
static const char *proxy_conn_hdr="Proxy-Connection: close\r\n";
typedef struct{
    int *buf;
    int n;
    int front;
    int rear;
    sem_t mutex;
    sem_t slots;
    sem_t items;
}sbuf_t;
sbuf_t sbuf;
void doit(int fd);
void parse(char*uri,char*hostname,char*port,char*path);
void reqheader(rio_t *rp,char*req,char*method,char*hostname,char*port,char*path);
void sbuf_init(sbuf_t *sp,int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp,int item);
int sbuf_remove(sbuf_t *sp);
void *thread(void *vargp);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <path>\n", argv[0]);
        exit(1);
    }
    listenfd = Open_listenfd(argv[1]);
    sbuf_init(&sbuf,SUBFSIZE);
    for(int i=0;i<NTHREADS;i++){
        Pthread_create(&tid,NULL,thread,NULL);
    }
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); 
        sbuf_insert(&sbuf,connfd);
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from (%s,%s)\n", hostname, port);                         
    }
    return 0;
}

void doit(int fd) 
{
    int server_fd;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE],  port[MAXLINE],path[MAXLINE],req[MAXLINE];
    rio_t client_rio,server_rio;

    Rio_readinitb(&client_rio, fd);
    Rio_readlineb(&client_rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);      

    parse(uri,hostname,port,path);    
    reqheader(&client_rio,req,method,hostname,port,path);      
    server_fd=Open_clientfd(hostname,port);              
    Rio_readinitb(&server_rio, server_fd);
    Rio_writen(server_fd,req,strlen(req));
    
    size_t n;
    while((n=Rio_readlineb(&server_rio,buf,MAXLINE))!=0){
        printf("proxy received %d bytes,then send\n",(int)n);
        Rio_writen(fd,buf,n);
    }
    Close(server_fd);
}
void parse(char*uri,char*hostname,char*port,char*path){
    char *_host=strstr(uri,"//");
    if(_host==NULL){
        char *_path=strstr(uri,"/");
        if(_path!=NULL){
            strcpy(path,_path);
            strcpy(path,"80");
            return;
        }
        else _host=uri;
    }
    else _host=_host+2;
    char*_port=strstr(_host,":");
    if(_port!=NULL){
        int port_num;
        char*_path=strstr(_port,"/");
        if(_path!=NULL){
            sscanf(_port+1,"%d%s",&port_num,path);
        }
        else{
            sscanf(_port+1,"%d",&port_num);
        }
        sprintf(port,"%d",port_num);
        *_port='\0';
    }
    else{
        char*_path=strstr(_port,"/");
        if(_path!=NULL){
           strcpy(path,_path);
           *_path='\0';
        }
        strcpy(port,"80");
    }
    strcpy(hostname,_host);
    return;
}
void reqheader(rio_t *rp,char*req,char*method,char*hostname,char*port,char*path){
    sprintf(req,"%s %s HTTP/1.0\r\n",method,path);
    char buf[MAXLINE];
    while(rio_readlineb(rp,buf,MAXLINE)>0){
        if(!strcmp(buf,"\r\n")) break;
        if(strstr(buf,"Host:")!=NULL) continue;
        if(strstr(buf,"User-Agent:")!=NULL) continue;
        if(strstr(buf,"Connection:")!=NULL) continue;
        if(strstr(buf,"Proxy-Connection:")!=NULL) continue;
        sprintf(req,"%s%s",req,buf);
    }
    sprintf(req,"%sHost: %s:%s\r\n",req,hostname,port);
    sprintf(req,"%s%s",req,user_agent_hdr);
    sprintf(req,"%s%s",req,conn_hdr);
    sprintf(req,"%s%s",req,proxy_conn_hdr);
    sprintf(req,"%s\r\n",req);
}
void sbuf_init(sbuf_t *sp,int n)
{
    sp->buf=Calloc(n,sizeof(int));
    sp->n = n;
    sp->front=sp->rear=0;
    Sem_init(&sp->mutex,0,1);
    Sem_init(&sp->slots,0,n);
    Sem_init(&sp->items,0,0);
}
void sbuf_deinit(sbuf_t *sp)
{
    Free(sp->buf);
}
void sbuf_insert(sbuf_t *sp,int item)
{
    P(&sp->slots);
    P(&sp->mutex);
    sp->buf[(++sp->rear)%(sp->n)]=item;
    V(&sp->mutex);
    V(&sp->items);
}
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(&sp->items);
    P(&sp->mutex);
    item=sp->buf[(++sp->front)%(sp->n)];
    V(&sp->mutex);
    V(&sp->slots);
    return item;
}
void *thread(void *vargp)
{
    Pthread_detach(pthread_self());
    while(1){
        int connfd=sbuf_remove(&sbuf);
        doit(connfd);
        Close(connfd);
    }
}