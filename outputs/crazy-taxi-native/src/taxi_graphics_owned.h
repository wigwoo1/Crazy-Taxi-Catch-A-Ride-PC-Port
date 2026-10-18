#ifndef TAXI_GRAPHICS_OWNED_H
#define TAXI_GRAPHICS_OWNED_H
#include "ordinary_graphics_owned.h"
#include "taxi_frame_cache_owned.h"
#include "taxi_frame_palette_owned.h"
/* Connect qualified taxi cache/bank requests and independent palette update
   to allocated ordinary owners. Placement remains separate original calls. */
int taxi_graphics_owned(TaxiOrdinaryGraphicsOwners *,TaxiSpriteCopies *,TaxiSpriteImmediateCopy *,TaxiPaletteTransfer *,uint32_t *previous,uint32_t frame,uint8_t driver,uint8_t immediate,uint8_t override);
#endif
