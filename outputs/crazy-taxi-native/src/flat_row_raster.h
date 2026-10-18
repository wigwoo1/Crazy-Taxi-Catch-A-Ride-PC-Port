#ifndef TAXI_FLAT_ROW_RASTER_H
#define TAXI_FLAT_ROW_RASTER_H
#include <stdint.h>
typedef struct {uint32_t framebuffer_halfword_writes,framebuffer_word_writes;} TaxiFlatRowWork;
/* Original1F14..1F88 clipped flat row batch; explicit original transfer widths. */
int taxi_flat_row_batch(uint8_t framebuffer[38400],unsigned first,unsigned count,const uint32_t *descriptors,unsigned left,unsigned right,uint16_t pattern,TaxiFlatRowWork *work);
#endif
