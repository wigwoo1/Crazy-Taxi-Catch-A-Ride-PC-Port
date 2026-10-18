#include "hud_cash_pixels.h"
#include "taxi_graphics_package.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static TaxiTitleAssets assets;static TaxiPaletteRegistration registry;static TaxiPaletteBanks banks;static TaxiFontSetup fonts;static TaxiStartupObjQueue queue;static TaxiStartupObjTransport transport;static uint16_t base[128][4];static TaxiObjMatrixSlot matrices[32];
int main(void){TaxiGraphicsPackage package={0};assert(!taxi_graphics_package_load(&package,"outputs/crazy-taxi-native/assets/taxis/graphics.ctg"));registry.flags=4;TaxiOrdinaryGraphicsOwners g={&assets,&registry,&banks,&fonts,&queue,&transport,base,matrices,package.bytes,package.size,0x06014000};assert(!taxi_ordinary_graphics_owned(&g,1));uint8_t atlas[161][512];FILE *file=fopen("outputs/crazy-taxi-native/assets/hud/cash.rgba","rb");assert(file&&fread(atlas,1,sizeof atlas,file)==sizeof atlas&&fgetc(file)==EOF);assert(!fclose(file));unsigned compared=0;for(unsigned frame=0;frame<80;frame++){TaxiCashGlyphs glyphs;taxi_cash_glyphs_init(&glyphs);for(unsigned i=0;i<10;i++)glyphs.total[i]=(uint16_t)(frame%60);for(unsigned i=0;i<9;i++)glyphs.pending[i]=(uint16_t)frame;TaxiCashGlyphs before=glyphs;uint8_t pixels[19][512];assert(!taxi_hud_cash_pixels(pixels,&g,&glyphs)&&!memcmp(&before,&glyphs,sizeof glyphs));for(unsigned i=0;i<19;i++){unsigned slot=i<10?i:i-10;unsigned expected=i<10?((slot==3||slot==7)?60+slot:frame%60):70+((slot==2||slot==6)?80+slot:frame);assert(!memcmp(pixels[i],atlas[expected],512));compared++;}}TaxiCashGlyphs blank;taxi_cash_glyphs_init(&blank);uint8_t pixels[19][512];assert(!taxi_hud_cash_pixels(pixels,&g,&blank));for(unsigned i=0;i<19;i++){unsigned slot=i<10?i:i-10;if(slot==(i<10?3u:2u)||slot==(i<10?7u:6u))continue;for(unsigned p=0;p<128;p++)assert(pixels[i][p*4+3]==0);}TaxiSpriteCopies shared={0};TaxiSpriteImmediateCopy dma={0};
 uint8_t foreign[64],scratch[64]={0},object_before[32768];memset(foreign,0x5a,sizeof foreign);
 shared.count=1;shared.records[0].source=0x03005500;shared.records[0].destination=g.destination+32704;shared.records[0].bytes=64;
 memcpy(object_before,assets.objects,sizeof object_before);TaxiSpriteCopy first_record=shared.records[0];TaxiCashGlyphs blank_before=blank;
 assert(!taxi_hud_cash_enqueue(&g,&shared,&dma,&blank));assert(shared.count==16);
 assert(!memcmp(&first_record,shared.records,sizeof first_record)&&!memcmp(object_before,assets.objects,sizeof object_before)&&!memcmp(&blank_before,&blank,sizeof blank));
 TaxiSpriteCopySource sources[]={{0x03005400,scratch,64},{0x03005500,foreign,64}};
 TaxiSpriteCopyFlushOwner flush={g.rom,g.rom_size,assets.objects,sizeof assets.objects,g.destination,&shared,&dma};
 assert(!taxi_sprite_copy_flush_sources_owned(&flush,sources,2)&&!shared.count);
 assert(!memcmp(assets.objects+32704,foreign,64));assert(!taxi_hud_cash_decode(pixels,&g));
 for(unsigned i=0;i<19;i++){unsigned slot=i<10?i:i-10;if(slot==(i<10?3u:2u)||slot==(i<10?7u:6u))continue;for(unsigned p=0;p<128;p++)assert(!pixels[i][p*4+3]);}
 printf("PASS deferred cash publication: existing foreign request retained, no object mutation before flush, all15 blank transfers published together\n");
 taxi_graphics_package_release(&package);printf("PASS native queued cash slot decoding: %u RGBA slots exact source atlas; blank alpha and glyph state retained\n",compared);return 0;}
