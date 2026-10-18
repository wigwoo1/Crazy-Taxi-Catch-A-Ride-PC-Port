#include "../src/hud_start_owned.h"
#include "../src/taxi_graphics_package.h"
#include <stdio.h>
#include <string.h>
static TaxiTitleAssets assets;static TaxiPaletteRegistration palette;
static TaxiSpriteCopies copies;static TaxiSpriteImmediateCopy dma;
static void output(const void *p,size_t n){const unsigned char *b=p;for(size_t j=0;j<n;j++)printf("%02x",b[j]);}
int main(void){TaxiGraphicsPackage pkg={0};if(taxi_graphics_package_load(&pkg,"outputs/crazy-taxi-native/assets/taxis/graphics.ctg"))return 2;unsigned seed,driver,demo;
 while(scanf("%u %u %u",&seed,&driver,&demo)==3){memset(&assets,seed,sizeof assets);memset(&palette,seed,sizeof palette);palette.count=16;((unsigned char *)palette.records)[0x720]=128;memset(&copies,0,sizeof copies);memset(&dma,0,sizeof dma);
 FILE *f=fopen("outputs/crazy-taxi-native/reports/ordinary-native-assets.bin","rb");if(!f)return 3;for(unsigned j=0;j<11;j++)if(fread(&assets.bases[j],4,1,f)!=1||fread(&assets.palette_bases[j],4,1,f)!=1)return 3;fclose(f);
 TaxiOrdinaryGraphicsOwners g={0};g.assets=&assets;g.palette=&palette;g.rom=pkg.bytes;g.rom_size=pkg.size;g.destination=0x06014000;
 TaxiHudStartOwners o;memset(&o,seed,sizeof o);o.graphics=&g;o.copies=&copies;o.immediate=&dma;TaxiHudStartInput i={(uint8_t)demo,128,(uint8_t)driver,(uint8_t)driver,(uint8_t)driver};if(taxi_hud_start_owned(&o,&i))return 4;
 output(o.scalar_16c0,32);output(o.cache_5300,256);output(o.sprite_5490,8);output(palette.records,sizeof palette.records);output(assets.objects,sizeof assets.objects);printf(" %u %u %u %u\n",copies.count,dma.source,dma.destination,dma.control);
 }taxi_graphics_package_release(&pkg);return 0;}
