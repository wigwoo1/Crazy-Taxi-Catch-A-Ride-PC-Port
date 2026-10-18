#include "palette_fade_registered.h"
#include <string.h>
/* Original E5A8 uses division; EE10 uses the immutable F69C-generated
   table floor(progress*channel/31). Mask0x201 then shares the brightest
   transformed channel across RGB. This is not22508's three-quarter grey. */
static uint16_t normal_color(uint16_t source,int16_t progress,unsigned mask){
 if(mask==1)return taxi_palette_fade_cap_rgb555(source,progress);
 uint16_t scaled=taxi_palette_fade_scale_rgb555(source,(uint8_t)progress);
 unsigned red=scaled&31,green=(scaled>>5)&31,blue=(scaled>>10)&31;
 unsigned grey=red>green?red:green;if(blue>grey)grey=blue;
 return (uint16_t)(grey|(grey<<5)|(grey<<10));
}
static int normal_mask(unsigned mask){return mask==1||mask==0x201;}
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_palette_fade_registered_normal(TaxiPaletteBanks *banks,const TaxiPaletteRegistration *registry,const TaxiPaletteFadeStart *start,const TaxiPaletteSource *sources,size_t count){
 if(!banks||!registry||!start||registry->count>128||start->masks.mode>1||start->fade.progress_178a<0||start->fade.progress_178a>31||(!sources&&count))return -1;
 const int bypass=((start->masks.background_mask|start->masks.object_mask)&0x3000)!=0;
 const TaxiPaletteSource *owners[128]={0};
 for(unsigned i=0;i<registry->count;i++){
  const uint8_t *r=registry->records[i];
  if(!r[6]||half(r+4)>start->masks.cap)continue;
  unsigned mask=r[7]?start->masks.background_mask:start->masks.object_mask;
  if(!bypass&&!normal_mask(mask))return -1;
  unsigned colors=half(r),offset=half(r+2);
  if(colors>256||(offset&1)||offset>512||colors*2>512-offset)return -1;
  for(size_t j=0;j<count;j++)if(sources[j].identity==word(r+16)){owners[i]=sources+j;break;}
  if(!owners[i]||!owners[i]->colors||owners[i]->count<colors)return -1;
 }
 TaxiPaletteBanks next=*banks;
 for(unsigned i=0;i<registry->count;i++)if(owners[i]){
  const uint8_t *r=registry->records[i];unsigned colors=half(r)&~1u,offset=half(r+2)/2;
  unsigned mask=r[7]?start->masks.background_mask:start->masks.object_mask;
  uint16_t *destination=(r[7]?next.background:next.objects)+offset;
  for(unsigned j=0;j<colors;j++)destination[j]=bypass?owners[i]->colors[j]:normal_color(owners[i]->colors[j],start->fade.progress_178a,mask);
 }
 *banks=next;return 0;
}

int taxi_palette_fade_registered_frame_normal(TaxiPaletteBanks *banks,const TaxiPaletteRegistration *registry,const TaxiPaletteFadeStart *start,const TaxiPaletteSource *sources,size_t count){
 if(!banks||!registry||!start||registry->count>128||start->masks.mode>1||start->scale_x_177c!=start->scale_y_177e)return -1;
 if((start->masks.background_mask|start->masks.object_mask)&0x2000)return 0;
 if(!normal_mask(start->masks.background_mask)||!normal_mask(start->masks.object_mask))return -1;
 /* Original lookup begins after32 raw records at02001A10. Its immutable
    boot-table equivalence is qualified only while descriptors do not overlap. */
 if((start->masks.background_mask==0x201||start->masks.object_mask==0x201)&&registry->count>32)return -1;
 for(unsigned i=0;i<registry->count;i++)if(registry->records[i][13]||(half(registry->records[i])!=256&&half(registry->records[i])!=16))return -1;
 if(!start->fade.active_177b)return 0;
 return taxi_palette_fade_registered_normal(banks,registry,start,sources,count);
}

int taxi_palette_fade_service_normal_retained(TaxiPaletteBanks *banks,const TaxiPaletteRegistration *registry,TaxiPaletteFadeStart *start,const TaxiPaletteSource *sources,size_t count,int32_t delta,uint16_t width,uint16_t height,uint16_t *mosaic,TaxiPaletteFadeScratch *scratch){
 if(!banks||!start||!mosaic)return -1;
 if((!normal_mask(start->masks.background_mask)&&start->masks.background_mask!=0x2001)||(!normal_mask(start->masks.object_mask)&&start->masks.object_mask!=0x2001))return -1;
 TaxiPaletteBanks next_banks=*banks;TaxiPaletteFadeStart next=*start;
 if(taxi_palette_fade_registered_frame_normal(&next_banks,registry,&next,sources,count))return -1;
 TaxiPaletteFadeScratch next_scratch={0};
 if(scratch){
  next_scratch=*scratch;next_scratch.written=0;
  if(!((start->masks.background_mask|start->masks.object_mask)&0x2000)){
   /* EED0 runs before cap/active rejection, including an inactive fade. */
   if(registry->count){next_scratch.next_record=registry->count;next_scratch.written|=TAXI_PALETTE_SCRATCH_NEXT_RECORD;}
   for(unsigned i=0;i<registry->count;i++){
    const uint8_t *r=registry->records[i];
    if(start->fade.active_177b&&r[6]&&half(r+4)<=start->masks.cap){
     /* F032 initializes the cursor; F320 advances it for every color. */
     next_scratch.color_cursor_identity=0x030050b0u+2u*half(r);
     next_scratch.written|=TAXI_PALETTE_SCRATCH_COLOR_CURSOR;
    }
   }
  }
 }
 uint16_t next_mosaic=*mosaic;
 if(next.fade.active_177b){
  taxi_palette_motion_step(&next.motion,1,next.masks.mode,delta,width,height);
  uint8_t interval=(uint8_t)(next.fade.interval_1771-1);
  if((!interval||interval>=128)&&((next.masks.background_mask|next.masks.object_mask)&0x2000)){
   uint16_t bits=(uint16_t)((uint32_t)(uint16_t)next.fade.progress_178a+(next.masks.mode?(uint32_t)delta:0u-(uint32_t)delta));
   int16_t raw;memcpy(&raw,&bits,2);taxi_palette_fade_bldy(next.masks.mode,raw,&next.bldy);
  }
  taxi_palette_fade_step(&next.fade,delta);next_mosaic=0;
 }
 *banks=next_banks;*start=next;*mosaic=next_mosaic;if(scratch)*scratch=next_scratch;return 0;
}

int taxi_palette_fade_service_normal(TaxiPaletteBanks *banks,const TaxiPaletteRegistration *registry,TaxiPaletteFadeStart *start,const TaxiPaletteSource *sources,size_t count,int32_t delta,uint16_t width,uint16_t height,uint16_t *mosaic){
 return taxi_palette_fade_service_normal_retained(banks,registry,start,sources,count,delta,width,height,mosaic,NULL);
}
