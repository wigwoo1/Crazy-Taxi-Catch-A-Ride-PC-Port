#include "city_palette_owned.h"
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int colors(TaxiCityPaletteOwners *o,uint32_t id,unsigned count,uint16_t *out){
 if(id<0x08000000||count>256)return -1;size_t off=id-0x08000000;
 if(off>o->rom_size||count*2>o->rom_size-off)return -1;
 for(unsigned i=0;i<count;i++)out[i]=(uint16_t)half(o->rom+off+i*2);return 0;
}
int taxi_city_palette_call(TaxiCityPaletteOwners *o,uint32_t target,const uint32_t a[7]){
 if(target!=0x0800e154&&target!=0x0800e5a8)return 0;
 if(!o||!o->registry||!o->banks||!o->fade||!o->rom||!a||o->registry->count>128)return -1;
 if(target==0x0800e154){uint16_t palette[256];TaxiPaletteTransfer transfer;
  if(colors(o,a[0],256,palette)||taxi_palette_register(o->registry,palette,a[0],a[1],a[2],&transfer)<0)return -1;
  return taxi_palette_transfer_apply(o->banks,&transfer,a[0],palette)?-1:1;
 }
 uint16_t storage[128][256];TaxiPaletteSource sources[128];size_t count=o->registry->count;
 for(size_t i=0;i<count;i++){const uint8_t *r=o->registry->records[i];unsigned n=half(r);uint32_t id=word(r+16);if(colors(o,id,n,storage[i]))return -1;sources[i]=(TaxiPaletteSource){id,storage[i],n};}
 if(taxi_palette_fade_start_normal(o->fade,a[0],a[1],a[2],a[3],o->width,o->height)||taxi_palette_fade_registered_normal(o->banks,o->registry,o->fade,sources,count))return -1;
 return 1;
}

int taxi_city_palette_frame_retained(TaxiCityPaletteOwners *o,int32_t delta,uint16_t *mosaic,TaxiPaletteFadeScratch *scratch){
 if(!o||!o->registry||!o->banks||!o->fade||!o->rom||!mosaic||o->registry->count>128)return -1;
 uint16_t storage[128][256];TaxiPaletteSource sources[128];size_t count=o->registry->count;
 for(size_t i=0;i<count;i++){const uint8_t *r=o->registry->records[i];unsigned n=half(r);uint32_t id=word(r+16);if(colors(o,id,n,storage[i]))return -1;sources[i]=(TaxiPaletteSource){id,storage[i],n};}
 return taxi_palette_fade_service_normal_retained(o->banks,o->registry,o->fade,sources,count,delta,o->width,o->height,mosaic,scratch);
}

int taxi_city_palette_apply(TaxiCityPaletteOwners *o){
 if(!o||!o->registry||!o->banks||!o->fade||!o->rom||o->registry->count>128)return -1;
 uint16_t storage[128][256];TaxiPaletteSource sources[128];size_t count=o->registry->count;
 for(size_t i=0;i<count;i++){const uint8_t *r=o->registry->records[i];unsigned n=half(r);uint32_t id=word(r+16);if(colors(o,id,n,storage[i]))return -1;sources[i]=(TaxiPaletteSource){id,storage[i],n};}
 return taxi_palette_fade_registered_normal(o->banks,o->registry,o->fade,sources,count);
}

int taxi_city_palette_gameplay_transfer(TaxiCityPaletteOwners *o,unsigned mode){
 if(!o||!o->registry||!o->banks||!o->rom||o->registry->count>128||mode>1)return -1;
 TaxiPaletteBanks next=*o->banks;
 for(unsigned i=0;i<o->registry->count;i++){
  /* Original22508 skips entire records0/2/11 only in alternate mode.
     Active/cap/fade flags do not gate this explicit transfer. */
  if(mode&&(i==0||i==2||i==11))continue;
  const uint8_t *r=o->registry->records[i];unsigned n=half(r),offset=half(r+2);
  if(n<2||n>256||(offset&1)||offset>512||(n&~1u)*2>512-offset)return -1;
  uint16_t source[256];
  if(colors(o,word(r+16),n,source))return -1;
  uint16_t *destination=(r[7]?next.background:next.objects)+offset/2;
  if(taxi_palette_gameplay_bank_transfer(destination,source,n&~1u,i,mode)<0)return -1;
 }
 *o->banks=next;return 0;
}
int taxi_city_palette_frame(TaxiCityPaletteOwners *o,int32_t delta,uint16_t *mosaic){
 return taxi_city_palette_frame_retained(o,delta,mosaic,NULL);
}
