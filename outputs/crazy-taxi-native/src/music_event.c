#include "music_event.h"
#include "music_note.h"
static void word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static void half(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
int taxi_music_row_effect(TaxiMusicState *s,TaxiMusicVoice *v,const TaxiMusicAsset *a,
                         uint8_t command,uint8_t parameter,int jump_seen){
    if(!s||!v||!a||!a->bytes||a->size<0x106||command==11)return 0;
    unsigned hi=parameter>>4,lo=parameter&15;
    switch(command){
    case 12:half(v->bytes+22,parameter);break;
    case 13:{
        unsigned row=hi*10+lo;half(s->state+10,row<=63?row:0);
        if(!jump_seen){
            unsigned order=(unsigned)s->state[5]+1;
            /* Source uses header0 (pattern count), not header1 order limit. */
            half(s->state+8,order<a->bytes[0]?order:0);
        }
        break;
    }
    case 14:
        if(hi==6){
            if(!lo)half(v->bytes+24,s->state[6]);
            else{
                unsigned count=v->bytes[26]|(unsigned)v->bytes[27]<<8;
                count=count?((count-1)&65535):lo;
                half(v->bytes+26,count);
                if(count)s->state[6]=(uint8_t)(v->bytes[24]-1);
            }
        }
        break;
    case 15:s->state[1]=parameter;break;
    default:break;
    }
    return 1;
}
int taxi_music_event_prefix(TaxiMusicVoice *voice,const TaxiMusicAsset *a,
                           const uint8_t e[5],const uint8_t *pitch,uint32_t rate){
    if(!voice||!a||!a->bytes||!e)return 0;
    TaxiMusicVoice v=*voice;
    if(e[1]){
        unsigned index=e[1]-1;
        if(index>=a->samples)return 0;
        TaxiMusicRecord r=a->sample[index];
        if(r.offset>a->size||a->size-r.offset<15)return 0;
        v.bytes[8]=(uint8_t)index;v.sample=a->bytes+r.offset;
        half(v.bytes+20,v.sample[14]);half(v.bytes+22,64);word(v.bytes+12,0);
    }
    if(e[2]>=16&&e[2]<=80)half(v.bytes+22,e[2]-16);
    if(e[0]!=0&&e[0]!=128){
        TaxiMusicNote n;
        if(!v.sample||!taxi_music_note(&n,e[0],v.sample[13],v.sample[12],pitch,rate))return 0;
        v.bytes[0]=n.note;half(v.bytes+2,n.period);word(v.bytes+4,n.frequency);word(v.bytes+16,n.step);
    }
    *voice=v;return 1;
}
