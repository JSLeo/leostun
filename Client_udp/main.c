#include "leostun_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>



void main()
{
    ipp s,hb_s;
    sprintf(s.ip,"127.0.0.01");
    s.port=3876;
    sprintf(hb_s.ip,"127.0.0.01");
    hb_s.port=6565;
    if(init_leostun(0,s,hb_s)<0)
    {
        debug("Initial leostun faile!");
        exit(0);
    }
    debug("Wait for thread exit")

    for(;;)
    {
        leo_send_cmd(hb_serverip,leostun_heartbeat,"2016010199");
        debug("Send HB");
        sleep(1);
        leo_send_cmd(serverip,leostun_linkrequest ,"2016010199,201657209866");
        sleep(1);
    }
    pthread_join(recv_thread,NULL);
}
