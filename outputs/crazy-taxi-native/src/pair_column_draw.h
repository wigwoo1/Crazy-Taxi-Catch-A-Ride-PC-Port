#ifndef TAXI_PAIR_COLUMN_DRAW_H
#define TAXI_PAIR_COLUMN_DRAW_H
#include <stdint.h>
#include <stddef.h>
#include "pair_column_raster.h"
/* Recovered1304..1410 single column, original indexed240x160 target.
   Caller supplies descriptor, live clipping bounds and texture parameters.
   Returns1 drawn,0 source rejection,-1 invalid bounded native input. */
int taxi_pair_column_draw(uint8_t framebuffer[38400],unsigned column,const uint32_t descriptor[8],unsigned top,unsigned bottom,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy);
int taxi_pair_column_batch_work(uint8_t framebuffer[38400],unsigned first,unsigned count,const uint32_t *descriptors,unsigned top,unsigned bottom,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *);
int taxi_pair_column_batch(uint8_t framebuffer[38400],unsigned first,unsigned count,const uint32_t *descriptors,unsigned top,unsigned bottom,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy);
#endif
