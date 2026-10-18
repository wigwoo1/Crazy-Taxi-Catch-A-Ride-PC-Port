#ifndef TAXI_DISTANCE_BITMAP_H
#define TAXI_DISTANCE_BITMAP_H
#include <stdint.h>
/* Original318e6 ->2c918 call: 240x160 indexed render downsampled to64x64.
   This copies an already rendered image; scene generation remains external. */
void taxi_distance_bitmap_sample(uint8_t output[4096],const uint8_t source[38400]);
/* Original31902..31a24: border index comes from last material's byte14.
   Material selection/palette ownership must be resolved by the caller. */
void taxi_distance_bitmap_border(uint8_t output[4096],uint8_t material_color);
#endif
