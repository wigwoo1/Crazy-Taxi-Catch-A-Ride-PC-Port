#include "music_state.h"
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static void put_half(uint8_t *p,unsigned n){p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8);}
int taxi_music_row_prepare(TaxiMusicState *s,const TaxiMusicAsset *a){
    if(!s||!a)return 0;
    unsigned order=half(s->state+8),row=half(s->state+10);
    TaxiMusicRecord pattern;
    if(order<32768){
        if(taxi_music_asset_order(a,(uint8_t)order,&pattern)!=0)return 0;
        s->state[5]=(uint8_t)order;s->pattern_offset=pattern.offset;
    }
    if(row<32768)s->state[6]=(uint8_t)row;
    return 1;
}
int taxi_music_row_finish(TaxiMusicState *s,const TaxiMusicAsset *a){
    if(!s||!a||!a->bytes||a->size<0x106||s->pattern_offset>=a->size)return -1;
    if(half(s->state+10)<32768)return 0;
    unsigned row=(unsigned)s->state[6]+1;
    put_half(s->state+10,row);
    if(row<a->bytes[s->pattern_offset])return 0;
    put_half(s->state+10,0);
    unsigned order=(unsigned)s->state[5]+1;
    put_half(s->state+8,order);
    return order>=a->bytes[1];
}
void taxi_music_tick_finish(TaxiMusicState *s){
    s->state[3]=(uint8_t)(s->state[3]+1);
    if((unsigned)s->state[3]>=(unsigned)s->state[1]+s->state[4]){
        s->state[4]=0;s->state[3]=0;
    }
}
int taxi_music_start(TaxiMusicState *s,const TaxiMusicAsset *a,uint8_t order){
    TaxiMusicRecord pattern;
    if(!s||!a||taxi_music_asset_order(a,order,&pattern)!=0)return 0;
    s->playing=0;
    s->state[1]=a->bytes[5];
    s->state[4]=0;s->state[3]=0;s->state[5]=order;s->state[6]=0;
    s->state[10]=255;s->state[11]=255;
    s->state[8]=255;s->state[9]=255;
    s->state[2]=a->channels;s->pattern_offset=pattern.offset;
    for(unsigned i=0;i<16;i++){
        uint8_t *v=s->voice[i];
        v[9]=0;v[0]=0;v[8]=0;
        v[28]=0;v[29]=0;v[30]=0;v[31]=0;
        v[2]=0;v[3]=0;
    }
    s->playing=1;
    return 1;
}

