#include "title_pixels.h"
#include <string.h>
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
int taxi_title_pixels(const TaxiTitleLoad *s,unsigned scroll,uint16_t brightness,uint16_t out[240*160]){
 if(!s||!out||scroll>272||s->background.control!=0x4440)return -1;
 uint8_t priorities[240*160];memset(priorities,255,sizeof priorities);
 for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
  unsigned sx=x+scroll,tx=sx/8,ty=y/8,e=half(s->background.graphics+8192+(tx/32)*2048+(ty*32+tx%32)*2);
  unsigned xx=sx%8,yy=y%8;if(e&1024)xx=7-xx;if(e&2048)yy=7-yy;
  size_t offset=(e&1023)*32+yy*4+xx/2;unsigned index=(s->background.graphics[offset]>>(4*(xx&1)))&15;
  out[y*240+x]=s->banks.background[index?index+(e>>12)*16:0];
 }
 static const unsigned dims[3][4][2]={{{8,8},{16,16},{32,32},{64,64}},{{16,8},{32,8},{32,16},{64,32}},{{8,16},{8,32},{16,32},{32,64}}};
 for(unsigned i=0;i<128;i++){
  const uint16_t *a=s->transport.output[i];if(!(a[0]&256)&&(a[0]&512))continue;
  unsigned shape=a[0]>>14;if(shape>=3)return -2;unsigned w=dims[shape][a[1]>>14][0],h=dims[shape][a[1]>>14][1];
  int left=a[1]&511,top=a[0]&255;if(left>=256)left-=512;if(top>=160)top-=256;
  if(left>=240||left+(int)w<=0||top>=160||top+(int)h<=0)continue;
  if((a[0]&256)||(a[0]&0x2c00))return -2;
  unsigned priority=(a[2]>>10)&3;if(priority>0)return -2; /* Scoped title requests use priority0. */
  for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++){
   int dx=left+(int)x,dy=top+(int)y;if(dx<0||dx>=240||dy<0||dy>=160)continue;
   unsigned sx=(a[1]&4096)?w-1-x:x,sy=(a[1]&8192)?h-1-y:y;
   size_t offset=((a[2]&1023)+(sy/8)*(w/8)+sx/8)*32+(sy%8)*4+(sx%8)/2;
   if(offset>=sizeof s->assets.objects)return -3;
   unsigned index=(s->assets.objects[offset]>>(4*(sx&1)))&15;if(!index)continue;
   unsigned p=(unsigned)dy*240+(unsigned)dx;if(priority>=priorities[p])continue;
   priorities[p]=(uint8_t)priority;out[p]=s->banks.objects[(a[2]>>12)*16+index];
  }
 }
 for(unsigned i=0;i<240*160;i++)out[i]=taxi_palette_fade_darken_rgb555(out[i],brightness);
 return 0;
}
