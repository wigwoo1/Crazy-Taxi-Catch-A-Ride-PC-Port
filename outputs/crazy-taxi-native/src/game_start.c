#include "game_start.h"
#include <string.h>
static int16_t half(uint32_t v){uint16_t h=(uint16_t)v;int16_t s;memcpy(&s,&h,2);return s;}
void taxi_game_start_begin(TaxiGameStart *s,int16_t width,const int32_t axes[3]){
    *s=(TaxiGameStart){.phase=-1,.offset=width,.scale=64,.active=1};memcpy(s->axes,axes,sizeof s->axes);
}
void taxi_game_start_begin_ordinary(TaxiGameStart *s,int16_t width){
    const int32_t axes[3]={12288,4096,12288};taxi_game_start_begin(s,width,axes);
}
static unsigned game_start_step_mode(TaxiGameStart *s,int32_t delta,int16_t width,int fade,int exiting,int audio_enabled,int challenge){
    if(fade||exiting||!s->active)return 0;
    if(s->phase==-1){
        if(challenge){taxi_game_start_challenge_intro(s,delta,fade,exiting);return 0;}
        s->rotation=(uint16_t)((s->rotation+(uint32_t)delta*32)&4095);
        for(unsigned i=0;i<3;i++){
            int64_t value=(int64_t)s->axes[i]-(int64_t)delta*64;
            s->axes[i]=value<4096?4096:(int32_t)value;
        }
        if(s->axes[0]<=4096)s->phase=0;
        return 0;
    }
    if(s->phase<=1){
        if(!s->hold){
            s->offset=half((uint32_t)(int32_t)s->offset-(uint32_t)delta*12);
            if(s->offset>0)return 0;
            s->hold=45;s->offset=0;
            return audio_enabled?(s->phase==0?TAXI_START_SOUND_READY:challenge?0:TAXI_START_SOUND_SET):0;
        }
        s->hold=half((uint32_t)(int32_t)s->hold-(uint32_t)delta);
        if(s->hold>0)return 0;
        s->hold=0;s->phase=half((uint32_t)(int32_t)s->phase+(challenge?2u:1u));s->offset=width;
        return TAXI_START_ART;
    }
    if(s->phase!=2)return 0;
    unsigned events=s->scale==64&&audio_enabled?TAXI_START_SOUND_GO:0;
    s->scale=half((uint32_t)(int32_t)s->scale+(uint32_t)delta*9);
    if(s->scale==256)s->scale=257;
    if(s->scale>575){s->scale=512;s->active=0;events|=TAXI_START_FINISHED;}
    return events;
}

void taxi_game_start_begin_challenge(TaxiGameStart *s,int16_t width){
    const int32_t axes[3]={1024,4096,1024};taxi_game_start_begin(s,width,axes);
}
int taxi_game_start_challenge_intro(TaxiGameStart *s,int32_t delta,int fade,int exiting){
    if(!s||fade||exiting||!s->active||s->phase!=-1)return 0;
    for(unsigned i=0;i<3;i++){
        uint32_t raw=(uint32_t)s->axes[i]+((uint32_t)delta<<6);
        int32_t value;memcpy(&value,&raw,4);
        s->axes[i]=value>4096?4096:value;
    }
    if(s->axes[0]>4095){s->phase=0;return 1;}
    return 0;
}

unsigned taxi_game_start_step(TaxiGameStart *s,int32_t delta,int16_t width,int fade,int exiting,int audio_enabled){
    return game_start_step_mode(s,delta,width,fade,exiting,audio_enabled,0);
}
unsigned taxi_game_start_challenge_step(TaxiGameStart *s,int32_t delta,int16_t width,int fade,int exiting,int audio_enabled){
    return game_start_step_mode(s,delta,width,fade,exiting,audio_enabled,1);
}
