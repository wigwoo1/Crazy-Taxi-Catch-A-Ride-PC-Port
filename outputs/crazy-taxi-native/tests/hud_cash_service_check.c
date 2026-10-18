#include "hud_cash_pixels.h"
#include "taxi_graphics_package.h"
#include "host_ordinary_presentation.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static TaxiTitleAssets assets;static TaxiPaletteRegistration registry;static TaxiPaletteBanks banks;static TaxiFontSetup fonts;static TaxiStartupObjQueue queue;static TaxiStartupObjTransport transport;static uint16_t base[128][4];static TaxiObjMatrixSlot matrices[32];
typedef struct {TaxiSpriteCopies *copies;unsigned calls;uint8_t before[32768];} Platform;
static int platform(void *context,TaxiPresentationService *s,uint32_t target,uint32_t arg){
 Platform *p=context;const uint32_t targets[]={0x08016208,0x08022508,0x08015c3c};
 assert(p->calls<3&&target==targets[p->calls]);assert(arg==(p->calls==2?0:1));
 assert(p->copies->count==16&&!memcmp(p->before,assets.objects,sizeof p->before));assert(queue.total==1);
 p->calls++;if(target==0x08016208)s->current_3520+=5;return 0;
}
int main(void){
 TaxiGraphicsPackage package={0};assert(!taxi_graphics_package_load(&package,"outputs/crazy-taxi-native/assets/taxis/graphics.ctg"));
 registry.flags=4;TaxiOrdinaryGraphicsOwners g={&assets,&registry,&banks,&fonts,&queue,&transport,base,matrices,package.bytes,package.size,0x06014000};assert(!taxi_ordinary_graphics_owned(&g,1));assert(!taxi_hud_cash_palette_sources(&registry));assert(!taxi_hud_cash_palette_init(&g));
 uint8_t atlas[161][512];FILE *file=fopen("outputs/crazy-taxi-native/assets/hud/cash.rgba","rb");assert(file&&fread(atlas,1,sizeof atlas,file)==sizeof atlas&&fgetc(file)==EOF);assert(!fclose(file));
 TaxiPaletteFadeStart fade={0};fade.masks.background_mask=fade.masks.object_mask=1;
 TaxiCityPaletteOwners palette={&registry,&banks,&fade,g.rom,g.rom_size,240,160};TaxiSpriteCopies copies={0};TaxiSpriteImmediateCopy dma={0};uint16_t mosaic=0;Platform p={.copies=&copies};
 TaxiOrdinaryPresentationOwner owner={&g,&palette,&copies,&dma,&mosaic,platform,&p};TaxiPresentationService s={0};s.mode_7541=4;s.first_754c=0x06000000;s.second_7550=0x0600a000;s.previous_3524=100;s.delta_3b00=1;TaxiHostTime host={.counter=100,.frame={.previous_3524=100,.delta_3b00=1}};
 uint8_t scratch[64]={0},foreign[64];memset(foreign,0x5a,64);TaxiSpriteCopySource sources[]={{0x03005400,scratch,64},{0x03005500,foreign,64}};
 unsigned compared=0;
 for(unsigned frame=0;frame<81;frame++){
  TaxiCashGlyphs glyphs;taxi_cash_glyphs_init(&glyphs);if(frame<80){for(unsigned i=0;i<10;i++)glyphs.total[i]=frame%60;for(unsigned i=0;i<9;i++)glyphs.pending[i]=frame;}
  TaxiCashGlyphs original=glyphs;memcpy(p.before,assets.objects,sizeof p.before);p.calls=0;
  copies.count=1;copies.records[0]=(TaxiSpriteCopy){0x03005500,g.destination+32704,64};assert(!taxi_hud_cash_enqueue(&g,&copies,&dma,&glyphs));
  queue.count[1]=queue.total=1;queue.attributes[1][0][0]=77;queue.attributes[1][0][1]=88;s.pending_5f24=2;
  assert(!taxi_host_ordinary_presentation_sources_commit(&host,&s,&owner,sources,2));
  assert(p.calls==3&&!copies.count&&!queue.total&&transport.output[0][0]==77&&transport.output[0][1]==88);
  assert(host.counter==105+5*frame&&host.frame.delta_3b00==5&&host.frame.elapsed_755c==5*(frame+1)&&host.frame.animation_1e10==4*(frame+1)&&host.frame.page_7540==((frame+1)&1));
  assert(!memcmp(assets.objects+32704,foreign,64)&&!memcmp(&glyphs,&original,sizeof glyphs));
  TaxiPaletteBanks saved=banks;uint8_t pixels[19][512];assert(!taxi_hud_cash_decode(pixels,&g)&&!memcmp(&saved,&banks,sizeof banks));
  for(unsigned i=0;i<19;i++){unsigned slot=i<10?i:i-10;int punctuation=slot==(i<10?3u:2u)||slot==(i<10?7u:6u);if(frame==80&&!punctuation){for(unsigned j=0;j<128;j++)assert(!pixels[i][j*4+3]);}else{unsigned expected=i<10?(punctuation?60+slot:frame%60):70+(punctuation?80+slot:frame);assert(!memcmp(pixels[i],atlas[expected],512));}compared++;}
 }

 /* Active registered fade applies the previous service delta before the tail. */
 assert(!taxi_palette_fade_start_normal(&fade,0,255,1,1,240,160));
 fade.fade.interval_1771=fade.fade.reload_1772=1;
 for(unsigned r=0;r<registry.count;r++){const uint8_t *record=registry.records[r];unsigned offset=record[2]|(unsigned)record[3]<<8;if(!record[7]&&(offset==288||offset==320)){uint32_t id=record[16]|(uint32_t)record[17]<<8|(uint32_t)record[18]<<16|(uint32_t)record[19]<<24;printf("CASH REGISTRY bank%u source%08x runtime%08x\n",offset/32,id,offset==288?0x08290af8u:0x08290b40u);}}
 unsigned active_frames=0,changed_banks=0;
 for(unsigned step=0;step<7;step++){
  int16_t applied=fade.fade.progress_178a;TaxiPaletteBanks expected=banks;
  for(unsigned r=0;r<registry.count;r++){
   const uint8_t *record=registry.records[r];if(!record[6])continue;
   unsigned colors=record[0]|(unsigned)record[1]<<8,offset=(record[2]|(unsigned)record[3]<<8)/2;
   uint32_t id=record[16]|(uint32_t)record[17]<<8|(uint32_t)record[18]<<16|(uint32_t)record[19]<<24;
   assert(id>=0x08000000&&id-0x08000000+colors*2<=g.rom_size);
   uint16_t *destination=(record[7]?expected.background:expected.objects)+offset;
   for(unsigned c=0;c<(colors&~1u);c++){size_t at=id-0x08000000+c*2;uint16_t raw=(uint16_t)(g.rom[at]|(uint16_t)g.rom[at+1]<<8);destination[c]=taxi_palette_fade_cap_rgb555(raw,applied);}
  }
  TaxiCashGlyphs glyphs;taxi_cash_glyphs_init(&glyphs);for(unsigned i=0;i<10;i++)glyphs.total[i]=55;for(unsigned i=0;i<9;i++)glyphs.pending[i]=79;
  memcpy(p.before,assets.objects,sizeof p.before);p.calls=0;copies.count=1;copies.records[0]=(TaxiSpriteCopy){0x03005500,g.destination+32704,64};assert(!taxi_hud_cash_enqueue(&g,&copies,&dma,&glyphs));queue.count[1]=queue.total=1;queue.attributes[1][0][0]=77;queue.attributes[1][0][1]=88;s.pending_5f24=2;
  assert(!taxi_host_ordinary_presentation_sources_commit(&host,&s,&owner,sources,2));
  assert(!memcmp(&banks,&expected,sizeof banks));int16_t next=applied>5?applied-5:0;assert(fade.fade.progress_178a==next);
  uint8_t pixels[19][512];TaxiPaletteBanks published=banks;assert(!taxi_hud_cash_decode(pixels,&g)&&!memcmp(&published,&banks,sizeof banks));
  for(unsigned i=0;i<19;i++){unsigned slot=i<10?i:i-10;int punctuation=slot==(i<10?3u:2u)||slot==(i<10?7u:6u);unsigned index=i<10?(punctuation?60+slot:55):70+(punctuation?80+slot:79);uint8_t expected_pixels[512];taxi_palette_fade_cap_rgba(atlas[index],expected_pixels,128,applied);assert(!memcmp(pixels[i],expected_pixels,512));}
  if(applied<31){changed_banks++;for(unsigned bank=9;bank<=10;bank++)for(unsigned c=0;c<16;c++){uint16_t color=banks.objects[bank*16+c];assert((color&31)<=(unsigned)applied&&((color>>5)&31)<=(unsigned)applied&&((color>>10)&31)<=(unsigned)applied);}}
  assert(!copies.count&&!queue.total&&p.calls==3);active_frames++;
 }
 assert(active_frames==7&&changed_banks==6);puts("PASS active registered service fade: seven publications, cap before previous-delta step, published cash palette retained; registry setup is packaged fixture");
 /* Already published capped colors must not be reset by decode. */
 for(unsigned i=0;i<256;i++)banks.objects[i]=taxi_palette_fade_cap_rgb555(banks.objects[i],17);
 TaxiPaletteBanks saved=banks;uint8_t pixels[19][512];assert(!taxi_hud_cash_decode(pixels,&g)&&!memcmp(&saved,&banks,sizeof banks));
 taxi_graphics_package_release(&package);printf("PASS cash/frame-service composition:81 ordered publications, %u slot comparisons, OAM reset, mixed copies, single clock tail and read-only published palette; platform clock fixture, not live cadence proof\n",compared);return 0;
}
