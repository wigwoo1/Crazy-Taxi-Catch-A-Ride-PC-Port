#include "../src/hud_start_owned.h"
#include "../src/taxi_graphics_package.h"
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
static TaxiTitleAssets assets;
static TaxiPaletteRegistration palette;
static TaxiSpriteCopies copies;
static TaxiSpriteImmediateCopy dma;
static int read_bytes(FILE *f,void *p,size_t n){return fread(p,1,n,f)==n?0:-1;}
static int output(const void *p,size_t n){return fwrite(p,1,n,stdout)==n?0:-1;}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 FILE *f=fopen(argv[1],"rb");if(!f)return 3;
 TaxiGraphicsPackage pkg={0};TaxiHudStartInput input={0};TaxiHudStartOwners owner={0};
 if(taxi_graphics_package_load(&pkg,"outputs/crazy-taxi-native/assets/taxis/graphics.ctg"))return 4;
 if(read_bytes(f,&input,sizeof input)||read_bytes(f,&palette.count,2)||read_bytes(f,&copies.count,2)||read_bytes(f,&dma,sizeof dma)||read_bytes(f,assets.bases,44)||read_bytes(f,owner.scalar_16c0,32)||read_bytes(f,owner.cache_5300,256)||read_bytes(f,owner.sprite_5490,8)||read_bytes(f,palette.records,2560)||read_bytes(f,assets.objects,32768)||fgetc(f)!=EOF)return 5;
 fclose(f);
 TaxiOrdinaryGraphicsOwners graphics={0};graphics.assets=&assets;graphics.palette=&palette;graphics.rom=pkg.bytes;graphics.rom_size=pkg.size;graphics.destination=0x06014000;
 owner.graphics=&graphics;owner.copies=&copies;owner.immediate=&dma;
 if(taxi_hud_start_owned(&owner,&input))return 6;
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 7;
 int rc=output(owner.scalar_16c0,32)||output(owner.cache_5300,256)||output(owner.sprite_5490,8)||output(palette.records,2560)||output(assets.objects,32768)||output(&copies.count,2)||output(&dma,sizeof dma);
 taxi_graphics_package_release(&pkg);return rc?8:0;
}
