#ifndef TAXI_BITMAP_FILL_H
#define TAXI_BITMAP_FILL_H
#include <stdint.h>
#include <stddef.h>
/* Source 26198: mode4 chooses first strictly closest RGB5 palette entry,
   with initial distance256/index0, then fills bank1. Mode3 fills bank0. */
int taxi_bitmap_fill(uint8_t mode,uint16_t width,uint16_t height,
 uint16_t red,uint16_t green,uint16_t blue,const uint16_t palette[256],
 uint8_t *bank0,uint8_t *bank1,size_t capacity);
#endif
