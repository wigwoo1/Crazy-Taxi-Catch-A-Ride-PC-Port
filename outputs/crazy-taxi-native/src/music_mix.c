#include "music_mix.h"
#include "audio_mix.h"
int taxi_music_fade_gate(uint8_t ended,uint8_t outer){
    return ended!=0||outer==2||outer==1;
}
int16_t taxi_music_fade_step(int16_t factor,uint32_t delta,uint8_t ended,uint8_t outer){
    if(!taxi_music_fade_gate(ended,outer))return factor;
    uint32_t low=((uint32_t)(uint16_t)factor-(delta<<2))&65535u;
    int32_t signed_low=low<32768u?(int32_t)low:(int32_t)low-65536;
    return (int16_t)(signed_low<64?64:signed_low);
}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t asr(uint32_t v,unsigned bits){return (v>>bits)|((v&0x80000000u)?(~0u<<(32-bits)):0);}
int taxi_music_voice_mix(TaxiMusicVoice *v,const TaxiMusicAsset *a,int16_t *dst,size_t count,
    uint8_t global,int fade_enabled,int16_t fade){
    if(!v||!a||!a->bytes||count>0x7fffffff)return -1;
    if(!v->sample)return 0;
    /* Resolve against indexed records, avoiding subtraction of unrelated pointers. */
    size_t offset=a->size;
    for(unsigned i=0;i<a->samples;i++)if(a->sample[i].offset<a->size&&v->sample==a->bytes+a->sample[i].offset){offset=a->sample[i].offset;break;}
    if(offset>a->size||a->size-offset<15)return -1;
    const uint8_t *h=a->bytes+offset,*pcm=h+15;size_t size=a->size-offset-15;
    uint32_t volume=asr((uint32_t)global*half(v->bytes+22),6);
    volume=asr(volume*half(v->bytes+20),6);
    if(fade_enabled)volume=asr(volume*(uint32_t)(int32_t)fade,8);
    uint32_t phase=word(v->bytes+12),step=word(v->bytes+16),next=phase;
    uint32_t repeat=word(h+8);size_t amount=count;int stop=0,status;
    if(repeat){
        status=taxi_audio_mix_loop(dst,count,pcm,size,phase,step,word(h+4)+repeat,word(h+4),volume,&next);
    }else{
        uint32_t remaining=word(h)-(phase>>8),advance=((uint32_t)count*step)>>8;
        /* Source BLE uses signed comparison; usual original lengths are positive. */
        int64_t rem=remaining<0x80000000u?remaining:(int64_t)remaining-0x100000000LL;
        if((int64_t)advance>rem){
            if(!step)return -1;
            uint32_t distance=(word(h)<<8)-phase;
            int64_t signed_distance=distance<0x80000000u?distance:(int64_t)distance-0x100000000LL;
            int64_t quotient=signed_distance/(int64_t)step;
            amount=quotient>0?(size_t)quotient:0;stop=1;
            if(amount>count)return -1;
        }
        status=taxi_audio_mix(dst,amount,pcm,size,phase,step,volume,&next);
    }
    if(status)return status;
    if(amount)for(unsigned i=0;i<4;i++)v->bytes[12+i]=(uint8_t)(next>>(8*i));
    if(stop)v->sample=NULL;
    return 0;
}
