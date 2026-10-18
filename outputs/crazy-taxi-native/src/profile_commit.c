#include "profile_commit.h"
#include "score_insert.h"
#include <string.h>
int taxi_profile_commit_normal(uint8_t block[512],const uint8_t snapshot[512],
    uint8_t runtime[800],const TaxiProfileChoice *s,uint8_t rank,
    uint8_t driver,uint8_t license,uint8_t extra_commit,const uint8_t initials[3],
    int32_t whole,int32_t fraction) {
    if(!block||!snapshot||!runtime||!s||!initials||s->current>1||s->choice>2||
       !s->exiting||s->exiting>2||extra_commit||(block[0]&224)>=128||block[4]>1)return -1;
    if(s->new_profile)block[11+s->current*208]=1;
    if(!s->choice)memmove(block,snapshot,512);
    else {
        /* Original D0B4 selects base0C/24/3C/54, then city and profile. */
        unsigned profile=s->choice-1,mode=(block[0]&224)>>5;
        taxi_profile_score_insert(block+12+24*mode+12*block[4]+208*profile,
            (uint8_t)profile,driver,license,whole,fraction);
    }
    taxi_score_insert((uint8_t (*)[8])(block+424),(uint8_t (*)[8])runtime,
        block[0],rank,s->choice,driver,license,initials,whole,fraction);
    /* Original25236 ->2536A refreshes the runtime head even for rank>10;
       choice0 or any timed mode bypasses this DMA entirely. */
    if(s->choice&&!(block[0]&224))memcpy(runtime,block+424,80);
    return s->exiting==2?0:1;
}
