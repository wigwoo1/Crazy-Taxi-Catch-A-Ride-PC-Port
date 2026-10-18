#include "audio_mix.h"
#include <string.h>
int taxi_audio_mix_loop(int16_t *dst,size_t count,const uint8_t *src,size_t size,
    uint32_t phase,uint32_t step,uint32_t threshold,uint32_t loop,uint32_t volume,uint32_t *next){
    if(!next||(!dst&&count)||(!src&&size))return -1;
    uint32_t cursor=phase;
    int64_t limit=threshold<0x80000000u?threshold:(int64_t)threshold-0x100000000LL;
    for(size_t i=0;i<count;i++){
        uint32_t index=cursor>>8;if(index>=size)return -1;
        if((int64_t)index>=limit)cursor=loop<<8;
        cursor+=step;
    }
    cursor=phase;
    for(size_t i=0;i<count;i++){
        uint32_t index=cursor>>8;
        unsigned byte=src[index];int32_t sample=byte<128?(int32_t)byte:(int32_t)byte-256;
        if((int64_t)index>=limit)cursor=loop<<8;
        uint16_t result=(uint16_t)((uint32_t)(int32_t)dst[i]+(uint32_t)sample*volume);
        memcpy(dst+i,&result,2);cursor+=step;
    }
    *next=cursor;return 0;
}
int taxi_audio_convert(const int16_t *source,uint8_t *output,size_t count){
    if((!source||!output)&&count)return -1;
    for(size_t i=0;i<count;i++){
        int value=source[i]/64;
        if(source[i]<0&&source[i]%64)value--;
        if(value>127)value=127;if(value < -127)value=-127;
        output[i]=(uint8_t)value;
    }
    return 0;
}
int taxi_audio_mix(int16_t *dst,size_t count,const uint8_t *src,size_t size,
    uint32_t phase,uint32_t step,uint32_t volume,uint32_t *next){
    if(!next||(!dst&&count)||(!src&&size))return -1;
    uint32_t cursor=phase;
    for(size_t i=0;i<count;i++,cursor+=step)if((cursor>>8)>=size)return -1;
    cursor=phase;
    for(size_t i=0;i<count;i++,cursor+=step){
        unsigned byte=src[cursor>>8];int32_t sample=byte<128?(int32_t)byte:(int32_t)byte-256;
        uint16_t result=(uint16_t)((uint32_t)(int32_t)dst[i]+(uint32_t)sample*volume);
        memcpy(dst+i,&result,2);
    }
    *next=cursor;return 0;
}
int taxi_audio_mix_packed(int16_t *dst,size_t count,const uint8_t *selectors,size_t blocks,
    const uint8_t *packed,size_t bytes,const uint8_t *codebook,size_t codes,
    uint32_t phase,uint32_t step,uint32_t volume,uint32_t *next){
    if(!next||(!dst&&count)||(!selectors&&blocks)||(!packed&&bytes)||(!codebook&&codes))return -1;
    uint32_t cursor=phase;
    for(size_t i=0;i<count;i++,cursor+=step){
        if((cursor>>12)>=blocks||(cursor>>9)>=bytes)return -1;
        unsigned nibble=(packed[cursor>>9]>>((cursor&256)?4:0))&15;
        if((size_t)selectors[cursor>>12]*16+nibble>=codes)return -1;
    }
    cursor=phase;
    for(size_t i=0;i<count;i++,cursor+=step){
        unsigned nibble=(packed[cursor>>9]>>((cursor&256)?4:0))&15;
        unsigned byte=codebook[(size_t)selectors[cursor>>12]*16+nibble];
        int32_t sample=byte<128?(int32_t)byte:(int32_t)byte-256;
        uint16_t result=(uint16_t)((uint32_t)(int32_t)dst[i]+(uint32_t)sample*volume);
        memcpy(dst+i,&result,2);
    }
    *next=cursor;return 0;
}
