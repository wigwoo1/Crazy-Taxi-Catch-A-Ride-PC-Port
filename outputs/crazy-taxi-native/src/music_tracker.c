#include "music_tracker.h"
#include "music_effects.h"
#include <string.h>
int taxi_music_clock_gate(uint8_t *divider,uint8_t enabled,uint8_t loaded,uint8_t playing){
    if(*divider==6){*divider=0;return 0;}
    *divider=(uint8_t)(*divider+1);
    return enabled&&loaded&&playing;
}
static void half(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
int taxi_music_tracker_start(TaxiMusicTracker *t,const TaxiMusicAsset *a,uint8_t order){
    if(!t||!taxi_music_start(&t->state,a,order))return 0;
    for(unsigned i=0;i<16;i++){
        /* Keep one authoritative native voice; source start has partial resets. */
        uint8_t *v=t->voice[i].bytes;v[0]=v[8]=v[9]=v[2]=v[3]=0;
        memset(v+28,0,4);t->voice[i].sample=NULL;
    }
    return 1;
}
static int restart(TaxiMusicTracker *t,const TaxiMusicAsset *a,const TaxiMusicContext *c,int from_effect){
    static const uint8_t orders[5]={63,88,115,34,5};unsigned selected;
    if(from_effect&&c->sound_active&&c->sound_row==3)selected=c->music_selection;
    else if(!c->main_mode){if(!c->random)return 0;selected=c->random(c->context)%4;}
    else selected=4;
    if(selected>=5||!taxi_music_tracker_start(t,a,orders[selected]))return 0;
    half(t->state.state+8,orders[selected]);return 1;
}
int taxi_music_tracker_jump(TaxiMusicTracker *t,const TaxiMusicAsset *a,const TaxiMusicContext *c,uint8_t parameter){
    if(!t||!a||!a->bytes||a->size<0x106||!c)return 0;
    if((c->sound_active&&c->sound_row==3)||!c->main_mode)return restart(t,a,c,1);
    half(t->state.state+10,0);half(t->state.state+8,parameter<a->bytes[1]?parameter:0);
    return 1;
}
int taxi_music_tracker_finish_row(TaxiMusicTracker *t,const TaxiMusicAsset *a,const TaxiMusicContext *c){
    if(!t||!a||!c)return 0;
    int end=taxi_music_row_finish(&t->state,a);
    if(end<0)return 0;
    return !end||restart(t,a,c,0);
}
static int process_events(TaxiMusicTracker *t,const TaxiMusicAsset *a,const uint8_t *pitch,
    uint32_t rate,const TaxiMusicContext *c,int row){
    size_t offset=t->state.pattern_offset+1+(size_t)t->state.state[6]*t->state.state[2]*5;
    unsigned channels=t->state.state[2];
    if(offset>a->size||(size_t)channels*5>a->size-offset)return 0;
    const uint8_t *event=a->bytes+offset;
    if(row){half(t->state.state+8,65535);half(t->state.state+10,65535);}
    int jump_seen=0;
    for(unsigned i=0;i<channels;i++,event+=5){
        TaxiMusicVoice *v=t->voice+i;
        if(!row){
            unsigned volume=v->bytes[22]|(unsigned)v->bytes[23]<<8;
            half(v->bytes+22,taxi_music_volume_tick((uint16_t)volume,event[3],event[4]));continue;
        }
        if(!taxi_music_event_prefix(v,a,event,pitch,rate))return 0;
        if(event[3]==11){
            jump_seen=1;
            if(!taxi_music_tracker_jump(t,a,c,event[4]))return 0;
        }else if(!taxi_music_row_effect(&t->state,v,a,event[3],event[4],jump_seen))return 0;
    }
    return 1;
}
int taxi_music_tracker_row(TaxiMusicTracker *t,const TaxiMusicAsset *a,const uint8_t *pitch,
    uint32_t rate,const TaxiMusicContext *c){
    if(!t||!a||!c||!pitch||!rate||t->state.state[2]>16)return 0;
    return process_events(t,a,pitch,rate,c,1);
}
int taxi_music_tracker_nonrow(TaxiMusicTracker *t,const TaxiMusicAsset *a){
    if(!t||!a||t->state.state[2]>16)return 0;
    return process_events(t,a,NULL,0,NULL,0);
}
int taxi_music_tracker_tick(TaxiMusicTracker *t,const TaxiMusicAsset *a,const uint8_t *pitch,
    uint32_t rate,const TaxiMusicContext *c){
    if(!t||!a||!c||!pitch||!rate||t->state.state[2]>16)return 0;
    int row=t->state.state[3]==0;
    if(row&&!taxi_music_row_prepare(&t->state,a))return 0;
    if(!process_events(t,a,pitch,rate,c,row))return 0;
    if(row&&!taxi_music_tracker_finish_row(t,a,c))return 0;
    taxi_music_tick_finish(&t->state);return 1;
}
