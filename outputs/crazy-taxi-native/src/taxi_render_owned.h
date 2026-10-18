#ifndef TAXI_RENDER_OWNED_H
#define TAXI_RENDER_OWNED_H
#include "taxi_graphics_owned.h"
#include "taxi_placement_requests.h"
#include "taxi_affine_queue.h"
/* Ordinary mode0 only: cache/copy, both28658 placements, then palette.
   Upstream projection supplies half positions/scale. Trig resolves -roll. */
int taxi_render_owned(TaxiOrdinaryGraphicsOwners *,TaxiSpriteCopies *,TaxiSpriteImmediateCopy *,TaxiPaletteTransfer *,uint32_t *previous,uint32_t frame,uint8_t driver,uint8_t immediate,uint8_t override,const uint32_t left[3],const uint32_t right[3],uint32_t scale,uint16_t roll,int32_t sine,int32_t cosine,uint16_t width,uint16_t height,uint16_t mask);
#endif
