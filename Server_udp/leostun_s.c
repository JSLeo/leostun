#include "leostun_s.h"
#include "credis.h"

int step=0;

void * recv_func(void *argv)
{
    char * msg;
    char * str_p;
    int sin_len;
    char sn_my[20];
    char sn[20];
    char ip[25];
    char ip_2[25];
    char buff[512];
    int res;
    REDIS redis;
    struct session IO;
    struct sockaddr_in sin;
    argv=argv;

    redis = credis_connect("127.0.0.1",0, 10000);
    debug("Start Listen..")
for(;;)
    {
        msg=buff;
        bzero(msg,sizeof(char)*512);
        recvfrom(s_fd, msg,sizeof(char)*512,0,(struct sockaddr *)&sin, (socklen_t*)&sin_len);
        debug("[Listen]recv msg:%s",msg);
        switch (*msg) {
        case leostun_linkrequest:
           str_p=strtok(msg,",");
           str_p=strtok(NULL,",");
           strcpy(sn_my,str_p);   //get registor sn
           str_p=strtok(NULL,",");
           strcpy(sn,str_p); //get offside sn
           debug("SN_MY:%s;SN_U:%s",sn_my,sn);
           res=credis_get(redis,sn,&msg); //if offside mechine online
           if(res<0)strcpy(ip,"offline");
           else{strcpy(ip,msg);}
           credis_get(redis,sn_my,&msg); //if offside mechine online
           memcpy(ip_2,msg,sizeof(char)*25);
           send_to_client(str2ipp(msg),leostun_linkrequest,ip);
           if(!strstr("offline",ip))send_to_client(str2ipp(ip),leostun_stunrequest,ip_2);
            break;
        default:
          break;
      }
    }

}
//default port is 3876
int init_leostun(int port)
{
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    if(!port)
    sin.sin_port = htons(3876);
    else
    sin.sin_port =htons(port);
    s_fd=socket(AF_INET,SOCK_DGRAM,0);
    if(bind(s_fd, (struct sockaddr *)&sin, sizeof(sin))<0)
    {
        debug("Bind %d Faile!",port);
        return -1;
    }
    debug("Bind %d Successed!",port);
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
ipp  str2ipp(char * str)
{
    ipp ipstr;
    char * q;
    char * p;
    q=str;
    p=strtok(q,":");
    strcpy(ipstr.ip,p);
    p=strtok(NULL,":");
    ipstr.port=atoi(p);
    return ipstr;
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

int send_to_client(ipp s,char cmd,const char *value)
{
     struct sockaddr_in sin;
     sin.sin_addr.s_addr=inet_addr(s.ip);
     sin.sin_family=AF_INET;
     sin.sin_port=htons(s.port);
      return leo_send_cmd(sin,cmd,value);
}
