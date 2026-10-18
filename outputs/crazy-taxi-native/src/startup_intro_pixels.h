#ifndef TAXI_STARTUP_INTRO_PIXELS_H
#define TAXI_STARTUP_INTRO_PIXELS_H
#include "startup_intro.h"
#include <stddef.h>
/* Native source-resolution compositor. Caller supplies original resolved BG
   indices: BG0 512x256, BG1 256x256 padded map; shared palette and OBJ tiles.
   Source display/fade/loader integration remains separate. */
int taxi_startup_intro_pixels(const TaxiStartupIntro *,const uint8_t *bg0,
 const uint8_t *bg1,const uint16_t bg_palette[256],const uint8_t *tiles,
 size_t tiles_size,uint16_t output[240*160]);
#endif
