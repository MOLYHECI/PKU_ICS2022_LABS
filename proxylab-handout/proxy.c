/*
 * proxylab
 * Zimo Meng 2100017702
 * This is a naive proxy.
 */
#include <stdio.h>
#include "csapp.h"
#include "cache.h"

#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection_hdr = "Connection: close\r\n";
static const char *proxy_connection_hdr = "Proxy-Connection: close\r\n";

void doit(int fd);
int parse_uri(char *uri, char *host, char *port, char *path);
int get_header(char *request, rio_t * rp, char *host, char *path);
void *thread(void *vargp);

/*
 * Handler function for SIGPIPE
 * No operation.
 */
void handler(int sig){
    return;
}

/*
 * A little bit modification has been made to the Tiny server main
 */

int main(int argc, char **argv)
{
    printf("%s", user_agent_hdr);
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;


    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    signal(SIGPIPE, handler);
    init_cache();

    listenfd = Open_listenfd(argv[1]);
    while(1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        pthread_create(&tid, NULL, thread, (void*)&connfd);
    }
    free_cache();
    return 0;
}

/*
 * Thread funtion
 * Automatic recycling
 */

void *thread(void * vargp){
    int fd = *((int*) vargp);
    Pthread_detach(pthread_self());
    doit(fd);
    close(fd);
    return NULL;
}

/*
 * Doit function
 * Error handling, caching
 * parse the URL by the client, forward and modify the request by the client
 */

void doit(int fd){
    char req_line[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char host[MAXLINE], port[MAXLINE], path[MAXLINE];
    char buf[MAXLINE];
    char request[MAXLINE];
    char info[MAX_OBJECT_SIZE];
    rio_t clientrio, serverrio;
    int serverfd;
    size_t len;
    int sum = 0;
    Rio_readinitb(&clientrio, fd);
    if (!Rio_readlineb(&clientrio, req_line, MAXLINE)) {
        fprintf(stderr, "fail to read URL\n");
        return;
    }
    if(strlen(req_line) >= MAXLINE - 1) {
        fprintf(stderr, "URL too long to parse\n");
        return;
    }
    sscanf(req_line, "%s %s %s", method, uri, version);
    if(reader(fd, req_line)){
        return;
    }
    if(parse_uri(uri, host, port, path)<0){
        printf("%s\n",req_line);
        printf("%s\n",uri);
        fprintf(stderr, "URL parse error, not a http request\n");
        return;
    }
    if(strcasecmp(method, "GET")!=0){
        printf("Proxy can't handle this method: %s\n", method);
        return;
    }
    if(get_header(request, &clientrio, host, path)==0){
        char tmp[MAXLINE] = "Host: ";
        strcat(tmp, host);
        strcat(tmp, "\r\n");
        strcat(tmp, request);
        strcpy(request, tmp);
    }
    serverfd = Open_clientfd(host, port);
    Rio_readinitb(&serverrio, serverfd);
    {
        char tmp[MAXLINE + 20];
        sprintf(tmp, "GET %s HTTP/1.0\r\n", path);
        strcat(tmp, request);
        Rio_writen(serverfd, tmp, MAXLINE);
    }
    while((len = rio_readnb(&serverrio, buf, MAXLINE))>0){
        if(sum+len < MAX_OBJECT_SIZE){
            memcpy(info+sum, buf, len);
        }
        sum+=len;
        rio_writen(fd, buf, len);
    }
    if(sum < MAX_OBJECT_SIZE){
        writer(req_line, info, sum);
    }
    Close(serverfd);
}

/*
 * Parse_uri function
 * Get host, port, path information by the URL.
 */
int parse_uri(char *uri, char *host, char *port, char *path){
    strcpy(port, "80");
    if(!strstr(uri, "http://")){
        return -1;
    }
    char * h = strstr(uri, "//");
    h+=2;
    char * p = strstr(h, ":");
    char * pp = strstr(h, "/");
    if(p){
        *p = '\0';
        strcpy(host, h);
        *p = ':';
        *pp = '\0';
        strcpy(port, p+1);
        *pp = '/';
        strcpy(path, pp);
    }
    else{
        *pp = '\0';
        strcpy(host, h);
        *pp = '/';
        strcpy(path, pp);
    }
    return 0;
}

/*
 * Read the request header by the client and forwardi them to the server
 */

int get_header(char *request, rio_t * rp, char *host, char *path){
    char buf[MAXLINE], key[MAXLINE], value[MAXLINE];
    int hflag = 0;
    sprintf(request, "GET %s HTTP/1.0\r\n", path);
    Rio_readlineb(rp,buf,MAXLINE);
    while(strcmp(buf, "\r\n")){
        sscanf(buf, "%s %s", key, value);
        printf("%s\n", buf);
        if(!strcasecmp(key, "Host:")){
            hflag = 1;
        }
        if(strcasecmp(key, "User-Agent:")!=0 && strcasecmp(key, "Connection:")!=0 && strcasecmp(key, "Proxy-Connection:")!=0){
            strcat(request, buf);
        }
        Rio_readlineb(rp,buf,MAXLINE);
    }
    strcat(request, user_agent_hdr);
    strcat(request, connection_hdr);
    strcat(request, proxy_connection_hdr);
    strcat(request, "\r\n");
    return hflag;
}