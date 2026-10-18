#include "bitmap_fill.h"
#include <string.h>
static int magnitude(int v){return v<0?-v:v;}
int taxi_bitmap_fill(uint8_t mode,uint16_t width,uint16_t height,
 uint16_t red,uint16_t green,uint16_t blue,const uint16_t palette[256],
 uint8_t *bank0,uint8_t *bank1,size_t capacity){
 size_t n=(size_t)width*height;
 if(mode!=3&&mode!=4)return 0;
 if(mode==3){
  if(!bank0||n>capacity/2)return -1;
  uint16_t color=(red&31)|((green&31)<<5)|((blue&31)<<10);
  for(size_t i=0;i<n;i++){bank0[2*i]=(uint8_t)color;bank0[2*i+1]=(uint8_t)(color>>8);}
 }else{
  if(!palette||!bank1||n>capacity)return -1;
  int best=256;unsigned index=0;
  for(unsigned i=0;i<256;i++){
   unsigned c=palette[i];
   int d=magnitude((int)(int16_t)red-(int)(c&31))+
    magnitude((int)(int16_t)green-(int)((c>>5)&31))+
    magnitude((int)(int16_t)blue-(int)((c>>10)&31));
   if(d<best){best=d;index=i;}
  }
  memset(bank1,(int)index,n);
 }
 return 0;
}
