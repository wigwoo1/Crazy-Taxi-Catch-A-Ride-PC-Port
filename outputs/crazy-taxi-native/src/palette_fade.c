#include "palette_fade.h"
uint16_t taxi_palette_fade_scale_rgb555(uint16_t source,uint8_t progress){
    if(progress>31)progress=31;
    uint16_t result=0;
    for(unsigned shift=0;shift<=10;shift+=5)
        result|=(uint16_t)((((source>>shift)&31)*progress/31)<<shift);
    return result;
}
#include <string.h>
void taxi_palette_fade_cap_rgba(const uint8_t *source,uint8_t *output,size_t pixels,int16_t progress) {
    unsigned cap=progress<0?0:(unsigned)progress;
    for(size_t i=0;i<pixels;i++) {
        for(unsigned c=0;c<3;c++) {
            /* Extractors use both floor scaling and bit replication for RGB555.
               Nearest decoding recovers either representation exactly. Keep
               unchanged channels byte-exact, including a full-cap reset. */
            unsigned value=((unsigned)source[i*4+c]*31+127)/255;
            output[i*4+c]=value>cap?(uint8_t)(cap*255/31):source[i*4+c];
        }
        output[i*4+3]=source[i*4+3];
    }
}
uint16_t taxi_palette_fade_cap_rgb555(uint16_t source,int16_t progress) {
    unsigned cap=progress<0?0:(unsigned)progress;
    uint16_t result=0;
    for(unsigned shift=0;shift<15;shift+=5) {
        unsigned channel=(source>>shift)&31;
        if(channel>cap)channel=cap;
        result|=(uint16_t)(channel<<shift);
    }
    return result;
}
int taxi_palette_fade_preserves_palette(uint16_t bg_mask,uint16_t obj_mask) {
    return ((bg_mask|obj_mask)&0x3000)!=0;
}
int taxi_palette_fade_bldy(uint8_t mode,int16_t progress,uint16_t *value) {
    if(mode>1||!value)return 0;
    int half=progress/2;
    if(progress<0&&progress%2)half--; /* Source signed ASR1. */
    int coefficient=16-half;
    if(mode==1&&coefficient<0)coefficient=0;
    if(mode==0&&coefficient>16)coefficient=16;
    *value=(uint16_t)coefficient;
    return 1;
}
uint16_t taxi_palette_fade_darken_rgb555(uint16_t source,uint16_t bldy) {
    unsigned coefficient=bldy&31;
    if(coefficient>16)coefficient=16;
    uint16_t result=0;
    for(unsigned shift=0;shift<15;shift+=5){
        unsigned channel=(source>>shift)&31;
        channel-=channel*coefficient/16;
        result|=(uint16_t)(channel<<shift);
    }
    return result;
}
void taxi_palette_fade_darken_rgba(const uint8_t *source,uint8_t *output,
    size_t pixels,uint16_t bldy) {
    unsigned coefficient=bldy&31;
    if(coefficient>16)coefficient=16;
    for(size_t i=0;i<pixels;i++){
        for(unsigned c=0;c<3;c++){
            uint8_t original=source[i*4+c];
            unsigned channel=((unsigned)original*31+127)/255;
            unsigned darkened=channel-channel*coefficient/16;
            output[i*4+c]=darkened==channel?original:(uint8_t)(darkened*255/31);
        }
        output[i*4+3]=source[i*4+3];
    }
}
int taxi_palette_motion_begin(TaxiPaletteMotion *s,uint8_t mode,
    uint16_t width,uint16_t height) {
    if(mode>1)return 0;
    int32_t w=(int32_t)width*4096,h=(int32_t)height*4096;
    s->position[0]=mode?-w:0;
    s->position[1]=mode?h:0;
    s->position[2]=mode?w:0;
    s->velocity[0]=mode?w/16:-w/16;
    s->velocity[1]=mode?-h/16:h/16;
    s->velocity[2]=mode?-w/16:w/16;
    return 1;
}
int taxi_palette_motion_sprite_offset(const TaxiPaletteMotion *s,
    uint8_t selector,int32_t out[2]) {
    if(selector==4)return 0;
    out[0]=out[1]=0;
    if(selector>=1&&selector<=3) {
        int32_t value=s->position[selector-1];
        /* ARM ASR12 rounds negatives down, unlike C signed division. */
        int32_t pixels=value/4096;
        if(value<0&&value%4096)pixels--;
        out[selector==2?1:0]=pixels;
    }
    return 1;
}
void taxi_palette_motion_step(TaxiPaletteMotion *s,uint8_t active,
    uint8_t mode,int32_t delta,uint16_t width,uint16_t height) {
    if(!active)return;
    for(unsigned i=0;i<3;i++) {
        uint32_t bits=(uint32_t)s->position[i]+(uint32_t)s->velocity[i]*(uint32_t)delta;
        memcpy(&s->position[i],&bits,sizeof bits);
    }
    if(mode==1) {
        if(s->position[0]>=0)s->position[0]=0;
        if(s->position[1]>=0)s->position[1]=0;
        if(s->position[2]<=0)s->position[2]=0;
    } else {
        int32_t left=-(int32_t)width*4096,top=-(int32_t)height*4096;
        int32_t right=(int32_t)width*4096;
        if(s->position[0]<=left)s->position[0]=left;
        if(s->position[1]<=top)s->position[1]=top;
        if(s->position[2]>=right)s->position[2]=right;
    }
}
void taxi_palette_fade_begin(TaxiPaletteFade *s,uint8_t mode) {
    s->mode_1773=mode;s->active_177b=1;s->endpoint_count_1774=0;
    s->progress_178a=mode==0?31:0;
}
int taxi_palette_fade_step(TaxiPaletteFade *s,int32_t delta) {
    if(!s->active_177b)return 0;
    s->interval_1771=(uint8_t)(s->interval_1771-1);
    /* Signed byte <=0 after wrap; e.g.0 becomes255/-1. */
    if(!s->interval_1771||s->interval_1771>=128) {
        s->interval_1771=s->reload_1772;
        uint16_t u=(uint16_t)((uint32_t)(uint16_t)s->progress_178a+
            (s->mode_1773==0?0u-(uint32_t)delta:(uint32_t)delta));
        memcpy(&s->progress_178a,&u,2);
    }
    if(s->progress_178a<0||s->progress_178a>31) {
        s->progress_178a=s->progress_178a<0?0:31;
        s->endpoint_count_1774=(uint8_t)(s->endpoint_count_1774+1);
        if(s->endpoint_count_1774==2) {s->active_177b=0;return 1;}
    }
    return 0;
}
uint16_t taxi_palette_obj_alpha_rgb555(uint16_t source,uint16_t background,
    unsigned layer,uint16_t bldcnt,uint16_t bldalpha){
    source&=0x7fff;
    if(layer>5||layer==4||!(bldcnt&(1u<<(8+layer))))return source;
    unsigned eva=bldalpha&31,evb=(bldalpha>>8)&31;
    if(eva>16)eva=16;
    if(evb>16)evb=16;
    uint16_t result=0;
    for(unsigned shift=0;shift<=10;shift+=5){
        unsigned value=(((source>>shift)&31)*eva+((background>>shift)&31)*evb)>>4;
        if(value>31)value=31;
        result|=(uint16_t)(value<<shift);
    }
    return result;
}

