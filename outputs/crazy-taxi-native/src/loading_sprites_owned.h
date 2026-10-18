#ifndef TAXI_LOADING_SPRITES_OWNED_H
#define TAXI_LOADING_SPRITES_OWNED_H
#include "loading_indexed_owned.h"
#include "sprite_replacement_owned.h"
typedef struct {TaxiLoadingIndexedOwner indexed;TaxiSpriteReplacementOwner *sprites;} TaxiLoadingSpritesOwner;
/* Loading27FB8 executes native source metadata and immediateobjectcopy.
   Allocation/group bases already belong to original caller's asset owner. */
int taxi_loading_sprites_owned(TaxiLoadingPresent *,const TaxiLoadingSpritesOwner *);
#endif
