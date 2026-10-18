#ifndef TAXI_CITY_BACKGROUND_H
#define TAXI_CITY_BACKGROUND_H
#include <stdint.h>
/* Native source BG0/secondary semitransparent footer composition.
   Input512x512 indexed map, BG palette256 colors/OBJ16 colors, strip32x16+8x16.
   Scroll wraps512. Palette cap precedes8/8RGB555 mixing. */
int taxi_city_background(uint8_t output[153600],const uint8_t indices[262144],
    const uint16_t bg_palette[256],const uint8_t strip[640],const uint16_t obj_palette[16],
    unsigned scroll_x,unsigned scroll_y,unsigned cap);
#endif
