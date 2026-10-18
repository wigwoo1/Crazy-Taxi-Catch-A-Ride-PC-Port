#include <stdio.h>
#include "../src/bitmap_fill.h"
int main(void){unsigned mode,w,h,r,g,b,seed;while(scanf("%u %u %u %u %u %u %u",&mode,&w,&h,&r,&g,&b,&seed)==7){
 uint16_t palette[256];uint8_t a[128],z[128];
 for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(seed+i*113);
 for(unsigned i=0;i<128;i++){a[i]=(uint8_t)(seed+i*37);z[i]=(uint8_t)(seed+i*53);}
 if(taxi_bitmap_fill((uint8_t)mode,(uint16_t)w,(uint16_t)h,(uint16_t)r,(uint16_t)g,(uint16_t)b,palette,a,z,128))return 2;
 for(unsigned i=0;i<128;i++)printf("%02x",a[i]);
 for(unsigned i=0;i<128;i++)printf("%02x",z[i]);puts("");
 }return 0;}
