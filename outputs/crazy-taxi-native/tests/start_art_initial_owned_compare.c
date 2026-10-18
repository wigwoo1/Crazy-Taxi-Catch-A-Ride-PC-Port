#include "start_art_owned.h"
#include "hud_start_owned.h"
#include "taxi_graphics_package.h"
#include "sprite_copy_flush_owned.h"
#include <stdio.h>
#include <string.h>
static TaxiTitleAssets a;static TaxiPaletteRegistration p;static TaxiPaletteBanks b;
static void hex(const void *v,size_t n){const unsigned char *q=v;for(size_t j=0;j<n;j++)printf("%02x",q[j]);}
int main(void){TaxiGraphicsPackage pkg={0};if(taxi_graphics_package_load(&pkg,"outputs/crazy-taxi-native/assets/taxis/graphics.ctg"))return 2;unsigned phase,lang;while(scanf("%u%u",&phase,&lang)==2){memset(&a,165,sizeof a);memset(&p,165,sizeof p);memset(&b,165,sizeof b);p.count=16;FILE *f=fopen("outputs/crazy-taxi-native/reports/ordinary-native-assets.bin","rb");if(!f)return 3;for(unsigned j=0;j<11;j++)if(fread(&a.bases[j],4,1,f)!=1||fread(&a.palette_bases[j],4,1,f)!=1)return 3;fclose(f);TaxiOrdinaryGraphicsOwners g={0};g.assets=&a;g.palette=&p;g.banks=&b;g.rom=pkg.bytes;g.rom_size=pkg.size;g.destination=0x06014000;TaxiSpriteCopies copies={0};TaxiSpriteImmediateCopy dma={0};TaxiHudStartOwners hud={0};hud.graphics=&g;hud.copies=&copies;hud.immediate=&dma;TaxiHudStartInput in={0,128,(uint8_t)lang,(uint8_t)phase,(uint8_t)phase};if(taxi_hud_start_owned(&hud,&in)||copies.count)return 4;uint32_t pal=0;for(unsigned j=0;j<4;j++)pal|=(uint32_t)p.records[11][16+j]<<(8*j);if(pal<0x08000000||pal-0x08000000>pkg.size-32)return 5;for(unsigned j=0;j<16;j++)b.objects[176+j]=(uint16_t)(pkg.bytes[pal-0x08000000+2*j]|(uint16_t)pkg.bytes[pal-0x08000000+2*j+1]<<8);uint8_t pixels[3][4096];if(taxi_start_art_decode_owned(pixels,&g))return 8;hex(pixels,sizeof pixels);printf(" ");hex(a.objects,sizeof a.objects);printf(" ");hex(b.objects+176,32);printf(" ");hex(p.records[11]+16,4);puts("");}return 0;}
