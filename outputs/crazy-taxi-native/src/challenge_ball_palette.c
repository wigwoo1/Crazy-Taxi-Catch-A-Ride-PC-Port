#include "challenge_ball_palette.h"
#include "palette_fade.h"
int taxi_challenge_ball_palette_rgba(uint8_t out[32768],const uint8_t indices[8192],const uint16_t registered[256],int16_t cap){
 if(!out||!indices||!registered)return -1;
 for(unsigned i=0;i<8192;i++){
  uint8_t index=indices[i];uint16_t color=taxi_palette_fade_cap_rgb555(registered[index],cap);
  out[4*i]=(uint8_t)((color&31)*255/31);
  out[4*i+1]=(uint8_t)(((color>>5)&31)*255/31);
  out[4*i+2]=(uint8_t)(((color>>10)&31)*255/31);
  out[4*i+3]=index?255:0;
 }
 return 0;
}
