#ifndef TAXI_PAIR_ROW_RASTER_H
#define TAXI_PAIR_ROW_RASTER_H
#include "pair_column_raster.h"
/* Original1934..195C unshaded keyed two-pixel row transfers. */
int taxi_pair_row_raster(uint8_t framebuffer[38400],size_t destination,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,uint32_t *x,uint32_t *y,uint32_t dx,uint32_t dy,unsigned pairs,TaxiPairColumnWork *work);
int taxi_pair_row_batch(uint8_t framebuffer[38400],unsigned first,unsigned count,const uint32_t *descriptors,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work);
typedef struct {uint32_t x,y,dx,dy;unsigned column,pairs;} TaxiPairRowPrepared;
/* Original1640..16D8 divide/clip/advance, including accepted zero-width loop. */
int taxi_pair_row_prepare_bounds(TaxiPairRowPrepared *,const uint32_t descriptor[8],int32_t left,int32_t right);
int taxi_pair_row_prepare_clipped(TaxiPairRowPrepared *,const uint32_t descriptor[8],unsigned left,unsigned right);
int taxi_pair_row_batch_clipped(uint8_t framebuffer[38400],unsigned first,unsigned count,const uint32_t *descriptors,unsigned left,unsigned right,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work);
int taxi_pair_row_prepare_perspective(TaxiPairRowPrepared *,const uint32_t descriptor[8],int32_t left,int32_t right);
int taxi_pair_row_batch_perspective(uint8_t framebuffer[38400],unsigned first,unsigned count,const uint32_t *descriptors,unsigned left,unsigned right,const uint8_t *texture,size_t size,uint32_t mx,uint32_t my,unsigned sx,unsigned sy,TaxiPairColumnWork *work);
#endif
