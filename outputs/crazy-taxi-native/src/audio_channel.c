#include "audio_channel.h"
#include <limits.h>
#include "audio_mix.h"
int taxi_audio_channels_stop(TaxiAudioChannel channels[4]){
    if(!channels)return -2;
    int stopped=0;
    for(unsigned i=0;i<4;i++)if(channels[i].sound){
        channels[i].sound=NULL;channels[i].flags4=channels[i].flags5=channels[i].last=0;
        channels[i].phase=channels[i].step=0;stopped++;
    }
    return stopped;
}
int taxi_audio_channel_stop(TaxiAudioChannel *channel,uint16_t rate){
    if(!channel)return -2;
    if(!channel->sound)return 0;
    channel->sound=NULL;channel->flags4=channel->flags5=channel->last=0;
    channel->phase=channel->step=0;channel->rate=rate;
    return 1;
}
uint32_t taxi_audio_effective_volume(uint8_t global_volume,uint8_t channel_volume){
    return ((uint32_t)global_volume*channel_volume)>>6;
}
int taxi_audio_channel_start(TaxiAudioChannel channels[4],const TaxiSoundAsset *sound,
    uint32_t output_rate,uint8_t flags4,uint8_t flags5){
    if(!channels||!sound||!sound->storage||!sound->rate||!output_rate)return -2;
    unsigned best=UINT_MAX;int selected=-1;
    for(unsigned i=0;i<4;i++){
        if(!channels[i].sound){selected=(int)i;break;}
        if(channels[i].stamp<best&&!channels[i].flags4&&!channels[i].flags5){
            best=channels[i].stamp;selected=(int)i;
        }
    }
    if(selected<0)return -1;
    TaxiAudioChannel *channel=channels+selected;
    channel->sound=sound;channel->phase=0;channel->rate=sound->rate;
    channel->step=((uint32_t)sound->rate<<8)/output_rate;
    channel->volume=64;channel->flags4=flags4;channel->flags5=flags5;channel->last=0;
    return selected;
}
int taxi_audio_channel_mix(TaxiAudioChannel *c,int16_t *dst,size_t count,
    const uint8_t *codebook,size_t codes,uint32_t volume){
    if(!c||(!dst&&count)||count>INT_MAX)return -2;
    if(!c->sound||!count)return 0;
    const TaxiSoundAsset *s=c->sound;
    if(c->flags4){
        /* Original26D0C skips packed protected sounds. Raw repeat callback
           receives threshold=length, loop=0, and consumes before resetting. */
        if(s->prefix)return 0;
        uint32_t next=c->phase;
        if(taxi_audio_mix_loop(dst,count,s->storage,s->size,c->phase,c->step,
            s->length,0,volume,&next))return -2;
        c->phase=next;return (int)count;
    }
    if(!c->step)return -2;
    if(s->length>2000000||(c->phase>>8)>s->length)return -2;
    uint32_t advance=((uint32_t)count*c->step)>>8;
    uint32_t remaining=s->length-(c->phase>>8);
    int stop=advance>remaining;
    int64_t tail=((int64_t)s->length<<8)-c->phase;
    size_t amount=stop?(tail>0?(size_t)(tail/c->step):0):count;
    uint32_t next=c->phase;
    if(amount){
        if(s->prefix>s->size)return -2;
        int status=s->prefix?taxi_audio_mix_packed(dst,amount,s->storage,s->prefix,
            s->storage+s->prefix,s->size-s->prefix,codebook,codes,c->phase,c->step,volume,&next):
            taxi_audio_mix(dst,amount,s->storage,s->size,c->phase,c->step,volume,&next);
        if(status)return -2;
        if(s->prefix){
            uint32_t final=c->phase+(uint32_t)(amount-1)*c->step;
            unsigned nibble=(s->storage[s->prefix+(final>>9)]>>((final&256)?4:0))&15;
            c->last=codebook[(size_t)s->storage[final>>12]*16+nibble];
        }
    }
    c->phase=next;
    if(stop){c->sound=NULL;c->flags4=c->flags5=0;}
    return (int)amount;
}
int taxi_audio_channel_busy(const TaxiAudioChannel *channel,const TaxiSoundAsset *sound){
    if(!channel||!channel->sound||!sound||!sound->storage||!channel->sound->storage||
       sound->prefix>sound->size||channel->sound->prefix>channel->sound->size)return 0;
    return channel->sound->storage+channel->sound->prefix==sound->storage+sound->prefix;
}

void taxi_audio_channel_volume(TaxiAudioChannel *c,uint32_t volume){if(c)c->volume=(uint8_t)(volume>64?64:volume);}
int taxi_audio_channel_pitch(TaxiAudioChannel *c,uint32_t rate,uint32_t output_rate){
 if(!c||!output_rate)return -2;
 uint32_t numerator=(uint32_t)(uint16_t)rate<<8;
 int64_t denominator=output_rate<=INT32_MAX?(int64_t)output_rate:(int64_t)output_rate-4294967296LL;
 c->rate=(uint16_t)rate;c->step=(uint32_t)((int64_t)numerator/denominator);return 0;
}

int taxi_audio_channels_stop_rate(TaxiAudioChannel c[4],uint16_t rate){
 if(!c)return -2;
 int n=0;for(unsigned j=0;j<4;j++)n+=taxi_audio_channel_stop(c+j,rate);return n;
}

void taxi_audio_channels_init(TaxiAudioChannel c[4]){
 if(!c)return;
 for(unsigned j=0;j<4;j++){c[j].sound=NULL;c[j].flags4=0;c[j].flags5=0;}
}
