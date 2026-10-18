#include "startup_bitmap.h"
#include "bitmap_fill.h"
#include <string.h>
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(p[1]<<8));}
static int load(TaxiPaletteRegistration *registry,TaxiPaletteBanks *banks,TaxiPaletteTransfer *transfer,TaxiDisplayMode *s,const uint8_t *image,
 size_t bytes,uint32_t address,uint16_t palette[256],
 uint8_t bank0[40960],uint8_t bank1[40960]){
 if(!s||!image||!palette||!bank0||!bank1||bytes<520)return -1;
 if(half(image)!=240||half(image+2)!=160||half(image+4)!=256||half(image+6)!=1)return -2;
 /* Validate the bounded compressed stream before changing caller state. */
 size_t end=520;
 for(unsigned y=0;y<160;y++){
  unsigned x=0;
  while(x<240){
   if(end>=bytes)return -3;
   unsigned c=image[end++],n=c&127;
   if(!n||n>240-x)return -3;
   size_t used=(c&128)?n:1;
   if(used>bytes-end)return -3;
   end+=used;x+=n;
  }
 }
 if(taxi_display_mode_four(s,4,0))return -4;
 if(registry&&taxi_palette_registration_reset(registry))return -4;
 for(unsigned i=0;i<256;i++)palette[i]=half(image+8+2*i);
 uint32_t ptr=address+8;
 for(unsigned i=0;i<4;i++)s->globals[20+i]=(uint8_t)(ptr>>(8*i));
 if(registry){
  if(taxi_palette_register(registry,palette,ptr,32,1,transfer)<0)return -4;
  if(taxi_palette_transfer_apply(banks,transfer,ptr,palette))return -4;
 }
 for(unsigned pass=0;pass<2;pass++){
  if(taxi_bitmap_fill(4,240,160,0,0,0,palette,bank0,bank1,40960))return -4;
  size_t cursor=520;uint8_t *out=pass?bank1:bank0;
  for(unsigned y=0;y<160;y++){
   unsigned x=0;
   while(x<240){
    unsigned c=image[cursor++],n=c&127;
    if(c&128){memcpy(out+y*240+x,image+cursor,n);cursor+=n;}
    else{memset(out+y*240+x,image[cursor++],n);}
    x+=n;
   }
  }
 }
 s->control&=0xffef;s->globals[8]=0;return 1;
}

int taxi_startup_bitmap_load(TaxiDisplayMode *s,const uint8_t *image,size_t bytes,
 uint32_t address,uint16_t palette[256],uint8_t bank0[40960],uint8_t bank1[40960]){
 return load(NULL,NULL,NULL,s,image,bytes,address,palette,bank0,bank1);
}
int taxi_startup_bitmap_load_registered(TaxiDisplayMode *s,TaxiPaletteRegistration *registry,
 TaxiPaletteBanks *banks,const uint8_t *image,size_t bytes,uint32_t address,
 uint16_t source_palette[256],uint8_t bank0[40960],uint8_t bank1[40960],TaxiPaletteTransfer *transfer){
 if(!registry||!banks||!transfer||registry->count>128)return -1;
 return load(registry,banks,transfer,s,image,bytes,address,source_palette,bank0,bank1);
}
