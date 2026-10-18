#include "challenge_profile_commit.h"
#include "challenge_completion.h"
#include <string.h>
static int challenge_profile_commit(uint8_t block[512],const uint8_t snapshot[512],
    const TaxiProfileChoice *s,uint8_t rank,uint8_t driver,int32_t score,int submode){
    if(!block||!snapshot||!s||s->current>1||s->choice>2||
       !s->exiting||s->exiting>2||rank>3||
       (block[0]&224)!=128||(snapshot[0]&224)!=128)return -1;
    if(s->new_profile)block[11+208u*s->current]=1;
    if(!s->choice)memmove(block,snapshot,512);
    else if(rank){
        uint8_t names[2][4];
        memcpy(names[0],block+8,4);memcpy(names[1],block+216,4);
        if(submode==5)taxi_challenge_bound_recalculate(block,snapshot,s->current,driver,(int16_t)score);
        else if(submode==4)taxi_challenge_turn_recalculate(block,snapshot,s->current,driver,(int16_t)score);
        else if(submode==3)taxi_challenge_drift_recalculate(block,snapshot,s->current,driver,(int16_t)score);
        else if(submode==2)taxi_challenge_ball_recalculate(block,snapshot,s->current,driver,(int16_t)score);
        else if(submode==1)taxi_challenge_flag_recalculate(block,snapshot,s->current,driver,(int16_t)score);
        else taxi_challenge_jump_recalculate(block,snapshot,s->current,driver,score);
        memcpy(block+8,names[0],4);memcpy(block+216,names[1],4);
    }
    /* D0B4(choice-1) has no mode128 branch;25226 skips ordinary top10. */
    return s->exiting==2?0:1;
}

int taxi_challenge_profile_commit_jump(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int32_t distance){return challenge_profile_commit(block,snapshot,choice,rank,driver,distance,0);}
int taxi_challenge_profile_commit_flag(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining){return challenge_profile_commit(block,snapshot,choice,rank,driver,remaining,1);}

int taxi_challenge_profile_commit_ball(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining){return challenge_profile_commit(block,snapshot,choice,rank,driver,remaining,2);}

int taxi_challenge_profile_commit_drift(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t count){return challenge_profile_commit(block,snapshot,choice,rank,driver,count,3);}

int taxi_challenge_profile_commit_turn(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining){return challenge_profile_commit(block,snapshot,choice,rank,driver,remaining,4);}

int taxi_challenge_profile_commit_bound(uint8_t block[512],const uint8_t snapshot[512],const TaxiProfileChoice *choice,uint8_t rank,uint8_t driver,int16_t remaining){return challenge_profile_commit(block,snapshot,choice,rank,driver,remaining,5);}
