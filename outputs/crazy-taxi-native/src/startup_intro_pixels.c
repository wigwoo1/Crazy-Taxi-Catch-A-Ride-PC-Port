#include "startup_intro_pixels.h"
#include <string.h>
#include "palette_fade.h"
typedef struct {uint16_t color;unsigned layer,priority,semi;} Pixel;
static uint16_t blend(uint16_t a,uint16_t b,unsigned eva,unsigned evb){
 uint16_t out=0;if(eva>16)eva=16;if(evb>16)evb=16;
 for(unsigned shift=0;shift<15;shift+=5){unsigned n=(((a>>shift)&31)*eva+((b>>shift)&31)*evb)>>4;if(n>31)n=31;out|=(uint16_t)(n<<shift);}return out;
}
int taxi_startup_intro_pixels(const TaxiStartupIntro *s,const uint8_t *bg0,
 const uint8_t *bg1,const uint16_t palette[256],const uint8_t *tiles,
 size_t size,uint16_t output[240*160]){
 if(!s||!bg0||!bg1||!palette||!tiles||!output)return -1;
 static const unsigned dims[3][4][2]={{{8,8},{16,16},{32,32},{64,64}},{{16,8},{32,8},{32,16},{64,32}},{{8,16},{8,32},{16,32},{32,64}}};
 uint16_t obj_color[240*160];uint8_t obj_priority[240*160],obj_semi[240*160];
 memset(obj_priority,255,sizeof obj_priority);
 for(unsigned i=0;i<128;i++){
  const uint16_t *a=s->transport.output[i];unsigned affine=a[0]&0x100,shape=a[0]>>14;
  if(shape>=3||(!affine&&(a[0]&0x200)))continue;
  if(!(a[0]&0x2000)||(a[0]&0xc00)==0x800)return -2;
  unsigned w=dims[shape][a[1]>>14][0],h=dims[shape][a[1]>>14][1];
  unsigned doubled=affine&&((a[0]&0x200)!=0),fw=w*(doubled+1),fh=h*(doubled+1);
  int left=a[1]&511,top=a[0]&255;if(left>=256)left-=512;if(top>=160)top-=256;
  int16_t matrix[4];unsigned slot=(a[1]>>9)&31;
  for(unsigned j=0;j<4;j++){uint16_t v=s->transport.output[4*slot+j][3];memcpy(&matrix[j],&v,2);}
  unsigned priority=(a[2]>>10)&3;
  for(unsigned y=0;y<fh;y++)for(unsigned x=0;x<fw;x++){
   int dx=left+(int)x,dy=top+(int)y;if(dx<0||dx>=240||dy<0||dy>=160)continue;
   int sx=(int)x,sy=(int)y;
   if(affine){if(taxi_obj_affine_point(matrix,w,h,(int)doubled,(int)x,(int)y,&sx,&sy)!=1)continue;}
   else {if(a[1]&0x1000)sx=(int)w-1-sx;if(a[1]&0x2000)sy=(int)h-1-sy;}
   size_t offset=((a[2]&1023)/2+(unsigned)sy/8*(w/8)+(unsigned)sx/8)*64+(unsigned)sy%8*8+(unsigned)sx%8;
   if(offset>=size)return -3;
   uint8_t index=tiles[offset];if(!index)continue;unsigned p=(unsigned)dy*240+(unsigned)dx;
   if(priority>=obj_priority[p])continue;
   obj_priority[p]=(uint8_t)priority;obj_color[p]=s->palette[index];obj_semi[p]=(uint8_t)((a[0]&0xc00)==0x400);
  }
 }
 for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
  unsigned p=y*240+x;Pixel layers[4];unsigned n=0;
  if(obj_priority[p]!=255)layers[n++]=(Pixel){obj_color[p],4,obj_priority[p],obj_semi[p]};
  uint8_t index=bg0[y*512+((x+s->scroll)&511)];if(index)layers[n++]=(Pixel){palette[index],0,0,0};
  index=bg1[y*256+x];if(index)layers[n++]=(Pixel){palette[index],1,1,0};
  layers[n++]=(Pixel){palette[0],5,4,0};
  for(unsigned i=1;i<n;i++){Pixel t=layers[i];unsigned j=i;while(j&&layers[j-1].priority>t.priority){layers[j]=layers[j-1];j--;}layers[j]=t;}
  uint16_t color=layers[0].color;
  unsigned second=n>1&&((s->blend>>(8+layers[1].layer))&1),first=(s->blend>>layers[0].layer)&1;
  if(second&&(layers[0].semi||(((s->blend>>6)&3)==1&&first)))color=blend(color,layers[1].color,s->alpha&31,(s->alpha>>8)&31);
  if(!layers[0].semi&&((s->blend>>6)&3)==3&&first)color=taxi_palette_fade_darken_rgb555(color,s->brightness);
  output[p]=color;
 }
 return 0;
}
