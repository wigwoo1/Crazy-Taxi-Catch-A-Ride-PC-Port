#include "title_assets.h"
#include <string.h>
static uint32_t word(const uint8_t *r,size_t n){return (uint32_t)r[n]|(uint32_t)r[n+1]<<8|(uint32_t)r[n+2]<<16|(uint32_t)r[n+3]<<24;}
int taxi_obj_assets_load_eleven(TaxiTitleAssets *a,TaxiPaletteRegistration *s,TaxiPaletteBanks *b,const uint8_t *r,size_t size,uint32_t identity,uint32_t destination){
 if(identity<0x08000000||identity>=0x08800000||destination>0xffffffffu-32768)return -1;const size_t resource=identity-0x08000000;size_t offsets[11],tiles[11],palettes[11];unsigned counts[11];size_t lengths[11];uint8_t allocated[11];size_t total=0;unsigned palette_total=0;
 if(!a||!s||!b||!r||size<resource+48||word(r,resource)!=11)return -1;
 /* Validate every range before changing caller-owned storage. */
 for(unsigned g=0;g<11;g++){
  size_t o=resource+(size_t)word(r,resource+4+g*4);if(o>size||size-o<20)return -1;
  uint32_t frames=word(r,o),units=word(r,o+4),count=word(r,o+8),pc=word(r,o+16);
  if(units!=16||frames>(size-o-20)/4||count>1024||pc>16)return -1;
  size_t p=o+20+(size_t)frames*4,t=p+(size_t)pc*32,n=(size_t)count*32;
  if(t>size||n>size-t||p>size||(size_t)pc*32>size-p||total+n>32768)return -1;
  offsets[g]=total;tiles[g]=t;palettes[g]=p;counts[g]=pc;lengths[g]=n;allocated[g]=(uint8_t)(destination>=0x06010000u&&destination<=0x06018000u&&total+n<=0x06018000u-destination);if(allocated[g])total+=n;palette_total+=pc;
 }
 if((unsigned)s->count+palette_total>128||(unsigned)s->obj_bytes+palette_total*32>512)return -1;
 for(unsigned g=0;g<11;g++){
  a->palette_bases[g]=counts[g]?(int32_t)(s->obj_bytes/32):-1;
  size_t n=lengths[g];a->bases[g]=allocated[g]?destination+(uint32_t)offsets[g]:0;if(allocated[g])memcpy(a->objects+offsets[g],r+tiles[g],n);
  for(unsigned j=0;j<counts[g];j++){
   size_t p=palettes[g]+j*32;uint16_t colors[16];TaxiPaletteTransfer transfer;
   for(unsigned k=0;k<16;k++)colors[k]=(uint16_t)(r[p+k*2]|r[p+k*2+1]<<8);
   if(taxi_palette_register_small(s,colors,0x08000000u+(uint32_t)p,32,0,&transfer)<0)return -2;
   if(taxi_palette_transfer_small_apply(b,&transfer,0x08000000u+(uint32_t)p,colors))return -2;
  }
 }
 return 0;
}


int taxi_title_assets_load(TaxiTitleAssets *a,TaxiPaletteRegistration *s,TaxiPaletteBanks *b,const uint8_t *r,size_t size){return taxi_obj_assets_load_eleven(a,s,b,r,size,0x08034634,0x06010000);}