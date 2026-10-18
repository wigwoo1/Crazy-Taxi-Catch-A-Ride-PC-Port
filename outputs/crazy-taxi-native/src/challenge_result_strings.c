#include "challenge_result_strings.h"
#include "results_state.h"
#include <string.h>
#include <stdio.h>
static int meters(char out[32],int32_t whole,int32_t fraction){
    if(taxi_results_cash_format(out,whole,fraction))return -1;
    size_t n=strlen(out);if(n+2>32)return -1;out[n]='m';out[n+1]=0;return 0;
}
int taxi_challenge_jump_strings(TaxiChallengeJumpStrings *out,
    const uint8_t settings[512],unsigned profile,int32_t distance,const char *empty){
    if(!out||!settings||profile>1||!empty)return -1;
    size_t n=0;while(n<4&&empty[n])n++;if(n>3)return -1;
    TaxiChallengeJumpStrings t={0};memcpy(t.name,settings+208*profile+8,3);
    uint32_t raw=(uint32_t)distance,shifted=(raw>>8)|(distance<0?0xff000000u:0);int32_t whole;
    memcpy(&whole,&shifted,4);
    if(meters(t.result,whole,(int32_t)(((raw&255)*100)>>8)))return -1;
    for(unsigned i=0;i<3;i++){
        const uint8_t *p=settings+208*profile+108+i*4;
        uint32_t word=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
        if(!(word&1)){memcpy(t.records[i],empty,n+1);memcpy(t.names[i],empty,n+1);}
        else {if(meters(t.records[i],(int32_t)(word>>15),(int32_t)((word>>8)&127)))return -1;
            memcpy(t.names[i],settings+208*((word>>7)&1)+8,3);}
    }
    *out=t;return 0;
}

/* Source10988 emits eight bytes and a terminator; negative inputs retain
   signed division/remainder behaviour rather than inventing a clamp. */
static void flag_time(char out[32],int32_t ticks){
    int32_t minutes=ticks/3600,seconds=(ticks-minutes*3600)/60;
    int32_t hundredths=(ticks-minutes*3600-seconds*60)*100/60;
    if(minutes>59){minutes=seconds=59;hundredths=99;}
    out[0]=(char)(48+minutes/10);out[1]=(char)(48+minutes%10);out[2]=':';
    out[3]=(char)(48+seconds/10);out[4]=(char)(48+seconds%10);out[5]=':';
    out[6]=(char)(48+hundredths/10);out[7]=(char)(48+hundredths%10);out[8]=0;
}
int taxi_challenge_flag_strings(TaxiChallengeFlagStrings *out,
    const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty){
    if(!out||!settings||profile>1||!empty)return -1;
    size_t n=0;while(n<4&&empty[n])n++;if(n>3)return -1;
    TaxiChallengeFlagStrings t={0};memcpy(t.name,settings+208*profile+8,3);
    flag_time(t.result,1200-(int32_t)remaining);
    for(unsigned i=0;i<3;i++){
        const uint8_t *p=settings+208*profile+120+i*4;
        uint32_t packed=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
        if(!(packed&1)){memcpy(t.records[i],empty,n+1);memcpy(t.names[i],empty,n+1);}
        else{flag_time(t.records[i],(int32_t)(packed>>15));memcpy(t.names[i],settings+208*((packed>>7)&1)+8,3);}
    }
    *out=t;return 0;
}

int taxi_challenge_ball_strings(TaxiChallengeBallStrings *out,
    const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty){
    if(!out||!settings||profile>1||!empty)return -1;
    size_t n=0;while(n<4&&empty[n])n++;if(n>3)return -1;
    TaxiChallengeBallStrings t={0};memcpy(t.name,settings+208*profile+8,3);
    flag_time(t.result,2700-(int32_t)remaining);
    for(unsigned i=0;i<3;i++){
        const uint8_t *p=settings+208*profile+132+i*4;
        uint32_t packed=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
        if(!(packed&1)){memcpy(t.records[i],empty,n+1);memcpy(t.names[i],empty,n+1);}
        else{flag_time(t.records[i],(int32_t)(packed>>15));memcpy(t.names[i],settings+208*((packed>>7)&1)+8,3);}
    }
    *out=t;return 0;
}

int taxi_challenge_drift_strings(TaxiChallengeDriftStrings *out,const uint8_t settings[512],unsigned profile,int16_t count,const char *empty){
 if(!out||!settings||profile>1||!empty)return -1;
 size_t n=0;while(n<4&&empty[n])n++;if(n>3)return -1;
 TaxiChallengeDriftStrings t={0};memcpy(t.name,settings+208*profile+8,3);
 snprintf(t.result,sizeof t.result,"%d",(int)count);
 for(unsigned i=0;i<3;i++){
  const uint8_t *p=settings+208*profile+144+i*4;
  uint32_t packed=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
  if(!(packed&1)){memcpy(t.records[i],empty,n+1);memcpy(t.names[i],empty,n+1);}
  else{snprintf(t.records[i],sizeof t.records[i],"%d",(int)(packed>>15));memcpy(t.names[i],settings+208*((packed>>7)&1)+8,3);}
 }
 *out=t;return 0;
}

int taxi_challenge_turn_strings(TaxiChallengeTurnStrings *out,
    const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty){
    if(!out||!settings||profile>1||!empty)return -1;
    size_t n=0;while(n<4&&empty[n])n++;if(n>3)return -1;
    TaxiChallengeTurnStrings t={0};memcpy(t.name,settings+208*profile+8,3);
    flag_time(t.result,2100-(int32_t)remaining);
    for(unsigned i=0;i<3;i++){
        const uint8_t *p=settings+208*profile+156+i*4;
        uint32_t packed=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
        if(!(packed&1)){memcpy(t.records[i],empty,n+1);memcpy(t.names[i],empty,n+1);}
        else{flag_time(t.records[i],(int32_t)(packed>>15));memcpy(t.names[i],settings+208*((packed>>7)&1)+8,3);}
    }
    *out=t;return 0;
}


int taxi_challenge_bound_strings(TaxiChallengeBoundStrings *out,
    const uint8_t settings[512],unsigned profile,int16_t remaining,const char *empty){
    if(!out||!settings||profile>1||!empty)return -1;
    size_t n=0;while(n<4&&empty[n])n++;if(n>3)return -1;
    TaxiChallengeBoundStrings t={0};memcpy(t.name,settings+208*profile+8,3);
    flag_time(t.result,2400-(int32_t)remaining);
    for(unsigned i=0;i<3;i++){
        const uint8_t *p=settings+208*profile+168+i*4;
        uint32_t packed=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
        if(!(packed&1)){memcpy(t.records[i],empty,n+1);memcpy(t.names[i],empty,n+1);}
        else{flag_time(t.records[i],(int32_t)(packed>>15));memcpy(t.names[i],settings+208*((packed>>7)&1)+8,3);}
    }
    *out=t;return 0;
}

