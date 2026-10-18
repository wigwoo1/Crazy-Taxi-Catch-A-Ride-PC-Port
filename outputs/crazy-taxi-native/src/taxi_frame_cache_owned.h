#ifndef TAXI_TAXI_FRAME_CACHE_OWNED_H
#define TAXI_TAXI_FRAME_CACHE_OWNED_H
#include "taxi_frame_copy_owned.h"
/* Original15910..15956 compares selected frame WORD to shared030053E0.
   Driver is not part of that cache key. Copy then publish selected frame,
   even if original deferred queue is full. Returns1 changed,0 equal,-1 hosterror. */
int taxi_frame_cache_owned(TaxiSpriteReplacementOwner *,uint32_t *previous,uint32_t selected,uint8_t driver,uint8_t immediate);
#endif
