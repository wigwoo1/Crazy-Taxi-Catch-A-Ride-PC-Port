#include "rank_display_owned.h"
#include <string.h>
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
static int32_t word(const uint8_t *p){
    uint32_t u=p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
    int32_t s;memcpy(&s,&u,4);return s;
}
static void put(uint8_t *p,uint32_t value,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(value>>(i*8));}
int taxi_rank_display_owner_valid(const TaxiRankDisplayOwner *o){return o&&o->scalar_16c0&&o->cache_5300;}
int taxi_rank_display_owned_step(TaxiRankDisplayOwner *o,uint8_t settings,
                                uint8_t rank,uint8_t trigger,int32_t delta){
    if(!taxi_rank_display_owner_valid(o))return -1;
    uint8_t *a=o->scalar_16c0,*b=o->cache_5300;
    /* Numeric rank is the same original16D4 owner as fare_session.rank_16d4.
       Synchronize that host projection; initialization still belongs to1EE28. */
    a[0x14]=rank;
    TaxiRankDisplay s={.active_16d5=a[0x15],.phase_16d6=a[0x16],
        .position_16d8=word(a+0x18),.hold_16dc=word(a+0x1c)};
    for(unsigned i=0;i<2;i++){s.current_5388[i]=half(b+0x88+2*i);s.target_538e[i]=half(b+0x8e + 2*i);}
    taxi_rank_display_step(&s,settings,rank,trigger,delta);
    a[0x15]=s.active_16d5;a[0x16]=s.phase_16d6;
    put(a+0x18,(uint32_t)s.position_16d8,4);put(a+0x1c,(uint32_t)s.hold_16dc,4);
    for(unsigned i=0;i<2;i++){put(b+0x88+2*i,s.current_5388[i],2);put(b+0x8e + 2*i,s.target_538e[i],2);}
    return 0;
}
