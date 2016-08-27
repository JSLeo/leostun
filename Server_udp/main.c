#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/epoll.h>    //epoll header
#include "credis.h"
#include "mysql/mysql.h"
#include "leostun_s.h"

#define MAX_EVENTS 10

int epfd = -1;
int socket_descriptor;   //接收数据用不参与打洞
int sin_len;

REDIS redis;
const char *iplist="iplist";
pthread_t thread_dealRedis;
int port = 6565;



void * DealRedis(void * arg)
{

    int res;
    char buff[100];
    char sn[20];
    char ip[20];

    char * p;
    char * str_p;
    arg=arg;
    debug("Start DealRedis Task..")
            for(;;)
    {
        res=credis_llen(redis,iplist);
        debug("[2]==>Redis Iist Mount :%d",res)
                if(res>0) {
            p=buff;
            res=credis_lpop(redis,iplist,&p);
            debug("[2]Redis pop values:%s",p)

            str_p=strtok(p,",");
            str_p=strtok(NULL,",");
            strcpy(sn,str_p);
            str_p=strtok(NULL,",");
            strcpy(ip,str_p);
            if(credis_set(redis,sn,ip)<0)
                debug("[2]==>credis set error!")
                        else
                        debug("[2]==>credis key:[%s] value:[%s] set successed!",sn,ip)
                        credis_expire(redis,sn,20);

        }else{
            sleep(1);
        }

    }
}
void Stop()
{
    debug("[*]==>Stop Service ")
            close(epfd);
    credis_close(redis);
    close(socket_descriptor);
    exit(0);
}

int main() {
    char message[100];
    char ipport [100];
    struct sockaddr_in sin;
    struct epoll_event ev, events[MAX_EVENTS];
    int  nfds;
    redis = credis_connect("127.0.0.1",0, 10000);
    signal(SIGTERM, Stop);
    signal(SIGINT, Stop);
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    sin_len = sizeof(sin);
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    init_leostun(0);
    if(bind(socket_descriptor, (struct sockaddr *)&sin, sizeof(sin))<0)
    {
        debug("[1]==>Error: bind faile ")
                exit(EXIT_FAILURE);
    }
    epfd = epoll_create(10);
    if (epfd == -1) {
        debug("[1]==>Error:epoll_create error!")
                exit(EXIT_FAILURE);
    }
    debug("[1]==>epoll_create ")
            ev.events = EPOLLIN;
    ev.data.fd = socket_descriptor;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, socket_descriptor, &ev) == -1) {
        debug("[1]==>Error:epoll_ctl: socket_descriptor")
                exit(EXIT_FAILURE);
    }
    debug("[1]==>Waiting for data form Client  ")
            pthread_create(&thread_dealRedis,NULL,DealRedis,NULL);

    for(;;)
    {
        debug("[1]==>Start epoll_wait ")
                nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            debug("[1]==>Error:epoll_pwait")
        }
        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == socket_descriptor) {
                memset(message,0,sizeof(message));
                recvfrom(socket_descriptor, message, sizeof(message), 0, (struct sockaddr *)&sin, (socklen_t*)&sin_len);
                debug("[1]==>Message:%s ", message)
                        debug("[1]==>udp from:%s ",inet_ntoa(sin.sin_addr))
                        debug("[1]==>udp port from :%d  ",sin.sin_port);
                sprintf(ipport,"%s,%s:%d",message,inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
                if(credis_rpush(redis,iplist,ipport)==-1)  //push to Queue
                    debug("[1]==>Error:Rpush_[redis]:faile")
            } else
            {
                ;;
            }
        }
    }
    return 0;
}
