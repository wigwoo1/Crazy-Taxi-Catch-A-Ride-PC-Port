#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "music_mix.h"
static void put(uint8_t *p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(i*8));}
static int signed8(unsigned n){return n<128?(int)n:(int)n-256;}
int main(int argc,char **argv){
    const uint32_t deltas[]={0,1,2,48,64,65535,0x40000000u,0xffffffffu};
    for(unsigned raw=0;raw<65536;raw++)for(unsigned d=0;d<sizeof deltas/sizeof deltas[0];d++){
        int16_t factor=(int16_t)(raw<32768?(int32_t)raw:(int32_t)raw-65536);
        unsigned result=(unsigned)((uint64_t)raw+0x400000000ULL-(uint64_t)deltas[d]*4)&65535;
        int expected=result<32768?(int)result:(int)result-65536;
        if(expected<64)expected=64;
        assert(taxi_music_fade_step(factor,deltas[d],1,0)==expected);
        assert(taxi_music_fade_step(factor,deltas[d],0,0)==factor);
    }
    int16_t factor=256;
    for(unsigned i=0;i<48;i++)factor=taxi_music_fade_step(factor,1,0,2);
    assert(factor==64&&taxi_music_fade_step(factor,1,0,2)==64);
    for(unsigned ended=0;ended<256;ended++)for(unsigned outer=0;outer<256;outer++)
        assert(taxi_music_fade_gate((uint8_t)ended,(uint8_t)outer)==
            (ended!=0||outer==1||outer==2));
    assert(argc==2);uint8_t *b=malloc(181241);assert(b);FILE *f=fopen(argv[1],"rb");assert(f);
    assert(fread(b,1,181241,f)==181241);fclose(f);TaxiMusicAsset a;assert(!taxi_music_asset_index(&a,b,181241));
    TaxiMusicVoice v={0};v.sample=b+a.sample[0].offset;v.bytes[20]=64;v.bytes[22]=64;
    put(v.bytes+12,(a.sample[0].length-1)*256);put(v.bytes+16,256);
    int16_t dst[4]={0};const uint8_t *pcm=v.sample+15;
    assert(!taxi_music_voice_mix(&v,&a,dst,3,64,0,0));
    assert(dst[0]==signed8(pcm[15463])*64);
    /* Actual next record header, not fabricated padding or clamped last byte. */
    assert(dst[1]==signed8(pcm[15464])*64&&dst[2]==signed8(pcm[13393])*64);
    assert(v.sample==b+a.sample[0].offset);
    v.sample=b+a.sample[8].offset;pcm=v.sample+15;put(v.bytes+12,(a.sample[8].length-2)*256);
    for(unsigned i=0;i<4;i++)dst[i]=7;
    assert(!taxi_music_voice_mix(&v,&a,dst,4,64,0,0)&&!v.sample);
    assert(dst[0]==7+signed8(pcm[6966])*64&&dst[1]==7+signed8(pcm[6967])*64&&dst[2]==7&&dst[3]==7);
    v.sample=b+a.sample[0].offset;put(v.bytes+12,0);put(v.bytes+16,256);memset(dst,0,sizeof dst);
    assert(!taxi_music_voice_mix(&v,&a,dst,1,32,1,128));assert(dst[0]==signed8((v.sample+15)[0])*16);
    free(b);puts("PASS original music channel: actual loop boundary/adjacent header, ordinary stop/tail, volume/fade");return 0;
}
