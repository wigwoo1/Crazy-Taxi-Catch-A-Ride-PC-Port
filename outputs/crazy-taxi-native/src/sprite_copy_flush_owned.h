#ifndef TAXI_SPRITE_COPY_FLUSH_OWNED_H
#define TAXI_SPRITE_COPY_FLUSH_OWNED_H
#include "sprite_copy_flush.h"
#include <stddef.h>
typedef struct {const uint8_t *rom;size_t rom_size;uint8_t *objects;size_t object_size;uint32_t object_identity;TaxiSpriteCopies *copies;TaxiSpriteImmediateCopy *immediate;} TaxiSpriteCopyFlushOwner;
/* ROM-backed DMA32 logical transfers performed with native CPU copies.
   Validate all pending ranges before mutation; malformed host inputs fail. */
int taxi_sprite_copy_flush_owned(TaxiSpriteCopyFlushOwner *);
typedef struct {uint32_t identity;const uint8_t *bytes;size_t size;} TaxiSpriteCopySource;
int taxi_sprite_copy_flush_sources_owned(TaxiSpriteCopyFlushOwner *,const TaxiSpriteCopySource *,size_t);
#endif
