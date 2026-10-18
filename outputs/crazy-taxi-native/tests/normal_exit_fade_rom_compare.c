#include "palette_fade_registered.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void half(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
int main(int argc,char **argv){
 uint16_t original[256];assert(argc==2);FILE *rom=fopen(argv[1],"rb");assert(rom&&!fseek(rom,0x4a5a34,SEEK_SET));
 unsigned char raw[512];assert(fread(raw,1,512,rom)==512&&!fclose(rom));for(unsigned i=0;i<256;i++)original[i]=(uint16_t)(raw[i*2]|(unsigned)raw[i*2+1]<<8);unsigned stage,mode,cap,bg,obj,limit,active,domain,colors,offset,seed,progress,fade_active,delta,interval,reload,endpoints;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",&stage,&mode,&cap,&bg,&obj,&limit,&active,&domain,&colors,&offset,&seed,&progress,&fade_active,&delta,&interval,&reload,&endpoints)==17){
  TaxiPaletteFadeStart start={0};start.scale_x_177c=start.scale_y_177e=256;start.bldcnt=0x1234;start.bldy=0x2345;
  start.fade.interval_1771=(uint8_t)interval;start.fade.reload_1772=(uint8_t)reload;
  assert(!taxi_palette_fade_start_normal(&start,mode,cap,bg,obj,240,160));
  TaxiPaletteRegistration registry={.count=1};uint8_t *r=registry.records[0];half(r,colors);half(r+2,offset);half(r+4,limit);r[6]=(uint8_t)active;r[7]=(uint8_t)domain;r[16]=0;r[17]=0;r[18]=1;r[19]=9;
  uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=seed==65536?original[i]:(uint16_t)(i*131+seed);
  if(seed==65536){r[16]=0x34;r[17]=0x5a;r[18]=0x4a;r[19]=8;}
  TaxiPaletteSource source={seed==65536?0x084a5a34:0x09010000,palette,256};TaxiPaletteBanks banks;memset(&banks,0xa5,sizeof banks);uint16_t mosaic=0x3456;
  if(stage){start.fade.progress_178a=(int16_t)progress;start.fade.active_177b=(uint8_t)fade_active;start.fade.endpoint_count_1774=(uint8_t)endpoints;start.bldy=0x2345;
   assert(!taxi_palette_fade_service_normal(&banks,&registry,&start,&source,1,(int32_t)delta,240,160,&mosaic));
  }else assert(!taxi_palette_fade_registered_normal(&banks,&registry,&start,&source,1));
  const unsigned char *bytes=(const unsigned char *)&banks;for(unsigned i=0;i<sizeof banks;i++)printf("%02x",bytes[i]);
  printf(" %u %u %u %u %d %u %u %u %u %u %u %u %u %u",start.masks.mode,start.masks.cap,start.masks.background_mask,start.masks.object_mask,start.fade.progress_178a,start.fade.active_177b,start.fade.interval_1771,start.fade.reload_1772,start.fade.endpoint_count_1774,start.scale_x_177c,start.scale_y_177e,start.bldcnt,start.bldy,mosaic);
  for(unsigned i=0;i<6;i++)printf(" %d",i<3?start.motion.position[i]:start.motion.velocity[i-3]);
  for(unsigned i=0;i<6;i++)printf(" %u",i<3?start.masks.background_channels[i]:start.masks.object_channels[i-3]);puts("");
 }
 return 0;
}
