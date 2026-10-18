#ifndef TAXI_RANK_BITMAP_H
#define TAXI_RANK_BITMAP_H
#include <stdint.h>
typedef struct {int32_t x,y;uint16_t width,height;} TaxiRankBitmapRect;
/* Original4F6C frame0, fixed36x32 rank panel atY88, source viewport240x160.
   Source input domainX>=0 includes the full reachable rank animation200..242.
   ARM7 word stores ignore low two address bits. Original clipping precedes
   complete-word copying. NegativeX is rejected, not silently approximated. */
int taxi_rank_bitmap_rect(TaxiRankBitmapRect *,int32_t source_x);
#endif