uint16_t taxi_palette_gameplay_alternate_rgb555(uint16_t source){
 unsigned red=source&31,green=(source>>5)&31,blue=(source>>10)&31;
 unsigned brightest=red>green?red:green;if(blue>brightest)brightest=blue;
 unsigned grey=brightest*3/4;return (uint16_t)(grey|(grey<<5)|(grey<<10));
}

int taxi_palette_gameplay_dispatch(uint8_t *request,uint8_t *alternate){
 if(!request||!alternate)return -2;
 uint8_t value=*request;*request=0;
 if(value==1){*alternate=0;return 0;}
 if(value==2){*alternate=1;return 1;}
 return -1;
}

int taxi_palette_gameplay_bank_transfer(uint16_t *destination,const uint16_t *source,
 size_t colors,unsigned record,unsigned mode){
 if(!destination||!source||mode>1)return -1;
 if(mode&&(record==0||record==2||record==11))return 0;
 for(size_t i=0;i<colors;i++)destination[i]=mode?taxi_palette_gameplay_alternate_rgb555(source[i]):source[i];
 return 1;
}

int taxi_palette_gameplay_alternate_rgba(const uint8_t *source,uint8_t *output,size_t pixels){
 if(!source||!output)return -1;
 for(size_t i=0;i<pixels;i++){
  uint16_t color=0;
  for(unsigned c=0;c<3;c++)color|=(uint16_t)((((unsigned)source[i*4+c]*31+127)/255)<<(c*5));
  uint16_t grey=taxi_palette_gameplay_alternate_rgb555(color);
  uint8_t channel=(uint8_t)((grey&31)*255/31),alpha=source[i*4+3];
  output[i*4]=output[i*4+1]=output[i*4+2]=channel;output[i*4+3]=alpha;
 }
 return 0;
}
