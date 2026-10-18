#include "hud_cash_pixels.h"
#include "hud_digit_copies_owned.h"
#include "sprite_copy_flush_owned.h"
#include "obj_tiles_rgba.h"
#include <string.h>
static int valid_glyphs(const TaxiCashGlyphs *glyphs){
 if(!glyphs)return -1;
 for(unsigned i=0;i<10;i++)if(i!=3&&i!=7&&glyphs->total[i]!=65535&&glyphs->total[i]>=60)return -1;
 for(unsigned i=0;i<9;i++)if(i!=2&&i!=6&&glyphs->pending[i]!=65535&&glyphs->pending[i]>=80)return -1;
 return 0;
}
int taxi_hud_cash_enqueue(TaxiOrdinaryGraphicsOwners *g,TaxiSpriteCopies *copies,TaxiSpriteImmediateCopy *dma,const TaxiCashGlyphs *glyphs){
 if(!g||!g->assets||!g->rom||g->rom_size<0x290b60||!copies||!dma||valid_glyphs(glyphs))return -1;
 int16_t first[10],second[9];memcpy(first,glyphs->total,sizeof first);memcpy(second,glyphs->pending,sizeof second);
 TaxiSpriteReplacementOwner replacement={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,copies,dma,g->assets->objects,sizeof g->assets->objects,g->destination};
 return taxi_hud_digit_copies_owned(&replacement,first,second);
}
int taxi_hud_cash_palette_sources(TaxiPaletteRegistration *registry){
 if(!registry||registry->count<11||registry->count>128)return -1;
 const uint32_t sources[2]={0x08290af8,0x08290b40};
 for(unsigned i=0;i<2;i++)for(unsigned byte=0;byte<4;byte++)registry->records[9+i][16+byte]=(uint8_t)(sources[i]>>(byte*8));
 return 0;
}
int taxi_hud_cash_palette_init(TaxiOrdinaryGraphicsOwners *g){
 if(!g||!g->banks||!g->rom||g->rom_size<0x290b60)return -1;
 for(unsigned group=5;group<=6;group++){
  unsigned bank=group==5?9:10;size_t palette=group==5?0x290af8:0x290b40;
  for(unsigned i=0;i<16;i++)g->banks->objects[bank*16+i]=(uint16_t)(g->rom[palette+i*2]|(uint16_t)g->rom[palette+i*2+1]<<8);
 }return 0;
}
int taxi_hud_cash_decode(uint8_t out[19][512],TaxiOrdinaryGraphicsOwners *g){
 if(!out||!g||!g->assets||!g->banks||!g->rom||g->rom_size<0x290b60)return -1;
 for(unsigned group=5;group<=6;group++){
  unsigned bank=group==5?9:10,slots=group==5?10:9,offset=group==5?0:10;
  for(unsigned slot=0;slot<slots;slot++){
   if(g->assets->bases[group]<0x06010000u)return -1;uint32_t tile=(g->assets->bases[group]-0x06010000u)/32+slot*2;
   if(tile>1023||taxi_obj_tiles_rgba(out[offset+slot],512,g->assets,g->banks,g->destination,(uint16_t)(tile|(bank<<12)),8,16))return -1;
  }
 }return 0;
}

int taxi_hud_cash_pixels(uint8_t out[19][512],TaxiOrdinaryGraphicsOwners *g,const TaxiCashGlyphs *glyphs){
 if(!out||!g||!g->banks)return -1;
 TaxiSpriteCopies copies={0};TaxiSpriteImmediateCopy dma={0};
 if(taxi_hud_cash_enqueue(g,&copies,&dma,glyphs))return -1;
 uint8_t scratch[64]={0};TaxiSpriteCopySource source={0x03005400,scratch,sizeof scratch};TaxiSpriteCopyFlushOwner flush={g->rom,g->rom_size,g->assets->objects,sizeof g->assets->objects,g->destination,&copies,&dma};
 if(taxi_sprite_copy_flush_sources_owned(&flush,&source,1))return -1;
 if(taxi_hud_cash_palette_init(g))return -1;
 return taxi_hud_cash_decode(out,g);
}
