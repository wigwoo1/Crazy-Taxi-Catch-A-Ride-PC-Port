#ifndef TAXI_SPRITE_REPLACEMENT_OWNED_H
#define TAXI_SPRITE_REPLACEMENT_OWNED_H
#include "sprite_replacement.h"
#include <stddef.h>
typedef struct {const uint8_t *rom;size_t rom_size;uint32_t resource;const uint32_t *group_bases;size_t base_count;TaxiSpriteCopies *copies;TaxiSpriteImmediateCopy *immediate;uint8_t *objects;size_t object_size;uint32_t object_identity;} TaxiSpriteReplacementOwner;
/* Resolve immutable resource/frame metadata, native27FB8, and immediate
   CPU copy into owned object storage. Deferred records retain identities. */
int taxi_sprite_replace_owned(TaxiSpriteReplacementOwner *,uint32_t source,uint32_t index,uint32_t frame,uint32_t group,uint32_t immediate);
#endif
