#ifndef  _leostun_c_h_
#define _leostun_c_h_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define leostun_heartbeat           0x35
#define leostun_linkrequest         0x32
#define leostun_stunrequest        0x34
#define leostun_stunresponse     0x04
#define leostun_data                   0x31
#define leostun_data_ack            0x38
#define leostun_get_destip          0x40
#define leostun_get_selfip           0x41


#define debug_leostun



typedef  struct   Ipps{
    char ip[16];
    int port;
}ipp;
struct  session{
    ipp myslef;
    ipp offside;
};

pthread_t recv_thread;
int s_fd;
struct sockaddr_in serverip;
struct sockaddr_in destip;
struct sockaddr_in hb_serverip;
/*send cmd to host  ipp*/
extern int  leo_send_cmd(struct sockaddr_in sin, char cmd, const char *value);
/*send data to offsite client*/
extern int  leo_send_dt(int leostun_fd,ipp s,char *value);
ipp  str2ipp(char * str);
extern int init_leostun(int port,ipp server,ipp hb_serverip);
void * recv_func(void *argv);
#ifdef debug_leostun
#define debug(...)      \
    do{     \
        printf("[leostun]==>"); \
        printf(__VA_ARGS__);                           \
        printf("\n");   \
    } \
    while(0);
#else
#define debug(...)
#endif
#endif
