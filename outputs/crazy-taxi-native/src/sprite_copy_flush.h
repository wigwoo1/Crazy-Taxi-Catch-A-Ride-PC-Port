#ifndef TAXI_SPRITE_COPY_FLUSH_H
#define TAXI_SPRITE_COPY_FLUSH_H
#include "sprite_replacement.h"
typedef int (*TaxiSpriteCopyTransfer)(void *,const TaxiSpriteImmediateCopy *);
/* Original28EE0 valid producer domain (0..64 records). Queue bytes retained.
   DMA requests execute in order; count clears only after all transfers. */
int taxi_sprite_copy_flush(TaxiSpriteCopies *,TaxiSpriteImmediateCopy *,TaxiSpriteCopyTransfer,void *);
#endif
