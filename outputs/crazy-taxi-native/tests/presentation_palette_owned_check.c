#ifdef NDEBUG
#undef NDEBUG
#endif
#include "host_ordinary_presentation.h"
#include "palette_registration_small.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static const uint16_t original[]={0x001f,0x03e0,0x7c00,0x7fff,0x0010,0x0200,0x4000,0x0008};
static const uint16_t alternate[]={0x5ef7,0x5ef7,0x5ef7,0x5ef7,0x318c,0x318c,0x318c,0x18c6};
typedef struct {TaxiPaletteBanks *banks;unsigned request,calls;} Context;
static void check_banks(const Context *c){
 for(unsigned i=0;i<256;i++){
  unsigned record=i/16;uint16_t expected=0x1234;
  if(record<13&&c->request==1)expected=original[record%8];
  if(record<13&&c->request==2&&record!=0&&record!=2&&record!=11)expected=alternate[record%8];
  assert(c->banks->objects[i]==expected);
  expected=c->request==1?original[13%8]:c->request==2?alternate[13%8]:0x1234;
  assert(c->banks->background[i]==expected);
 }
}
static int common(Context *c,TaxiPresentationService *s,uint32_t target,uint32_t arg){
 assert(c->calls<2&&target==(c->calls?0x08015c3c:0x08016208)&&arg==(c->calls?0u:1u));
 if(c->calls){assert(!s->pending_5f24);check_banks(c);}
 c->calls++;return 0;
}
static int legacy(void *context,TaxiPresentationService *s,uint32_t target,uint32_t arg){
 Context *c=context;int result=common(c,s,target,arg);s->current_3520+=c->calls;return result;
}
static int live(void *context,TaxiHostTime *host,TaxiPresentationService *s,uint32_t target,uint32_t arg){
 Context *c=context;assert(host->counter==s->current_3520);int result=common(c,s,target,arg);host->counter+=c->calls;return result;
}
static void source_owner(uint8_t rom[1024],TaxiPaletteRegistration *registry){
 uint16_t colors[256];TaxiPaletteTransfer request;registry->flags=4;
 for(unsigned bank=0;bank<14;bank++){
  unsigned count=bank==13?256:16,offset=bank==13?512:bank*32;
  for(unsigned j=0;j<count;j++){
   colors[j]=original[bank%8];rom[offset+j*2]=(uint8_t)colors[j];rom[offset+j*2+1]=(uint8_t)(colors[j]>>8);
  }
  int index=bank==13?taxi_palette_register(registry,colors,0x08000000+offset,32,1,&request):
   taxi_palette_register_small(registry,colors,0x08000000+offset,32,0,&request);
  assert(index==(int)bank);
 }
 /*22508 does not filter by active/cap; EE10 is inactive in this fixture. */
 registry->records[1][6]=0;registry->records[1][4]=0;
}
int main(void){
 unsigned count=0;const unsigned requests[]={0,1,2,3,255};
 for(unsigned method=0;method<2;method++)for(unsigned frozen=0;frozen<2;frozen++)
 for(unsigned wrap=0;wrap<2;wrap++)for(unsigned request=0;request<5;request++){
  uint8_t rom[1024]={0};TaxiPaletteRegistration registry={0};source_owner(rom,&registry);
  TaxiPaletteBanks banks;for(unsigned i=0;i<256;i++)banks.background[i]=banks.objects[i]=0x1234;
  TaxiTitleAssets assets={0};TaxiFontSetup fonts={0};TaxiStartupObjQueue queue={0};TaxiStartupObjTransport transport={0};
  uint16_t base[128][4]={{0}};TaxiObjMatrixSlot matrices[32]={0};
  TaxiOrdinaryGraphicsOwners graphics={&assets,&registry,&banks,&fonts,&queue,&transport,base,matrices,rom,sizeof rom,0x06014000};
  TaxiPaletteFadeStart fade={0};fade.masks.background_mask=fade.masks.object_mask=1;
  TaxiCityPaletteOwners palette={&registry,&banks,&fade,rom,sizeof rom,240,160};TaxiSpriteCopies copies={0};TaxiSpriteImmediateCopy immediate={0};uint16_t mosaic=7;
  Context context={&banks,requests[request],0};
  TaxiOrdinaryPresentationOwner owner={&graphics,&palette,&copies,&immediate,&mosaic,legacy,&context};
  uint32_t previous=wrap?UINT32_MAX-1:100;
  TaxiHostTime host={.counter=previous,.frame={.previous_3524=previous,.delta_3b00=7}};
  TaxiPresentationService service={.pending_5f24=(uint8_t)requests[request],.frozen_5f20=(uint8_t)frozen,.previous_3524=previous,.delta_3b00=7};
  int result=method?taxi_host_ordinary_presentation_serviced_commit(&host,&service,&owner,live,&context):
   taxi_host_ordinary_presentation_commit(&host,&service,&owner);
  assert(!result&&context.calls==2&&!service.pending_5f24);check_banks(&context);
  assert(host.counter==(uint32_t)(previous+3)&&host.frame.previous_3524==host.counter&&host.frame.delta_3b00==3);
  assert(host.frame.elapsed_755c==(frozen?0u:3u)&&host.frame.animation_1e10==4&&host.frame.page_7540==1&&mosaic==7);
  count++;
 }
 /* Late invalid record must not partly overwrite either bank. */
 uint8_t rom[1024]={0};TaxiPaletteRegistration registry={0};source_owner(rom,&registry);
 TaxiPaletteBanks banks={0},before=banks;TaxiCityPaletteOwners palette={.registry=&registry,.banks=&banks,.rom=rom,.rom_size=sizeof rom};
 registry.records[13][16]=0xff;registry.records[13][17]=0xff;registry.records[13][18]=0xff;registry.records[13][19]=0xff;
 assert(taxi_city_palette_gameplay_transfer(&palette,0)==-1&&!memcmp(&banks,&before,sizeof banks));
 assert(taxi_city_palette_gameplay_transfer(&palette,1)==-1&&!memcmp(&banks,&before,sizeof banks));
 assert(taxi_city_palette_gameplay_transfer(&palette,2)==-1&&!memcmp(&banks,&before,sizeof banks));
 /* Excluded records require no source resolution in alternate mode. */
 registry.count=1;memset(registry.records[0],255,20);
 assert(!taxi_city_palette_gameplay_transfer(&palette,1)&&!memcmp(&banks,&before,sizeof banks));
 assert(taxi_city_palette_gameplay_transfer(&palette,0)==-1&&!memcmp(&banks,&before,sizeof banks));
 /* Odd DMA word count copies the even prefix and retains the final color. */
 memset(&registry,0,sizeof registry);registry.count=1;registry.records[0][0]=3;registry.records[0][19]=8;
 rom[0]=0x1f;rom[1]=0;rom[2]=0xe0;rom[3]=3;rom[4]=0xff;rom[5]=0x7f;
 banks.objects[2]=0x4321;
 assert(!taxi_city_palette_gameplay_transfer(&palette,0)&&banks.objects[0]==0x001f&&banks.objects[1]==0x03e0&&banks.objects[2]==0x4321);
 assert(count==40);
 puts("PASS native palette22508 ownership: 40 active-bank presentations across legacy/live counters, request clearing/order, restore/alternate/exclusions, freeze/wrap; invalid rollback and odd-word retention");
 return 0;
}