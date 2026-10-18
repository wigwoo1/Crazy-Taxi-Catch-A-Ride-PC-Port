#include "startup_animation.h"
#include <string.h>
static int32_t wrap(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static int32_t sub(int32_t a,int32_t b){return wrap((uint32_t)a-(uint32_t)b);}
static int emit(TaxiStartupAnimationSink sink,void *ctx,unsigned kind,uint32_t value){TaxiStartupAnimationEvent e={.kind=kind,.values={value}};return sink(ctx,&e)?-1:0;}
static uint16_t alpha(int32_t a){int32_t v=a>>8;return (uint16_t)(((uint32_t)v<<8)|((uint32_t)16-(uint32_t)v));}
static int sprite(TaxiStartupAnimationSink sink,void *ctx,uint32_t frame,int32_t x,int32_t y,uint32_t flags){
 TaxiStartupAnimationEvent e={.kind=4,.values={frame,0,(uint32_t)x,(uint32_t)y,0,flags,0,0,0,0,0}};return sink(ctx,&e)?-1:0;
}
static int draw(TaxiStartupAnimationSink sink,void *ctx,int scaled,uint16_t width,int32_t scale){
 TaxiStartupAnimationEvent e={.kind=scaled?5u:4u,.values={0,0,width/2u,52,0,0,scaled?4u:0u,0,scaled?(uint32_t)(int32_t)(int16_t)(uint16_t)scale:0u,0,0}};
 return sink(ctx,&e)?-1:0;
}
int taxi_startup_animation_step(TaxiStartupAnimation *s,uint8_t fade,uint8_t mode,uint16_t width,const uint16_t palette[256],uint8_t confirm,uint8_t start,uint16_t fade_bg,uint16_t fade_obj,TaxiStartupAnimationSink sink,void *ctx){
 if(!s||!palette||!sink)return -1;
 if(s->phase_3574==255){
  s->scroll_3580=65536;s->scroll_step_3584=768;s->alpha_3578=4096;s->alpha_step_357c=64;
  s->scroll_register=256;if(emit(sink,ctx,1,s->scroll_register))return -1;s->phase_3574=0;
 }
 if(fade&&mode==1)return 0;
 switch(s->phase_3574){
 case 0:
  s->blend_control=0x241;if(emit(sink,ctx,2,s->blend_control))return -1;
  s->blend_alpha=alpha(s->alpha_3578);if(emit(sink,ctx,3,s->blend_alpha))return -1;
  s->phase_3574=1;memcpy(s->saved_palette,palette,512);s->palette_snapshot=1;return 0;
 case 1:
  s->scroll_3580=sub(s->scroll_3580,s->scroll_step_3584);s->alpha_3578=sub(s->alpha_3578,s->alpha_step_357c);if(s->alpha_3578<=0)s->alpha_3578=0;
  s->blend_control=0x241;if(emit(sink,ctx,2,s->blend_control))return -1;
  s->blend_alpha=alpha(s->alpha_3578);if(emit(sink,ctx,3,s->blend_alpha))return -1;
  s->scroll_register=(uint16_t)(s->scroll_3580>>8);if(emit(sink,ctx,1,s->scroll_register))return -1;
  if(s->scroll_3580>24576)return 0;
  s->scale_3588=512;s->blend_control=0x1340;if(emit(sink,ctx,2,s->blend_control)||emit(sink,ctx,3,s->blend_alpha))return -1;
  s->phase_3574=2;s->alpha_3578=4096;s->alpha_step_357c=128;return 0;
 case 2:
  if(s->scroll_3580){
   s->scroll_3580=sub(s->scroll_3580,s->scroll_step_3584);if(s->scroll_3580<=0)s->scroll_3580=0;
   s->scroll_register=(uint16_t)(s->scroll_3580>>8);if(emit(sink,ctx,1,s->scroll_register))return -1;
   if(s->scroll_3580<=0){s->scroll_3580=0;s->scroll_register=0;if(emit(sink,ctx,1,0))return -1;}
  }
  s->blend_control=0x1340;if(emit(sink,ctx,2,s->blend_control))return -1;
  s->blend_alpha=alpha(s->alpha_3578);if(emit(sink,ctx,3,s->blend_alpha)||draw(sink,ctx,1,width,s->scale_3588))return -1;
  s->alpha_3578=sub(s->alpha_3578,s->alpha_step_357c);if(s->alpha_3578<=0)s->alpha_3578=0;
  s->scale_3588=sub(s->scale_3588,8);if(s->scale_3588>256)return 0;
  s->scale_3588=256;s->phase_3574=3;s->delay_358c=20;return 0;
 case 3:
  if(draw(sink,ctx,0,width,0))return -1;
  s->delay_358c=sub(s->delay_358c,1);if(s->delay_358c<0){s->delay_358c=0;s->phase_3574=4;}return 0;
 case 4:
  if(draw(sink,ctx,0,width,0)||sprite(sink,ctx,32,wrap((uint32_t)(s->delay_358c>>8)+159u),26,0)||sprite(sink,ctx,1,159,26,1))return -1;
  s->delay_358c=wrap((uint32_t)s->delay_358c+256u);if(s->delay_358c==3328){s->phase_3574=5;s->delay_358c=20;}return 0;
 case 5:
  if(draw(sink,ctx,0,width,0)||sprite(sink,ctx,1,159,26,0))return -1;
  s->delay_358c=sub(s->delay_358c,1);if(s->delay_358c<0){s->delay_358c=0;s->phase_3574=6;s->letters_left_3590=-192;s->letters_right_3594=240;}return 0;
 case 6:
 case 7: {
  unsigned phase=s->phase_3574;
  if(draw(sink,ctx,0,width,0)||sprite(sink,ctx,1,159,26,0))return -1;
  for(unsigned i=0;i<4;i++)if(sprite(sink,ctx,i+2,wrap((uint32_t)s->letters_left_3590+i*32u),111,0))return -1;
  for(unsigned i=0;i<6;i++)if(sprite(sink,ctx,i+6,wrap((uint32_t)s->letters_right_3594+i*32u),138,0))return -1;
  if(phase==6){
   s->letters_left_3590=wrap((uint32_t)s->letters_left_3590+8u);s->letters_right_3594=sub(s->letters_right_3594,8);
   if(s->letters_left_3590==-128){s->delay_358c=0;s->phase_3574=7;}return 0;
  }
  s->delay_358c=sub(s->delay_358c,1);
  if(s->delay_358c<0){
   s->delay_358c=0;s->phase_3574=8;s->scale_3588=512;
   s->revealed_3598=1;
   s->alpha_3578=4096;s->alpha_step_357c=512;
   for(unsigned i=0;i<13;i++){s->letter_scale_37b2[i]=512;s->letter_flags_37a4[i]=4;s->letter_37d8[i]=5632;s->letter_380c[i]=39424;s->letter_3840[i]=65280;}
   s->value_37cc=1864;s->value_37d0=808;s->value_37d4=0;
  }
  s->letters_left_3590=wrap((uint32_t)s->letters_left_3590+8u);s->letters_right_3594=sub(s->letters_right_3594,8);
  if(s->letters_left_3590>=0)s->letters_left_3590=0;if(s->letters_right_3594<=48)s->letters_right_3594=48;return 0;
 }
 case 8:
 default: {
  unsigned phase=s->phase_3574;
  if(s->revealed_3598>13)return -2;
  if(draw(sink,ctx,0,width,0)||sprite(sink,ctx,1,159,26,0))return -1;
  for(unsigned i=0;i<4;i++)if(sprite(sink,ctx,i+2,wrap((uint32_t)s->letters_left_3590+i*32u),111,0))return -1;
  for(unsigned i=0;i<6;i++)if(sprite(sink,ctx,i+6,wrap((uint32_t)s->letters_right_3594+i*32u),138,0))return -1;
  for(unsigned i=0;i<7;i++)if(sprite(sink,ctx,i+25,(int32_t)(8+i*32u),121,1))return -1;
  for(int32_t i=0;i<s->revealed_3598;i++){
   if(phase==8){
    if(i==s->revealed_3598-1){
     s->letter_flags_37a4[i]=(uint8_t)(s->letter_flags_37a4[i]-1u);
     if((int8_t)s->letter_flags_37a4[i]<=0){s->letter_flags_37a4[i]=0;s->revealed_3598++;if(s->revealed_3598==14)s->revealed_3598=13;}
    }
    TaxiStartupAnimationEvent e={.kind=5,.values={(uint32_t)i+12,0,(uint32_t)(24+i*16),129,0,0,0,0,(uint32_t)(int32_t)(int16_t)s->letter_scale_37b2[i],0,0}};
    if(sink(ctx,&e))return -1;
    s->letter_scale_37b2[i]=(uint16_t)(s->letter_scale_37b2[i]-8u);
    if((int16_t)s->letter_scale_37b2[i]<=256){
     s->letter_scale_37b2[i]=256;
     if(i==12){s->phase_3574=(uint8_t)(s->phase_3574+1u);s->delay_358c=0;s->blend_control=0x1f40;s->blend_alpha=0x808;if(emit(sink,ctx,2,s->blend_control)||emit(sink,ctx,3,s->blend_alpha))return -1;}
    }
    if(!fade){
     s->letter_37d8[i]=wrap((uint32_t)s->letter_37d8[i]+(uint32_t)s->value_37cc);
     s->letter_380c[i]=wrap((uint32_t)s->letter_380c[i]+(uint32_t)s->value_37d0);
     s->letter_3840[i]=wrap((uint32_t)s->letter_3840[i]+(uint32_t)s->value_37d4);
     if(s->letter_37d8[i]>65535)s->letter_37d8[i]=65535;
     if(s->letter_380c[i]>65535)s->letter_380c[i]=65535;
     if(s->letter_3840[i]>65535)s->letter_3840[i]=65535;
     uint16_t color=(uint16_t)(((uint32_t)(s->letter_3840[i]>>11)<<10)|((uint32_t)(s->letter_380c[i]>>11)<<5)|(uint32_t)(s->letter_37d8[i]>>11));
     s->visible_letter_colors[i]=color;TaxiStartupAnimationEvent c={.kind=6,.values={(uint32_t)i,color}};if(sink(ctx,&c))return -1;s->saved_palette[48+i]=color;
    }
   }else if(sprite(sink,ctx,(uint32_t)i+12,24+i*16,129,0))return -1;
  }
  if(phase==8){
   s->letters_left_3590=wrap((uint32_t)s->letters_left_3590+8u);s->letters_right_3594=sub(s->letters_right_3594,8);
   if(s->letters_left_3590>=0)s->letters_left_3590=0;if(s->letters_right_3594<=48)s->letters_right_3594=48;return 0;
  }
  s->delay_358c=wrap((uint32_t)s->delay_358c+1u);
  if(s->delay_358c>299||(s->delay_358c>59&&(confirm||start))){
   s->blend_control=0x1f40;s->blend_alpha=0x808;if(emit(sink,ctx,2,s->blend_control)||emit(sink,ctx,3,s->blend_alpha))return -1;
   s->delay_358c=0;s->fade_interval_1771=s->fade_reload_1772=0;
   TaxiStartupAnimationEvent e={.kind=7,.values={0,32,fade_bg,fade_obj}};if(sink(ctx,&e))return -1;s->remaining_359c=0;
  }
  return 0;
 }

 }
}
