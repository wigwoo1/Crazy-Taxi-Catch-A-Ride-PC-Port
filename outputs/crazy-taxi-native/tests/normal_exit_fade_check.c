#include "palette_fade_registered.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void half(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void rejected(TaxiPaletteRegistration *registry,TaxiPaletteFadeStart initial,TaxiPaletteSource *source){
 TaxiPaletteBanks banks;memset(&banks,0xa5,sizeof banks);TaxiPaletteBanks before=banks;
 uint16_t mosaic=0x3456;TaxiPaletteFadeStart state=initial;
 assert(taxi_palette_fade_service_normal(&banks,registry,&state,source,1,1,240,160,&mosaic)==-1);
 assert(!memcmp(&banks,&before,sizeof banks)&&!memcmp(&state,&initial,sizeof state)&&mosaic==0x3456);
}
int main(void){
 TaxiPaletteRegistration registry={.count=1};half(registry.records[0],16);registry.records[0][6]=1;registry.records[0][16]=1;
 TaxiPaletteFadeStart start={0};assert(!taxi_palette_fade_start_normal(&start,0,32,0x201,0x201,240,160));
 uint16_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(i*131+32769);
 TaxiPaletteSource source={1,palette,256};TaxiPaletteBanks banks;memset(&banks,0xa5,sizeof banks);uint16_t mosaic=0x3456;
 assert(!taxi_palette_fade_service_normal(&banks,&registry,&start,&source,1,1,240,160,&mosaic));
 TaxiPaletteFadeStart invalid=start;invalid.masks.object_mask=0x401;rejected(&registry,invalid,&source);
 registry.records[0][13]=1;rejected(&registry,start,&source);registry.records[0][13]=0;
 registry.records[0][16]=2;rejected(&registry,start,&source);registry.records[0][16]=1;
 half(registry.records[0]+2,482);rejected(&registry,start,&source);half(registry.records[0]+2,0);
 invalid=start;invalid.fade.progress_178a=32;rejected(&registry,invalid,&source);
 registry.count=33;rejected(&registry,start,&source);registry.count=1;
 assert(!taxi_palette_fade_start_normal(&start,0,32,1,1,240,160));
 assert(!taxi_palette_fade_service_normal(&banks,&registry,&start,&source,1,1,240,160,&mosaic));
 printf("PASS mask0x201 valid service,6 unsupported/invalid cases preserve banks/fade/mosaic, mask1 remains supported\n");return 0;
}
