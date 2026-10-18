#include "city_palette_owned.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
static void half(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void word(uint8_t *p,uint32_t v){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(v>>(j*8));}
static void edges(void){
 TaxiPaletteRegistration registry={0};TaxiPaletteBanks banks={0},before=banks;TaxiPaletteFadeStart fade={0},old;TaxiPaletteFadeScratch scratch={0x87654321,0xfedcba98,0xaa},saved=scratch;uint16_t mosaic=0x1357;
 fade.scale_x_177c=fade.scale_y_177e=256;fade.masks.background_mask=fade.masks.object_mask=1;fade.masks.cap=32;registry.count=1;half(registry.records[0],16);registry.records[0][6]=1;fade.fade.active_177b=1;
 old=fade;assert(taxi_palette_fade_service_normal_retained(&banks,&registry,&fade,NULL,0,1,240,160,&mosaic,&scratch)<0);assert(!memcmp(&before,&banks,sizeof banks)&&!memcmp(&old,&fade,sizeof fade)&&!memcmp(&saved,&scratch,sizeof scratch)&&mosaic==0x1357);
 registry.count=0;fade.masks.background_mask=7;old=fade;assert(taxi_palette_fade_service_normal_retained(&banks,&registry,&fade,NULL,0,1,240,160,&mosaic,&scratch)<0);assert(!memcmp(&before,&banks,sizeof banks)&&!memcmp(&old,&fade,sizeof fade)&&!memcmp(&saved,&scratch,sizeof scratch)&&mosaic==0x1357);
 /* ROM-backed city adapter: one16-color source with its32-byte header. */
 uint8_t rom[64]={0};TaxiCityPaletteOwners owner={&registry,&banks,&fade,rom,sizeof rom,240,160};registry.count=1;registry.records[0][6]=1;half(registry.records[0]+4,32);word(registry.records[0]+16,0x08000000);fade.masks.background_mask=fade.masks.object_mask=1;
 assert(!taxi_city_palette_frame_retained(&owner,1,&mosaic,&scratch));assert(scratch.next_record==1&&scratch.color_cursor_identity==0x030050d0&&scratch.written==3);
 word(registry.records[0]+16,0x08000040);saved=scratch;before=banks;old=fade;uint16_t retained=mosaic;
 assert(taxi_city_palette_frame_retained(&owner,1,&mosaic,&scratch)<0);assert(!memcmp(&before,&banks,sizeof banks)&&!memcmp(&old,&fade,sizeof fade)&&!memcmp(&saved,&scratch,sizeof scratch)&&mosaic==retained);
}
int main(void){
 unsigned n,mode,bg,obj,active,progress,profile;uint32_t seed;edges();
 while(scanf("%u %u %u %u %u %u %u %" SCNu32,&n,&mode,&bg,&obj,&active,&progress,&profile,&seed)==8){
  assert(n<=128);TaxiPaletteRegistration registry={0};registry.count=(uint16_t)n;TaxiPaletteBanks banks={0};TaxiPaletteFadeStart fade={0};uint16_t values[256],mosaic=0;
  for(unsigned j=0;j<256;j++)values[j]=(uint16_t)(j*131+seed);
  for(unsigned i=0;i<n;i++){uint8_t *r=registry.records[i];unsigned colors=(i+profile)%5==0?256:16,limit=(i+profile)%3==0?33:32,on=(i+profile)%4!=0;if(profile==4){limit=33;on=1;}if(profile==5){limit=32;on=0;}half(r,colors);half(r+4,limit);r[6]=(uint8_t)on;r[7]=(uint8_t)((i+profile)&1);word(r+16,0x09010000);}
  fade.scale_x_177c=fade.scale_y_177e=256;fade.masks.background_mask=(uint16_t)bg;fade.masks.object_mask=(uint16_t)obj;fade.masks.mode=(uint8_t)mode;fade.masks.cap=32;memset(fade.masks.background_channels,1,3);memset(fade.masks.object_channels,1,3);fade.fade.mode_1773=(uint8_t)mode;fade.fade.active_177b=(uint8_t)active;fade.fade.progress_178a=(int16_t)progress;fade.fade.interval_1771=fade.fade.reload_1772=3;
  TaxiPaletteSource source={0x09010000,values,256};TaxiPaletteFadeScratch scratch={seed,seed^0xa5a5a5a5u,0xff};
  assert(!taxi_palette_fade_service_normal_retained(&banks,&registry,&fade,&source,1,1,240,160,&mosaic,&scratch));
  printf("%" PRIu32 " %" PRIu32 " %u ",scratch.next_record,scratch.color_cursor_identity,scratch.written);
  for(unsigned i=0;i<512;i++){uint16_t color=i<256?banks.background[i]:banks.objects[i-256];printf("%02x%02x",color&255,color>>8);}putchar('\n');
 }
 return 0;
}
