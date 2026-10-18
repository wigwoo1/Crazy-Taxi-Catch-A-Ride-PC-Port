#include "../src/sprite_replacement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned char rom[8388608];
static uint32_t word(unsigned n){return (uint32_t)rom[n]|(uint32_t)rom[n+1]<<8|(uint32_t)rom[n+2]<<16|(uint32_t)rom[n+3]<<24;}
static uint16_t half(unsigned n){return (uint16_t)(rom[n]|rom[n+1]<<8);}
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f||fread(rom,1,sizeof rom,f)!=sizeof rom)return 3;fclose(f);
 unsigned kind,language,index,frame,group,immediate,count;
 while(scanf("%u %u %u %u %u %u %u",&kind,&language,&index,&frame,&group,&immediate,&count)==7){
  unsigned resource=kind?0x34634:0x82104;TaxiSpriteCopyAsset asset={0};asset.group_count=word(resource);
  if((uint16_t)group<asset.group_count){unsigned g=resource+word(resource+4+4*(uint16_t)group);asset.frame_count=word(g);asset.palette_units=word(g+4);asset.destination_base=0x06010000;
   if((uint16_t)frame<asset.frame_count){unsigned m=resource+word(g+20+4*(uint16_t)frame);asset.width_tiles=half(m);asset.height_tiles=half(m+2);asset.tile_index=half(m+16);}}
  uint32_t source=word(0x7ebf48+4*language);TaxiSpriteCopies q;memset(&q,165,sizeof q);q.count=(uint16_t)count;TaxiSpriteImmediateCopy dma={0x11111111,0x22222222,0x33333333};
  if(taxi_sprite_replace(&q,&asset,source,index,frame,group,immediate,&dma)<0)return 4;
  printf("%u %u %u %u ",q.count,dma.source,dma.destination,dma.control);
  const unsigned char *p=(const unsigned char *)q.records;for(unsigned i=0;i<sizeof q.records;i++)printf("%02x",p[i]);puts("");
 }return 0;}
