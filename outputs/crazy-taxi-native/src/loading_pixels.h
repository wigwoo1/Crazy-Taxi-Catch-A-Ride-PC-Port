#ifndef TAXI_LOADING_PIXELS_H
#define TAXI_LOADING_PIXELS_H
#include "startup_obj_transport.h"
#include "palette_bank.h"
#include <stddef.h>
/* Native indexed bitmap plus scoped ordinary opaque priority0 4bpp OBJ.
   1D tiles, RGB555 output; zero mosaic extent only. Reject visible unsupported object formats.
   object_identity is the original address represented by objects[0]. */
int taxi_loading_pixels(const uint8_t [38400],const TaxiPaletteBanks *,const TaxiStartupObjTransport *,const uint8_t *,size_t,uint32_t,uint16_t,uint16_t [38400]);
#endif
