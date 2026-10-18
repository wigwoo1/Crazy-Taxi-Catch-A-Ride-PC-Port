#include "../src/effect_update.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
static void put(unsigned char*p,unsigned u){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(u>>(8*i));}
static int hit;static int height(void*c,TaxiPoint p,int32_t*y){(void)c;(void)p;*y=1234;return hit;}
int main(void){unsigned tag,frame,size,clock;int delta,timer;while(scanf("%u%d%d%u%u%d%u",&tag,&delta,&timer,&frame,&size,&hit,&clock)==7){TaxiEffectRecord r;memset(&r,0x5a,sizeof r);put(r.bytes,2);put(r.bytes+0x14,0xfffff000u);put(r.bytes+0x18,8192);put(r.bytes+0x1c,4095);put(r.bytes+0x24,3072);put(r.bytes+0x28,0xfffffc00u);put(r.bytes+0x2c,8192);put(r.bytes+0x40,(unsigned)timer);r.bytes[0x35]=(unsigned char)frame;r.bytes[0x36]=(unsigned char)size;r.bytes[0x37]=(unsigned char)(size>>8);int rc=taxi_effect_update_tag(&r,tag,(TaxiPoint){20,100,300},delta,clock,height,0);if(rc<0)return 1;for(unsigned i=0;i<72;i++)printf("%u%c",r.bytes[i],i==71?'\n':' ');}return 0;}
