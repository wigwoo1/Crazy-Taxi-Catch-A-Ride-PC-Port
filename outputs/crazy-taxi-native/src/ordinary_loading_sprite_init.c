#include "ordinary_loading_sprite_init.h"
int taxi_ordinary_loading_sprite_init(TaxiOrdinaryGraphicsOwners *g,TaxiSpriteCopies *copies,TaxiSpriteImmediateCopy *dma,uint32_t retained_sl){
 if(!g||!g->assets)return -1;
 TaxiSpriteReplacementOwner owner={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,copies,dma,g->assets->objects,sizeof g->assets->objects,g->destination};
 for(unsigned i=0;i<3;i++)if(taxi_sprite_replace_owned(&owner,0x080fcff8,i,i,7,retained_sl)<0)return -1;
 return taxi_sprite_replace_owned(&owner,0x080fd5fc,0,0,2,retained_sl)<0?-1:0;
}
