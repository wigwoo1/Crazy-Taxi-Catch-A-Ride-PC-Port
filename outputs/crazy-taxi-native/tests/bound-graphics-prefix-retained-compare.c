#include "../src/ordinary_graphics_owned.h"
#include "../src/taxi_graphics_package.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "../src/city_palette_owned.h"
static TaxiTitleAssets assets;
static TaxiPaletteRegistration registry;
static TaxiPaletteBanks banks;
static TaxiFontSetup fonts;
static TaxiStartupObjQueue queue;
static TaxiStartupObjTransport transport;
static uint16_t base[128][4];static TaxiObjMatrixSlot matrices[32];
static void put_half(unsigned char *p,unsigned v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
static int read_bytes(FILE *f,void *p,size_t n){return fread(p,1,n,f)==n?0:-1;}
static int output(const void *p,size_t n){return fwrite(p,1,n,stdout)==n?0:-1;}
int main(int argc,char **argv){
 if(argc!=2&&argc!=3)return 2;unsigned frames=argc==3?(unsigned)atoi(argv[2]):0;if(frames>40)return 2;FILE *f=fopen(argv[1],"rb");if(!f)return 3;
 unsigned char globals[32];TaxiPaletteFadeStart fade={0};
 uint16_t palette[256];TaxiGraphicsPackage pkg={0};
 if(taxi_graphics_package_load(&pkg,"outputs/crazy-taxi-native/assets/taxis/graphics.ctg"))return 4;
 if(read_bytes(f,&registry.count,2)||read_bytes(f,&registry.bg_bytes,2)||read_bytes(f,&registry.obj_bytes,2)||read_bytes(f,&registry.flags,1)||read_bytes(f,registry.records,2560)||read_bytes(f,&fonts,sizeof fonts)||read_bytes(f,assets.objects,32768)||read_bytes(f,&banks,sizeof banks)||read_bytes(f,palette,512)||read_bytes(f,globals,32)||read_bytes(f,&fade.motion,24)||read_bytes(f,&fade.bldcnt,2)||read_bytes(f,&fade.bldy,2)||fgetc(f)!=EOF)return 5;fclose(f);
 TaxiOrdinaryGraphicsOwners owner={&assets,&registry,&banks,&fonts,&queue,&transport,base,matrices,pkg.bytes,pkg.size,0x06014000};
 fade.fade.interval_1771=globals[1];fade.fade.reload_1772=globals[2];
 if(taxi_bound_graphics_start_owned(&owner,1,palette,0x087557ac,&fade))return 6;
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 7;
 int rc=output(assets.bases,44)||output(assets.palette_bases,44)||output(assets.objects,32768)||output(registry.records,2560)||output(&fonts,sizeof fonts)||output(&banks,sizeof banks)||output(&registry.count,2)||output(&registry.bg_bytes,2)||output(&registry.obj_bytes,2);
 globals[1]=fade.fade.interval_1771;globals[2]=fade.fade.reload_1772;globals[3]=fade.masks.mode;globals[4]=fade.masks.endpoint_count;
 memcpy(globals+5,fade.masks.background_channels,3);memcpy(globals+8,fade.masks.object_channels,3);globals[11]=fade.fade.active_177b;
 put_half(globals+12,fade.scale_x_177c);put_half(globals+14,fade.scale_y_177e);
 put_half(globals+16,registry.count);put_half(globals+18,registry.bg_bytes);put_half(globals+20,registry.obj_bytes);
 put_half(globals+22,fade.masks.background_mask);put_half(globals+24,fade.masks.object_mask);put_half(globals+26,(uint16_t)fade.fade.progress_178a);put_half(globals+28,fade.masks.cap);
 rc=rc||output(globals,32)||output(&fade.motion,24)||output(&fade.bldcnt,2)||output(&fade.bldy,2);
 TaxiCityPaletteOwners city={&registry,&banks,&fade,pkg.bytes,pkg.size,240,160};uint16_t mosaic=0x3456;
 for(unsigned i=0;i<frames;i++){
  if(taxi_city_palette_frame(&city,1,&mosaic))return 9;
  unsigned char timing[5]={fade.fade.mode_1773,fade.fade.active_177b,fade.fade.endpoint_count_1774,fade.fade.interval_1771,fade.fade.reload_1772};
  rc=rc||output(&banks,sizeof banks)||output(&fade.fade.progress_178a,2)||output(timing,5)||output(&mosaic,2)||output(&fade.motion,24);
 }
 taxi_graphics_package_release(&pkg);return rc?8:0;
}
