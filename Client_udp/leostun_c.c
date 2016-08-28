#include "leostun_c.h"
/*
    1:linked
    2:linking
    3.ready for offside IP
*/
enum io_stat io_s=leostun_nolink;
rxfunc rx;
const char * SNself;
int leostun_rx_opt(rxfunc f)
{
    rx=f;
    return 0;
}
int  leo_send_dt_ack(ipp dest)
{
    struct sockaddr_in sin;
    sin.sin_addr.s_addr=inet_addr(dest.ip);
    sin.sin_family=AF_INET;
    sin.sin_port=htons(dest.port);
    if(io_s==leostun_readylink||io_s==leostun_link)
    {
        leo_send_cmd(sin,leostun_data_ack,NULL);
    }else{
        debug("no Link now!Link first")
        return -1;
    }
   return 0;
}
void * recv_func(void *argv)
{
    char * msg=NULL;
    char ip[30];
    int sin_len;
    struct sockaddr_in sin;
    argv=argv;
    msg=malloc(sizeof(char)*512);
    debug("Start Listen..")
    for(;;)
    {
        bzero(msg,sizeof(char)*512);
        recvfrom(s_fd, msg,sizeof(char)*512,0,(struct sockaddr *)&sin, (socklen_t*)&sin_len);
        debug("[Listen]recv msg:%s",msg);
        strtok(msg,",");
        switch (msg[0]) {
        case leostun_data:
             io_s=leostun_link;
             strcpy(IO.offside.ip,inet_ntoa(sin.sin_addr));
             IO.offside.port=ntohs(sin.sin_port);
             debug("Received Offside data");
             leo_send_dt_ack(IO.offside);
             msg=strtok(NULL,",");
             rx(msg);
            break;
        case leostun_stunrequest:
            io_s=leostun_readylink;
            strcpy(ip,strtok(NULL,","));
            debug("recived hole request:%s",ip)
            leo_send_dt(str2ipp(ip),"hello");
            break;
        case leostun_stunresponse:
            debug("get hole response")
                    break;
        case  leostun_linkrequest:
            strcpy(ip,strtok(NULL,","));
            debug("get Offsetip:%s",ip);
            if(!strstr("offline",ip)){
                   IO.offside=str2ipp(ip);
                    debug("Result:%s,%d",IO.offside.ip,IO.offside.port);
                    io_s=leostun_readylink;
            }else{
                    io_s=leostun_nolink;
            }
            break;
        case leostun_data_ack:
                  debug("Data Ack...");
                  io_s=leostun_link;
            break;
        default:
            break;
        }
    }
    free(msg);
}
//default port is 38765
int init_leostun(int port,ipp server,ipp hb_server,const char * sn)
{
    struct sockaddr_in sin;
    SNself=sn;
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
    leo_send_cmd(serverip,0,"->");
    pthread_create(&recv_thread,NULL,recv_func,NULL);
    return s_fd;
}
int  leo_send_cmd(struct sockaddr_in sin,char cmd,const char *value)
{
    char buff[100];
    sprintf(buff,"%c,%s",cmd,value);
    debug("buffer :%s",buff);
    if(sendto(s_fd,buff,strlen(buff),0,(struct sockaddr *)&sin,sizeof(sin))<0)
    {
        debug("Send leostun commend faile.")
                return -1;
    }
    debug("Send commed sunccessd!")
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
 int  leo_send_dt(ipp dest,char *value)
 {
     struct sockaddr_in sin;
     sin.sin_addr.s_addr=inet_addr(dest.ip);
     sin.sin_family=AF_INET;
     sin.sin_port=htons(dest.port);
     if(io_s==leostun_readylink||io_s==leostun_link)
     {
         leo_send_cmd(sin,leostun_data,value);
         io_s=leostun_nolink;
     }else{
         debug("no Link now!Link first")
         return -1;
     }
    return 0;
 }
int leostun_hb(void)
{
    int res=0;
    res=leo_send_cmd(hb_serverip,leostun_heartbeat,SNself);
    if(res<0){
        debug("Send HB Faile!")
        return -1;
    }
    debug("Send HB");
    return 0;
}
int leostun_linknow(char * SN)
{
        char  p[50];
        sprintf(p,"%s,%s",SNself,SN);
        leo_send_cmd(serverip,leostun_linkrequest ,p);
        for(int i=0;i<1000;i++)
        {
              if(io_s != leostun_nolink){
                debug("linked ....done")
                 return 0;
              }
              usleep(1000);
        }
        debug("link falie")
        return -1;
}
int leostun_transmit(char * data)
{
    leo_send_dt(IO.offside,data);
    return 0;
}
io_stat leostun_getlinkstatus()
{
    return io_s;
}
