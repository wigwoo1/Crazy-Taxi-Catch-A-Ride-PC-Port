#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_mix.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
    static int16_t all[65536];static uint8_t converted[65536];
    for(unsigned i=0;i<65536;i++)all[i]=(int16_t)((int)i-32768);
    assert(!taxi_audio_convert(all,converted,65536));
    for(unsigned i=0;i<65536;i++){
        int input=(int)i-32768;
        int expected=input < -8128?-127:input>=8128?127:input>=0?input/64:-((-input+63)/64);
        assert(converted[i]==(uint8_t)expected);
    }
    const uint8_t samples[]={0,127,128,255};
    int16_t dst[8]={0};uint32_t next=99;
    assert(!taxi_audio_mix(dst,8,samples,4,0,128,64,&next)&&next==1024);
    const int16_t expected[]={0,0,8128,8128,-8192,-8192,-64,-64};
    assert(!memcmp(dst,expected,sizeof dst));
    dst[0]=32760;assert(!taxi_audio_mix(dst,1,samples,4,256,256,64,&next)&&dst[0]==-24648&&next==512);
    int16_t saved[8];memcpy(saved,dst,sizeof dst);next=123;
    assert(taxi_audio_mix(dst,8,samples,4,0,256,64,&next)==-1&&next==123&&!memcmp(dst,saved,sizeof dst));
    for(unsigned byte=0;byte<256;byte++){
        uint8_t value=(uint8_t)byte;dst[0]=0;
        assert(!taxi_audio_mix(dst,1,&value,1,0,0,1,&next));
        assert(dst[0]==(byte<128?(int)byte:(int)byte-256));
    }
    assert(!taxi_audio_mix(NULL,0,NULL,0,0xffffffffu,1,64,&next)&&next==0xffffffffu);
    uint8_t codes[32],selectors[2]={0,1},packed[16];int16_t decoded[32]={0};
    for(unsigned i=0;i<32;i++)codes[i]=(uint8_t)((int)i-16);
    for(unsigned i=0;i<16;i++)packed[i]=(uint8_t)(((2*i+1)&15)<<4|((2*i)&15));
    assert(!taxi_audio_mix_packed(decoded,32,selectors,2,packed,16,codes,32,0,256,64,&next)&&next==8192);
    for(unsigned i=0;i<32;i++)assert(decoded[i]==((int)i-16)*64);
    next=123;int16_t original=decoded[0];selectors[0]=2;
    assert(taxi_audio_mix_packed(decoded,1,selectors,2,packed,16,codes,32,0,256,64,&next)==-1&&next==123&&decoded[0]==original);
    puts("PASS source signed8 PCM/Q8 stepping/wrapped16 mixing and invalid-bounds retention; no audio playback claim.");
}
