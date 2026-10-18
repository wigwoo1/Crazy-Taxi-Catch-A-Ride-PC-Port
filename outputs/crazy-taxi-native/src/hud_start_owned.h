#ifndef TAXI_HUD_START_OWNED_H
#define TAXI_HUD_START_OWNED_H
#include "hud_start.h"
#include "ordinary_graphics_owned.h"
#include "sprite_replacement_owned.h"
typedef struct {
 uint8_t scalar_16c0[32],cache_5300[256],sprite_5490[8];
 TaxiOrdinaryGraphicsOwners *graphics;TaxiSpriteCopies *copies;TaxiSpriteImmediateCopy *immediate;
} TaxiHudStartOwners;
/* Original1EE28 native state writes and27FB8 effects; extracted immutable
   asset package supplies data only. Does not render or service frame timing. */
int taxi_hud_start_owned(TaxiHudStartOwners *,const TaxiHudStartInput *);
#endif
