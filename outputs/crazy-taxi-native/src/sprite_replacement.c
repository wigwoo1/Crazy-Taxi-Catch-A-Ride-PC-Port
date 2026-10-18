#include "sprite_replacement.h"
int taxi_sprite_replace(TaxiSpriteCopies *queue,const TaxiSpriteCopyAsset *asset,uint32_t source,uint32_t source_index,uint32_t frame,uint32_t group,uint32_t immediate,TaxiSpriteImmediateCopy *dma){
 if(!queue||!asset||!dma)return -1;
 if((uint16_t)group>=asset->group_count||(uint16_t)frame>=asset->frame_count)return 0;
 uint32_t tile_bytes=asset->palette_units==16?32:64;
 uint32_t bytes=(uint32_t)asset->width_tiles*asset->height_tiles*tile_bytes;
 uint32_t from=source+(uint16_t)source_index*bytes,to=asset->destination_base+asset->tile_index*tile_bytes;
 if((uint8_t)immediate){*dma=(TaxiSpriteImmediateCopy){from,to,0x84000000u|(bytes/4)};return 1;}
 if(queue->count>63)return 0;
 queue->records[queue->count++]=(TaxiSpriteCopy){from,to,bytes};return 1;
}
