#ifndef TAXI_START_ART_OWNED_H
#define TAXI_START_ART_OWNED_H
#include "ordinary_graphics_owned.h"
#include "sprite_replacement_owned.h"
int taxi_start_art_replace_owned(TaxiOrdinaryGraphicsOwners *,TaxiSpriteCopies *,TaxiSpriteImmediateCopy *,int16_t,uint8_t);
/* Decode published group7 objects, three32x32 frames; caller owns28EE0 flush. */
int taxi_start_art_decode_owned(uint8_t out[3][4096],const TaxiOrdinaryGraphicsOwners *);
#endif
