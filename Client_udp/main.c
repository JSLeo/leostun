#include "leostun_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


void ddd(char * p)
{
    debug("===================++>%s",p);
}
void main()
{
    ipp s,hb_s;
    sprintf(s.ip,"112.74.107.40");
    s.port=3876;
    sprintf(hb_s.ip,"112.74.107.40");
    hb_s.port=6565;
    if(init_leostun(0,s,hb_s,"2017")<0)
    {
        debug("Initial leostun faile!");
        exit(0);
    }
    leostun_rx_opt(ddd);
    debug("Wait for thread exit")

    for(;;)
    {
        leostun_hb();
        sleep(1);
        leostun_transmit("nihao");
    }
    pthread_join(recv_thread,NULL);
}
