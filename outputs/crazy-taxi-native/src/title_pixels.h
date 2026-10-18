#ifndef TAXI_TITLE_PIXELS_H
#define TAXI_TITLE_PIXELS_H
#include "title_load.h"
/* Normal titleBG0 plus ordinary opaque priority0 4bpp OBJ, 1D tiles, RGB555.
   Rejects visible affine/8bpp/semitransparent/window objects. */
int taxi_title_pixels(const TaxiTitleLoad *,unsigned,uint16_t,uint16_t[240*160]);
#endif
