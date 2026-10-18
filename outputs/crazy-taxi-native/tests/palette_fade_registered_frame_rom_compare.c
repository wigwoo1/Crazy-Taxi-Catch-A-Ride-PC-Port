#include "palette_fade_registered.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void half(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
int main(void){unsigned mode,cap,bg,obj,limit,active,domain,colors,offset,seed,progress,fade_active;
 while(scanf("%u %u %u %u %u %u %u %u %u %u %u %u",&mode,&cap,&bg,&obj,&limit,&active,&domain,&colors,&offset,&seed,&progress,&fade_active)==12){
  TaxiPaletteFadeStart start={0};assert(!taxi_palette_fade_start_normal(&start,mode,cap,bg,obj,240,160));
  start.fade.progress_178a=(int16_t)progress;start.fade.active_177b=(uint8_t)fade_active;start.scale_x_177c=start.scale_y_177e=256;
  TaxiPaletteRegistration registry={.count=1};uint8_t *r=registry.records[0];half(r,colors);half(r+2,offset);half(r+4,limit);r[6]=(uint8_t)active;r[7]=(uint8_t)domain;r[16]=0x34;r[17]=0x5a;r[18]=0x4a;r[19]=8;
  uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(i*131+seed);
  TaxiPaletteSource source={0x084a5a34,palette,256};TaxiPaletteBanks banks;memset(&banks,0xa5,sizeof banks);
  assert(!taxi_palette_fade_registered_frame_normal(&banks,&registry,&start,&source,1));
  const unsigned char *bytes=(const unsigned char *)&banks;for(unsigned i=0;i<sizeof banks;i++)printf("%02x",bytes[i]);puts("");
 }
 return 0;
}
