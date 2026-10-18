#ifndef TAXI_LOADING_IMAGE_H
#define TAXI_LOADING_IMAGE_H
#include <stdint.h>
/* Original 08031ef8 flag1: 8-bit indices, zero preserves destination.
   Ordinary d764 content draws the same 64x64 image at y=0,60,120.
   Attract override and settings mode0x80 skip this content after clear. */
/* One original loading31EF8 request: x0,64x64,index0transparent.
   Supported source anchors y0/60/120; returns-1 outside this domain. */
int taxi_loading_image_placement(uint8_t output[38400],const uint8_t image[4096],unsigned y);
void taxi_loading_image(uint8_t output[38400], const uint8_t image[4096]);
/* Clear color is caller-owned original03006900 low byte; palette is the
   registered scene palette. Mode1 fade caps RGB555 before expansion. */
void taxi_loading_background(uint8_t output[153600], const uint8_t image[4096],
    const uint16_t palette[256], uint8_t clear_index, int16_t progress);
#endif
