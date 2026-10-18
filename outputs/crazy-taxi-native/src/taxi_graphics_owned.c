#include "taxi_graphics_owned.h"
int taxi_graphics_owned(TaxiOrdinaryGraphicsOwners *g,TaxiSpriteCopies *copies,TaxiSpriteImmediateCopy *dma,TaxiPaletteTransfer *palette_dma,uint32_t *previous,uint32_t frame,uint8_t driver,uint8_t immediate,uint8_t override){
 if(!g||!g->assets||!g->rom||!g->palette||!g->banks)return -1;
 TaxiSpriteReplacementOwner sprite={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,copies,dma,g->assets->objects,sizeof g->assets->objects,g->destination};
 if(taxi_frame_cache_owned(&sprite,previous,frame,driver,immediate)<0)return -1;
 if(taxi_frame_palette_owned(g->rom,g->rom_size,g->palette,g->banks,palette_dma,immediate,driver,override)<0)return -1;
 return 0;
}
