#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_channel.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
    uint8_t payload[32]={0};
    TaxiSoundAsset wanted={.storage=payload,.prefix=8,.size=32};
    TaxiSoundAsset alias={.storage=payload+8,.prefix=0,.size=24};
    TaxiSoundAsset other={.storage=payload+9,.prefix=0,.size=23};
    TaxiAudioChannel query={.sound=&wanted};
    assert(taxi_audio_channel_busy(&query,&wanted));
    assert(taxi_audio_channel_busy(&query,&alias));
    assert(!taxi_audio_channel_busy(&query,&other));
    query.sound=NULL;assert(!taxi_audio_channel_busy(&query,&wanted));
    query.sound=&other;assert(!taxi_audio_channel_busy(&query,&wanted));
    assert(!taxi_audio_channel_busy(NULL,&wanted));
    assert(!taxi_audio_channel_busy(&query,NULL));
    for(unsigned global=0;global<256;global++)for(unsigned channel=0;channel<256;channel++)
        assert(taxi_audio_effective_volume((uint8_t)global,(uint8_t)channel)==global*channel/64);
    uint8_t sample=0;TaxiSoundAsset sound={.rate=10512,.length=1,.storage=&sample,.size=1};
    for(unsigned mask=0;mask<16;mask++){
        TaxiAudioChannel stopped[4],expected[4];memset(stopped,0,sizeof stopped);
        unsigned active=0;
        for(unsigned i=0;i<4;i++){
            stopped[i]=(TaxiAudioChannel){.stamp=123+i,.phase=765,.step=256,.flags4=4,.flags5=2,.volume=(uint8_t)(17+i),.last=89,.sound=mask&(1u<<i)?&sound:NULL};
        }
        memcpy(expected,stopped,sizeof stopped);
        for(unsigned i=0;i<4;i++)if(mask&(1u<<i)){
            expected[i].sound=NULL;expected[i].phase=expected[i].step=0;
            expected[i].flags4=expected[i].flags5=expected[i].last=0;active++;
        }
        assert(taxi_audio_channels_stop(stopped)==(int)active);
        assert(!memcmp(expected,stopped,sizeof stopped));
        assert(!taxi_audio_channels_stop(stopped));
    }
    assert(taxi_audio_channels_stop(NULL)==-2);
    TaxiAudioChannel c[4]={0};
    for(int i=0;i<4;i++){
        c[i].stamp=(uint32_t)(40-i*10);
        assert(taxi_audio_channel_start(c,&sound,21024,0,0)==i);
        assert(c[i].stamp==(unsigned)(40-i*10)&&c[i].step==128&&c[i].volume==64&&!c[i].phase&&!c[i].last);
    }
    assert(taxi_audio_channel_start(c,&sound,15768,1,2)==3&&c[3].step==170&&c[3].flags4==1&&c[3].flags5==2);
    assert(taxi_audio_channel_start(c,&sound,10512,0,0)==2&&c[2].step==256);
    for(unsigned i=0;i<4;i++){c[i].stamp=7;c[i].flags4=0;c[i].flags5=0;}
    assert(taxi_audio_channel_start(c,&sound,10512,0,0)==0);
    for(unsigned mask=0;mask<16;mask++){
        for(unsigned i=0;i<4;i++)c[i].flags5=(uint8_t)((mask>>i)&1);
        TaxiAudioChannel saved[4];memcpy(saved,c,sizeof c);
        int expected=-1;for(int i=0;i<4;i++)if(!(mask&(1u<<i))){expected=i;break;}
        assert(taxi_audio_channel_start(c,&sound,10512,0,0)==expected);
        if(expected<0)assert(!memcmp(saved,c,sizeof c));
    }
    for(unsigned i=0;i<4;i++){c[i].stamp=UINT32_MAX;c[i].flags4=c[i].flags5=0;}
    assert(taxi_audio_channel_start(c,&sound,10512,0,0)==-1);
    c[2].sound=NULL;assert(taxi_audio_channel_start(c,&sound,10512,0,0)==2);
    TaxiAudioChannel saved[4];memcpy(saved,c,sizeof c);
    assert(taxi_audio_channel_start(c,&sound,0,0,0)==-2&&!memcmp(saved,c,sizeof c));
    uint8_t pcm[4]={1,2,255,128};int16_t output[8]={0};
    sound=(TaxiSoundAsset){.rate=10512,.length=4,.storage=pcm,.size=4};
    c[0]=(TaxiAudioChannel){.sound=&sound,.step=256};
    assert(taxi_audio_channel_mix(c,output,4,NULL,0,64)==4&&c[0].sound&&c[0].phase==1024);
    assert(output[0]==64&&output[1]==128&&output[2]==-64&&output[3]==-8192);
    assert(taxi_audio_channel_mix(c,output,4,NULL,0,64)==0&&!c[0].sound);
    c[0]=(TaxiAudioChannel){.sound=&sound,.step=256,.phase=256,.flags5=1};
    memset(output,0,sizeof output);
    assert(taxi_audio_channel_mix(c,output,8,NULL,0,1)==3&&!c[0].sound&&!c[0].flags5&&c[0].phase==1024);
    assert(output[0]==2&&output[1]==-1&&output[2]==-128&&output[3]==0);
    c[0]=(TaxiAudioChannel){.sound=&sound,.step=256,.flags4=1};
    assert(taxi_audio_channel_mix(c,output,4,NULL,0,1)==4&&c[0].phase==1024&&c[0].sound==&sound);
    /* Next frame needs the original trailing sample; absent storage rejects atomically. */
    memcpy(saved,c,sizeof c);assert(taxi_audio_channel_mix(c,output,4,NULL,0,1)==-2&&!memcmp(saved,c,sizeof c));
    uint8_t packed[2]={0,0x10},codes[16]={127,128};
    sound=(TaxiSoundAsset){.rate=10512,.length=2,.prefix=1,.storage=packed,.size=2};
    c[0]=(TaxiAudioChannel){.sound=&sound,.step=128};memset(output,0,sizeof output);
    assert(taxi_audio_channel_mix(c,output,4,codes,16,1)==4&&c[0].last==128&&c[0].phase==512&&c[0].sound);
    assert(output[0]==127&&output[1]==127&&output[2]==-128&&output[3]==-128);
    assert(!taxi_audio_channel_mix(c,output,4,codes,16,1)&&!c[0].sound&&c[0].last==128);
    puts("PASS source four-channel selection, protection, unsigned ties, Q8 rate and caller-owned stamp; no playback claim.");
}

