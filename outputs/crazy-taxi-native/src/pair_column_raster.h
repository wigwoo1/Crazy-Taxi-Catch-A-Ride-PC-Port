#ifndef TAXI_PAIR_COLUMN_RASTER_H
#define TAXI_PAIR_COLUMN_RASTER_H
#include <stdint.h>
#include <stddef.h>
/* Recovered ROM13E8..1410 inner loop. Fixed original indexed240x160 target.
   Native bounded raster/reference component; no CPU interpreter. */
typedef struct {uint32_t texture_byte_reads,framebuffer_halfword_writes;} TaxiPairColumnWork;
/* Logical original inner-loop transfers only, not CPU/bus cycle estimates. */
int taxi_pair_column_raster_work(uint8_t framebuffer[38400],size_t destination,
 const uint8_t *texture,size_t texture_size,uint32_t mask_x,uint32_t mask_y,
 unsigned shift_x,unsigned shift_y,uint32_t *x,uint32_t *y,
 uint32_t step_x,uint32_t step_y,unsigned rows,TaxiPairColumnWork *);
int taxi_pair_column_raster(uint8_t framebuffer[38400],size_t destination,
 const uint8_t *texture,size_t texture_size,uint32_t mask_x,uint32_t mask_y,
 unsigned shift_x,unsigned shift_y,uint32_t *x,uint32_t *y,
 uint32_t step_x,uint32_t step_y,unsigned rows);
#endif
