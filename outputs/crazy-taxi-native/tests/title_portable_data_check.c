#define main legacy_title_load_main
#include "title-load-check.c"
#undef main
#include "../src/taxi_graphics_package.h"
#include "../src/title_runtime.h"
#include "../src/title_requests.h"
#include "../src/title_publish.h"
#include "../src/title_pixels.h"
#include "../src/startup_screens.h"
static TaxiTitleLoad full,sparse;
static TaxiStartupScreens screen_full,screen_sparse;
static uint16_t pixels_full[38400],pixels_sparse[38400];
int main(int argc,char **argv){
 if(argc!=3)return 20;
 char *legacy_args[2]={argv[0],argv[1]};if(legacy_title_load_main(2,legacy_args))return 21;
 TaxiGraphicsPackage package={0};if(taxi_graphics_package_load(&package,argv[2])||package.size!=sizeof rom)return 22;
 for(unsigned language=0;language<4;language++)for(unsigned dirty=0;dirty<2;dirty++){
  memset(&full,0,sizeof full);memset(full.background.graphics,dirty?165:0,sizeof full.background.graphics);memset(&full.banks,165,sizeof full.banks);memset(&full.fonts,165,sizeof full.fonts);memset(full.matrix_template,165,sizeof full.matrix_template);full.background.retained_blend=(TaxiPresentationBlend){0x1234,0x5678};full.fade.fade.interval_1771=(uint8_t)language;full.fade.fade.reload_1772=3;
  memcpy(&sparse,&full,sizeof full);
  if(taxi_title_load(&full,rom,sizeof rom,language)||taxi_title_load(&sparse,package.bytes,package.size,language)||memcmp(&full,&sparse,sizeof full))return 23;
  if(full.background.retained_blend.alpha_00e0!=half(rom+0x341dc)||full.background.retained_blend.alpha_00e2!=half(rom+0x341de))return 24;
  TaxiTitleRuntime a={0},b={0};uint32_t ar=0,br=0;if(taxi_title_entry(&a,&full)||taxi_title_entry(&b,&sparse))return 25;
  for(unsigned frame=0;frame<300;frame++){
   TaxiTitleRuntimeOutput ao={0},bo={0};
   if(taxi_title_runtime_frame(&a,&ar,0,0,&ao)||taxi_title_runtime_frame(&b,&br,0,0,&bo)||ar!=br||memcmp(&a,&b,sizeof a)||memcmp(&ao,&bo,sizeof ao))return 26;
   if(taxi_title_requests_queue(&full,&ao.callback,rom,sizeof rom,a.obj_palette_mask)||taxi_title_requests_queue(&sparse,&bo.callback,package.bytes,package.size,b.obj_palette_mask))return 27;
   if(taxi_title_objects_publish(&full)||taxi_title_objects_publish(&sparse)||memcmp(&full,&sparse,sizeof full))return 28;
   if(taxi_title_pixels(&full,(unsigned)a.callback.scroll.x_q8/256,a.bldy,pixels_full)||taxi_title_pixels(&sparse,(unsigned)b.callback.scroll.x_q8/256,b.bldy,pixels_sparse)||memcmp(pixels_full,pixels_sparse,sizeof pixels_full))return 29;
  }
  printf("PASS native title package language%u dirty%u 300frames retained%u/%u all owners/OAM/pixels match fullROM\n",language,dirty,(unsigned)full.background.retained_blend.alpha_00e0,(unsigned)full.background.retained_blend.alpha_00e2);
 }
 for(unsigned index=0;index<3;index++){
  const uint8_t *p=rom+0x49d560+index*28;TaxiScreenDescriptor d={0};d.image=word(p);d.timer=word(p+4);d.fade_in_bg=half(p+8);d.fade_in_obj=half(p+10);d.fade_out_bg=half(p+12);d.fade_out_obj=half(p+14);d.background=word(p+16);d.callback=word(p+20);d.type=(int8_t)p[24];d.flags=p[25];
  memset(&screen_full,0,sizeof screen_full);memset(&screen_sparse,0,sizeof screen_sparse);
  if(memcmp(p,package.bytes+0x49d560+index*28,28)||taxi_startup_screens_load(&screen_full,&d,rom+d.image-0x08000000u,sizeof rom-(d.image-0x08000000u))||taxi_startup_screens_load(&screen_sparse,&d,package.bytes+d.image-0x08000000u,package.size-(d.image-0x08000000u))||memcmp(&screen_full,&screen_sparse,sizeof screen_full))return 30;
  printf("PASS native startup bitmap%u packaged/fullROM complete loading owners equal\n",index);
 }
 taxi_graphics_package_release(&package);puts("PASS portable native title/startup data and retained header owners; full original menu-to-game handoff remains pending");return 0;
}
