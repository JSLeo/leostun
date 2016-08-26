#include "leostun_c.h"

int step=0;

void * recv_func(void *argv)
{
    char * msg;
    int sin_len;
    struct sockaddr_in sin;
    argv=argv;
    msg=malloc(sizeof(char)*1024);
    char * buf;
    char  values[20];
    debug("Start Listen..")
   do
    {
        bzero(msg,sizeof(char)*1024);
        recvfrom(s_fd, msg,sizeof(char)*1024,0,(struct sockaddr *)&sin, (socklen_t*)&sin_len);
        debug("[Listen]recv msg:%s",msg);
        switch (msg[0]) {
        case leostun_stunresponse:
            ;;
            break;
        default:
            break;
        }
    }while(1);
    free(msg);
}
//default port is 38765
int init_leostun(int port,ipp server,ipp hb_server)
{
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if(!port)
    sin.sin_port = htons(38765);
    else
    sin.sin_port =htons(port);
    s_fd=socket(AF_INET,SOCK_DGRAM,0);
    if(bind(s_fd, (struct sockaddr *)&sin, sizeof(sin))<0)
    {
        debug("Bind %d Faile!",port);
        return -1;
    }
    debug("Bind %d Successed!",port);
    serverip.sin_addr.s_addr=inet_addr(server.ip);
    serverip.sin_port=htons(server.port);
    serverip.sin_family=AF_INET;
    hb_serverip.sin_port=htons(hb_server.port);
    hb_serverip.sin_addr.s_addr=inet_addr(hb_server.ip);
    hb_serverip.sin_family=AF_INET;
    pthread_create(&recv_thread,NULL,recv_func,NULL);
    return s_fd;
}
int  leo_send_cmd(struct sockaddr_in sin,char cmd,const char *value)
{
      char *buff;
      buff=malloc(sizeof(char)*100);
      sprintf(buff,"%c,%s",cmd,value);
      debug("buffer :%s",buff);
      if(sendto(s_fd,buff,strlen(buff),0,(struct sockaddr *)&sin,sizeof(sin))<0)
      {
          debug("Send leostun commend faile.")
          return -1;
      }
      debug("Send commed sunccessd!")
      free(buff);
      return 0;
}
/*
    source test : "192.168.10.10:9832"
    ----->   ipp.ip="192.168.10.10"
    ----->   ipp.port=9832
*/
int str2ipp(const char * str)
{

    return 0;
}
/*
 *  get dest ip && port
*/
ipp * get_destip(const char *sn)
{

    return 0;
}
/*
 *  get src ip && port
*/
ipp * get_srcip(const char *sn)
{

    return 0;
}
