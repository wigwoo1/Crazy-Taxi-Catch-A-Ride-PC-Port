#ifndef TAXI_SPRITE_REPLACEMENT_H
#define TAXI_SPRITE_REPLACEMENT_H
#include <stdint.h>
typedef struct {uint32_t source,destination,bytes;} TaxiSpriteCopy;
typedef struct {TaxiSpriteCopy records[64];uint16_t count;} TaxiSpriteCopies;
typedef struct {uint32_t group_count,frame_count,palette_units,destination_base;uint16_t width_tiles,height_tiles,tile_index;} TaxiSpriteCopyAsset;
typedef struct {uint32_t source,destination,control;} TaxiSpriteImmediateCopy;
/* Original27FB8 with caller-resolved immutable group/frame metadata.
 Source/destination identities are data; backend executes the copy. */
int taxi_sprite_replace(TaxiSpriteCopies *,const TaxiSpriteCopyAsset *,uint32_t source,uint32_t source_index,uint32_t frame,uint32_t group,uint32_t immediate,TaxiSpriteImmediateCopy *);
#endif
