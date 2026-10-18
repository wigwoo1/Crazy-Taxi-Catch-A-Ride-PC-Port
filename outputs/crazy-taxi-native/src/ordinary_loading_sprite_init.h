#ifndef TAXI_ORDINARY_LOADING_SPRITE_INIT_H
#define TAXI_ORDINARY_LOADING_SPRITE_INIT_H
#include "ordinary_graphics_owned.h"
#include "sprite_replacement_owned.h"
/* Original ADEE..AE30 four sprite replacement requests only.
   Scene/display/camera setup remains separate. */
int taxi_ordinary_loading_sprite_init(TaxiOrdinaryGraphicsOwners *,TaxiSpriteCopies *,TaxiSpriteImmediateCopy *,uint32_t retained_sl);
#endif
